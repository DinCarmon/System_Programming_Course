#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "file_parser.h"
#include "string_parser.h"
#include "tester.h"
#pragma warning(disable : 4996)

int TestsRunner(test_args *p_first_test)
{
	if (NULL == p_first_test)
		return ERROR_NULL;
	#pragma region Open a thread for each test
	test_args *p_test_args_var = p_first_test;
	HANDLE *p_thread_handles = (HANDLE*)malloc(sizeof(HANDLE));
	DWORD *p_thread_ids = (DWORD*)malloc(sizeof(DWORD));
	DWORD wait_code_tests_threads;
	BOOL ret_val;
	DWORD thread_count = 0;
	while (p_test_args_var != NULL)
	{
		(*p_test_args_var).test_num = thread_count + 1;

		printf("test %d created\n", thread_count+1);
		if (thread_count != 0) // Not first run in loop.
		{
			p_thread_handles = realloc(p_thread_handles, (thread_count + 1)*sizeof(HANDLE));
			p_thread_ids = realloc(p_thread_ids, (thread_count + 1)*sizeof(DWORD));
		}

		p_thread_handles[thread_count] = CreateThreadSimple(TestThread, &p_thread_ids[thread_count], p_test_args_var);	
		
		thread_count++;
		p_test_args_var = p_test_args_var->p_next_test_args;
	}
	#pragma endregion

	#pragma region Check Threads SumUp
	wait_code_tests_threads = WaitForMultipleObjects(thread_count, p_thread_handles, TRUE, INFINITE); // Wait for all threads (tests).
	printf("WaitForMultipleObjects output: ");
	switch (wait_code_tests_threads)
	{
		case WAIT_TIMEOUT:
			printf("WAIT_TIMEOUT\n"); break;
		case WAIT_OBJECT_0:
			printf("WAIT_OBJECT_0\n"); break;
		case WAIT_FAILED:
			printf("WAIT_FAILED\n"); break;
		default:
			printf("0x%x\n", wait_code_tests_threads);
	}
	#pragma endregion

	#pragma region Free Memory, and System
	for (int i = 0; i < thread_count; i++)
	{
		ret_val = CloseHandle(p_thread_handles[i]);
		if (false == ret_val)
		{
			printf("Error when closing\n");
			return ERROR_CLOSING_HANDLE;
		}
	}
	free(p_thread_handles);
	free(p_thread_ids);
	#pragma endregion
	return 0;
}

static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE *p_start_routine, DWORD *p_thread_id, test_args *p_test_args_var)
{
	HANDLE thread_handle;

	if (NULL == p_start_routine)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(ERROR_THREAD);
	}

	if (NULL == p_thread_id || NULL == p_test_args_var)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(ERROR_THREAD);
	}

	thread_handle = CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		p_start_routine, /*  thread function */
		p_test_args_var,            /*  argument to thread function */
		0,               /*  use default creation flags */
		p_thread_id);    /*  returns the thread identifier */

	if (NULL == thread_handle)
	{
		printf("Couldn't create thread\n");
		exit(ERROR_THREAD);
	}

	return thread_handle;
}

static DWORD WINAPI TestThread(LPVOID lpParam)
{
	if (NULL == lpParam)
		exit(ERROR_NULL);
	int finishThread = 1;
	while (finishThread!=0)
	{
		finishThread = TestRun(lpParam);
	};
}

static int TestRun(test_args *p_test_args_var)
{
	if (NULL == p_test_args_var)
		return ERROR_NULL;

	printf("thread is running\n");
	printf(p_test_args_var->p_test_command);
	printf("\n");

	#pragma region CreateTestCmdProcess
	PROCESS_INFORMATION lpProcessInfo;

	char *command_line_before_TCHAR = AddEscapeCharacter(p_test_args_var->p_test_command);
	printf(command_line_before_TCHAR);
	printf("\n");

	bool retval_of_test = CreateProcessSimple(_T(command_line_before_TCHAR), &lpProcessInfo);
	if (retval_of_test == 0)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return 0;
	}
	#pragma endregion

	#pragma region Wait and Exit Process
	DWORD wait_code_test_process = WaitForSingleObject(lpProcessInfo.hProcess, TIMEOUT_TIME);
	printf("WaitForSingleObject output: ");
	(*p_test_args_var).thread_timeout_flag = 0;
	switch (wait_code_test_process)
	{
		case WAIT_TIMEOUT:
			printf("WAIT_TIMEOUT\n");
			(*p_test_args_var).thread_timeout_flag = 1; break;
		case WAIT_OBJECT_0:
			printf("WAIT_OBJECT_0\n"); break;
		case WAIT_FAILED:
			printf("WAIT_FAILED\n"); break;
		default:
			printf("0x%x\n", wait_code_test_process);
	}
	DWORD exit_code_test_process;
	BOOL exit_code_test_process_flag = GetExitCodeProcess(lpProcessInfo.hProcess, &exit_code_test_process);
	printf("The exit code for the process is 0x%x\n", exit_code_test_process);
	#pragma endregion

	#pragma region Free Memory, and System
	BOOL close_test_process_handle = CloseHandle(lpProcessInfo.hProcess);
	#pragma endregion

	#pragma region CheckOutput
	int succeed = -1;

	if (exit_code_test_process == 0 ) // Test cmd ended successfully in time
	{
		#pragma region ParsePathOfOutputFromTestPath
		char *test_output_path = strtok(p_test_args_var->p_test_command, TEST_ARGS_SEPARATOR);
		strcpy(test_output_path + strlen(test_output_path) - 4, ".txt");
		#pragma endregion
		
		succeed = SameFile(test_output_path, p_test_args_var->p_expected_result_path);
		//succeed is equal to 0 if the result is true and -1 otherwise
	}
	else
	{
		succeed = exit_code_test_process;
	}
	#pragma endregion
	
	UpdateOutputInStruct(p_test_args_var, exit_code_test_process, succeed);

	return 0;
}

BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr)
{
	STARTUPINFO	startinfo = { sizeof(STARTUPINFO), NULL, 0 }; /* <ISP> here we */
															  /* initialize a "Neutral" STARTUPINFO variable. Supplying this to */
															  /* CreateProcess() means we have no special interest in this parameter. */
															  /* This is equivalent to what we are doing by supplying NULL to most other */
															  /* parameters of CreateProcess(). */

	return CreateProcess(NULL, /*  No module name (use command line). */
		CommandLine,			/*  Command line. */
		NULL,					/*  Process handle not inheritable. */
		NULL,					/*  Thread handle not inheritable. */
		FALSE,					/*  Set handle inheritance to FALSE. */
		NORMAL_PRIORITY_CLASS,	/*  creation/priority flags. */
		NULL,					/*  Use parent's environment block. */
		NULL,					/*  Use parent's starting directory. */
		&startinfo,				/*  Pointer to STARTUPINFO structure. */
		ProcessInfoPtr			/*  Pointer to PROCESS_INFORMATION structure. */
	);
}

