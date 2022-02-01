#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"
#include "math_functions.h"
#include "thread_runner.h"
#pragma warning(disable : 4996)

static HANDLE min_free_n_param_mutex_handle = NULL;
static HANDLE writing_threesome_mutex_handle = NULL;
static HANDLE empty = NULL;
static HANDLE full = NULL;

static primitive_pitagoras_threesome *p_first_threesome = 0;

static int computation_threads_are_done = 0;
static int threesome_array_next_free_slot = 0;
static int num_of_ordered_threesomes = 0;
static int max_number_of_m;
static int output_buffer_size;

static primitive_pitagoras_threesome *threesome_array;
static valid_mutex_struct *mutex_array;

static int min_free_n = 1;/* A shared resource - a variable accessed from different threads. */
static int stop = 0;

void FindAndOrderPrimitivePitagorasThreesomes(primitive_pitagoras_threesome **p_p_first_primitive_pitagoras_threesome, int maxNumberOfM,
	int num_of_computation_threads,
	int outputBufferSize)
{
	output_buffer_size = outputBufferSize;
	max_number_of_m = maxNumberOfM;

	threesome_array = (primitive_pitagoras_threesome*)malloc(sizeof(primitive_pitagoras_threesome)*output_buffer_size);

#pragma region Create Empty And Full Semaphores
	empty = CreateSemaphore(
		NULL,	/* Default security attributes */
		output_buffer_size,		/* Initial Count - all slots are empty */
		output_buffer_size,		/* Maximum Count */
		NULL); /* un-named */
	if (NULL == empty)
	{
		printf("Error when creating empty semaphore: %d\n", GetLastError());
		exit(MUTEX_OR_SEMAPHORE_ERROR);
	}

	full = CreateSemaphore(
		NULL,	/* Default security attributes */
		0,		/* Initial Count - no slots are full */
		output_buffer_size,		/* Maximum Count */
		NULL); /* un-named */
	if (NULL == full)
	{
		printf("Error when creating full semaphore: %d\n", GetLastError());
		exit(MUTEX_OR_SEMAPHORE_ERROR);
	}
#pragma endregion

#pragma region CreateMutexes
	mutex_array = (valid_mutex_struct*)malloc(maxNumberOfM * sizeof(valid_mutex_struct));

	for (int i = 0; i < max_number_of_m; i++)
	{
		char *mutex_name = (char*)malloc(sizeof(char) * 13);
		strcpy(mutex_name, "n_param_\0\0\0\0\0");
		itoa(i + 1, mutex_name + 7, 10);
			HANDLE n_mutex = CreateMutex(
				NULL,	/* default security attributes */
				FALSE,	/* initially not owned */
				mutex_name);	/* named mutex */
		if (NULL == n_mutex)
		{
			printf("Error when creating mutex: %d\n", GetLastError());
			exit(MUTEX_OR_SEMAPHORE_ERROR);
		}
		mutex_array[i].n_mutex = n_mutex;
		mutex_array[i].valid_n = 1;
	}

	min_free_n_param_mutex_handle = CreateMutex(
		NULL,	/* default security attributes */
		FALSE,	/* initially not owned */
		"min_n_param");	/* named mutex */
	if (NULL == min_free_n_param_mutex_handle)
	{
		printf("Error when creating mutex: %d\n", GetLastError());
		exit(MUTEX_OR_SEMAPHORE_ERROR);
	}

	writing_threesome_mutex_handle = CreateMutex(
		NULL,
		FALSE,
		"writing_mutex");
	if (NULL == writing_threesome_mutex_handle)
	{
		printf("Error when creating mutex: %d\n", GetLastError());
		exit(MUTEX_OR_SEMAPHORE_ERROR);
	}

#pragma endregion

#pragma region Create Ordering Thread
	DWORD wait_code_array_thread;
	HANDLE *p_ordering_thread_handle;
	DWORD *ordering_thread_id;
	printf("ordering thread created\n");
	p_ordering_thread_handle = CreateThreadSimple(OrderingThread,
		&ordering_thread_id,
		NULL);
#pragma endregion

#pragma region Create computation threads
	DWORD wait_code_computation_threads;
	HANDLE *p_computation_thread_handles = (HANDLE*)malloc(sizeof(HANDLE)*num_of_computation_threads);
	DWORD *p_computation_thread_ids = (DWORD*)malloc(sizeof(DWORD)*num_of_computation_threads);
	computation_thread_params *p_computation_threads_params = (computation_thread_params*)malloc(sizeof(computation_thread_params)*num_of_computation_threads);
	for (int thread_index = 0; thread_index < num_of_computation_threads; thread_index++)
	{
		printf("computation thread %d created\n", thread_index + 1);
		p_computation_threads_params[thread_index].n = 0;
		p_computation_threads_params[thread_index].computation_thread_index = thread_index + 1;
		p_computation_thread_handles[thread_index] = CreateThreadSimple(ComputationThread,
			&p_computation_thread_ids[thread_index],
			&p_computation_threads_params[thread_index]);
	}
#pragma endregion

#pragma region Check Threads SumUp
	wait_code_computation_threads = WaitForMultipleObjects(num_of_computation_threads, p_computation_thread_handles, TRUE, INFINITE); // Wait for all computation threads.
	printf("WaitForMultipleObjects output: ");
	switch (wait_code_computation_threads)
	{
	case WAIT_TIMEOUT:
		printf("WAIT_TIMEOUT\n"); break;
	case WAIT_OBJECT_0:
		printf("WAIT_OBJECT_0\n"); break;
	case WAIT_FAILED:
		printf("WAIT_FAILED\n"); break;
	default:
		printf("0x%x\n", wait_code_computation_threads);
	}
	free(p_computation_thread_handles);
	free(p_computation_thread_ids);
	free(p_computation_threads_params);

	computation_threads_are_done = 1;

	wait_code_array_thread = WaitForSingleObject(p_ordering_thread_handle, INFINITE);
	switch (wait_code_array_thread)
	{
	case WAIT_TIMEOUT:
		printf("WAIT_TIMEOUT\n"); break;
	case WAIT_OBJECT_0:
		printf("WAIT_OBJECT_0\n"); break;
	case WAIT_FAILED:
		printf("WAIT_FAILED\n"); break;
	default:
		printf("0x%x\n", wait_code_computation_threads);
	}
#pragma endregion

	free(threesome_array);
	*p_p_first_primitive_pitagoras_threesome = p_first_threesome;
	return;
}

static DWORD WINAPI OrderingThread(LPVOID lpParam)
{
	int finishThread = 1;
	while (finishThread != 0)
	{
		finishThread = OrderingThreadRun(lpParam);
	};
}

int OrderingThreadRun()
{
	int exit_wait_for_new_threesome = 0;
#pragma region Initialize
	DWORD wait_code;
	BOOL ret_val;
#pragma endregion

	while (1)
	{
		wait_code = WaitForSingleObject(full, 10);
#pragma region WaitForThreesomeToOrderOrToEnd
		while (wait_code != WAIT_OBJECT_0)
		{
			if (computation_threads_are_done == 1 && threesome_array_next_free_slot == 0)
			{
				exit_wait_for_new_threesome = 1;
				break;
			}
			wait_code = WaitForSingleObject(full, 10);
		}
		if (exit_wait_for_new_threesome == 1)
		{
			break;
		}
		wait_code = WaitForSingleObject(writing_threesome_mutex_handle, INFINITE);
		if (wait_code != WAIT_OBJECT_0)
		{
			printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
			exit(ERROR_WAIT_OBJECT);
		}
#pragma endregion

#pragma region CreateACopyOfThreesomeToOrder
		primitive_pitagoras_threesome *p_threesome_to_order = (primitive_pitagoras_threesome*)malloc(sizeof(primitive_pitagoras_threesome));
		p_threesome_to_order->m = (threesome_array[threesome_array_next_free_slot - 1]).m;
		p_threesome_to_order->n = (threesome_array[threesome_array_next_free_slot - 1]).n;
		p_threesome_to_order->a = (threesome_array[threesome_array_next_free_slot - 1]).a;
		p_threesome_to_order->b = (threesome_array[threesome_array_next_free_slot - 1]).b;
		p_threesome_to_order->c = (threesome_array[threesome_array_next_free_slot - 1]).c;
		p_threesome_to_order->p_next_primitive_pitagoras_threesome = 0;
		printf("thread run for n= %d, m= %d, a= %d, b= %d,c = %d \n",
			p_threesome_to_order->n,
			p_threesome_to_order->m,
			p_threesome_to_order->a,
			p_threesome_to_order->b,
			p_threesome_to_order->c);
#pragma endregion

#pragma region Order
		order(&p_first_threesome, p_threesome_to_order);
#pragma endregion

		num_of_ordered_threesomes += 1;
		threesome_array_next_free_slot -= 1;

		ret_val = ReleaseMutex(writing_threesome_mutex_handle);
		if (ret_val == FALSE)
		{
			printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
			exit(1);
		}
		ret_val = ReleaseSemaphore(
			empty,
			1,
			NULL);
		if (ret_val == FALSE)
		{
			printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
			exit(1);
		}
		printf("num of threesomes waiting to be ordered is: %d , num_of_ordered_threesomes: %d\n", threesome_array_next_free_slot, num_of_ordered_threesomes);
	}
	return 0;
}

static DWORD WINAPI ComputationThread(LPVOID lpParam)
{
	if (NULL == lpParam)
		exit(ERROR_NULL);
	int finishThread = 1;
	while (finishThread != 0)
	{
		finishThread = ComputationThreadRun(lpParam);
	};
}

int ComputationThreadRun(computation_thread_params *p_computation_thread_params_var)
{
	DWORD wait_code;
	BOOL ret_val;

#pragma region GetnForThreadAndExitIfNonLeft
	while (1)
	{
		int i = 0;
		int done = 0;
		while (!done)
		{
			if (i >= max_number_of_m - 1)
			{
				done = 1;
				stop = 1;
			}
			else
			{
				if (mutex_array[i].valid_n == 1)
				{
					wait_code = WaitForSingleObject(mutex_array[i].n_mutex, INFINITE);
					if (WAIT_OBJECT_0 != wait_code)
					{
						printf("Error when waiting for mutex\n");
						return ERROR_MUTEX_WAIT;
					}
					mutex_array[i].valid_n = 0;
					p_computation_thread_params_var->n = i + 1;
					ret_val = ReleaseMutex(mutex_array[i].n_mutex);
					if (FALSE == ret_val)
					{
						printf("Error when releasing\n");
						return ERROR_MUTEX_RELEASE;
					}
					done = 1;
				}
			}
			i += 1;
		}
		if (stop)
		{
			break;
		}
		
#pragma endregion

		printf("thread: %d, thread run for n= %d \n",
			p_computation_thread_params_var->computation_thread_index,
			p_computation_thread_params_var->n);

#pragma region CreateAllPrimitiveThreesomesForN
		for (int m_index = p_computation_thread_params_var->n + 1; m_index <= max_number_of_m; m_index++)
		{
			// check if valid m,n creators
			if (GreatestCommonDivider(m_index, p_computation_thread_params_var->n) == 1 &&
				(m_index % 2 == 0 || p_computation_thread_params_var->n % 2 == 0))
			{
#pragma region CreatePrimitiveThreesomes
				primitive_pitagoras_threesome primitive_pitagoras_threesome_var = PrimitiveThreesomeCreator(m_index,
					p_computation_thread_params_var->n);
				printf("thread: %d, thread run for n= %d, m= %d, a= %d, b= %d,c = %d \n",
					p_computation_thread_params_var->computation_thread_index,
					primitive_pitagoras_threesome_var.n,
					primitive_pitagoras_threesome_var.m,
					primitive_pitagoras_threesome_var.a,
					primitive_pitagoras_threesome_var.b,
					primitive_pitagoras_threesome_var.c);
#pragma endregion

#pragma region SendToWaitingToOrderArray
				wait_code = WaitForSingleObject(empty, INFINITE);
				if (wait_code != WAIT_OBJECT_0)
				{
					printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
					exit(ERROR_WAIT_OBJECT);
				}
				wait_code = WaitForSingleObject(writing_threesome_mutex_handle, INFINITE);
				if (wait_code != WAIT_OBJECT_0)
				{
					printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
					exit(ERROR_WAIT_OBJECT);
				}

				InsertThreesomeToArray(&primitive_pitagoras_threesome_var);
				threesome_array_next_free_slot += 1;
				printf("thread %d inserted one item. number of threesomes waiting to be ordered: %d\n", p_computation_thread_params_var->computation_thread_index, threesome_array_next_free_slot);

				ret_val = ReleaseMutex(writing_threesome_mutex_handle);
				if (ret_val == FALSE)
				{
					printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
					exit(ERROR_MUTEX_RELEASE);
				}
				ret_val = ReleaseSemaphore(
					full,
					1,
					NULL);
				if (ret_val == FALSE)
				{
					printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
					exit(ERROR_MUTEX_RELEASE);
				}
#pragma endregion
			}
		}
	}
#pragma endregion
	printf("thread: %d ended\n", p_computation_thread_params_var->computation_thread_index);
	return 0;
}

int InsertThreesomeToArray(primitive_pitagoras_threesome *p_primitive_pitagoras_threesome_var)
{
	if (NULL == p_primitive_pitagoras_threesome_var)
	{
		exit(ERROR_NULL);
	}
	if (NULL == threesome_array)
	{
		exit(ERROR_NULL);
	}

	threesome_array[threesome_array_next_free_slot].a = p_primitive_pitagoras_threesome_var->a;
	threesome_array[threesome_array_next_free_slot].b = p_primitive_pitagoras_threesome_var->b;
	threesome_array[threesome_array_next_free_slot].c = p_primitive_pitagoras_threesome_var->c;
	threesome_array[threesome_array_next_free_slot].m = p_primitive_pitagoras_threesome_var->m;
	threesome_array[threesome_array_next_free_slot].n = p_primitive_pitagoras_threesome_var->n;
}

static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id,
	LPVOID p_thread_parameters)
{
	HANDLE thread_handle;

	if (NULL == p_start_routine)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(ERROR_NULL);
	}

	if (NULL == p_thread_id)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(ERROR_NULL);
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		p_start_routine,     /*  thread function */
		p_thread_parameters, /*  argument to thread function */
		0,                   /*  use default creation flags */
		p_thread_id);        /*  returns the thread identifier */

	if (NULL == thread_handle)
	{
		printf("Couldn't create thread\n");
		exit(ERROR_THREAD);
	}

	return thread_handle;
}

void order(primitive_pitagoras_threesome **p_p_first_threesome, primitive_pitagoras_threesome *p_primitive_pitagoras_threesome_var)
{
	if (*p_p_first_threesome == 0)
	{
		*p_p_first_threesome = p_primitive_pitagoras_threesome_var;
		return;
	}
	if (p_primitive_pitagoras_threesome_var->n < (*p_p_first_threesome)->n ||
		(p_primitive_pitagoras_threesome_var->n == (*p_p_first_threesome)->n && p_primitive_pitagoras_threesome_var->m < (*p_p_first_threesome)->m))
	{
		p_primitive_pitagoras_threesome_var->p_next_primitive_pitagoras_threesome = *p_p_first_threesome;
		*p_p_first_threesome = p_primitive_pitagoras_threesome_var;
	}

	primitive_pitagoras_threesome *p_first_threesome = *p_p_first_threesome;
	primitive_pitagoras_threesome *p_threesome_index = p_first_threesome;
	primitive_pitagoras_threesome *p_threesome_index_next = p_threesome_index->p_next_primitive_pitagoras_threesome;
	while (p_threesome_index != 0)
	{
		if (p_threesome_index_next == 0)
		{
			p_threesome_index->p_next_primitive_pitagoras_threesome = p_primitive_pitagoras_threesome_var;
			break;
		}
		if (p_primitive_pitagoras_threesome_var->n < p_threesome_index_next->n ||
			(p_primitive_pitagoras_threesome_var->n == p_threesome_index_next->n && p_primitive_pitagoras_threesome_var->m < p_threesome_index_next->m))
		{
			p_threesome_index->p_next_primitive_pitagoras_threesome = p_primitive_pitagoras_threesome_var;
			p_primitive_pitagoras_threesome_var->p_next_primitive_pitagoras_threesome = p_threesome_index_next;
			break;
		}


		p_threesome_index = p_threesome_index->p_next_primitive_pitagoras_threesome;
		p_threesome_index_next = p_threesome_index->p_next_primitive_pitagoras_threesome;
	}
}