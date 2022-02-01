#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"

typedef struct computation_thread_params_s computation_thread_params;
typedef struct valid_mutex_struct_s valid_mutex_struct;

#ifndef computation_thread_params_s
struct computation_thread_params_s
{
	int n;
	int computation_thread_index;
};
#endif

#ifndef valid_mutex_struct_s
struct valid_mutex_struct_s
{
	HANDLE n_mutex;
	int valid_n;
};
#endif

/*
Description:
The function is the main function that finds the primitive pitagoras threesomes and orders them.
Parameters:
primitive_pitagoras_threesome **p_p_first_primitive_pitagoras_threesome
int max_number_of_m
int num_of_computation_threads
int output_buffer_size
*/
void FindAndOrderPrimitivePitagorasThreesomes(primitive_pitagoras_threesome **p_p_first_primitive_pitagoras_threesome, int max_number_of_m, int num_of_computation_threads,	int output_buffer_size);

/*
Description:
The function is the ordering thread function, it calls OrderingThreadRun for the operation itself.
Parameters:
LPVOID lpParam
*/
static DWORD WINAPI OrderingThread(LPVOID lpParam);

/*
Description:
The function orders the threesomes while reaing them from the buffer.
Parameters:
LPVOID lpParam
Returns:
Error codes if program failed and 0 if program succeeded
*/
int OrderingThreadRun();

/*
Description:
The function is the computation threads function, it calls ComputationThreadRun for the operation itself.
Parameters:
LPVOID lpParam
*/
static DWORD WINAPI ComputationThread(LPVOID lpParam);

/*
Description:
The function inserts threesomes to buffer when calling InsertThreesomeToArray.
Parameters:
LPVOID lpParam
Returns:
Error codes if program failed and 0 if program succeeded
*/
int ComputationThreadRun(computation_thread_params *p_computation_thread_params_var);

/*
Description:
The function inserts threesomes to buffer.
Parameters:
LPVOID lpParam
Returns:
Error codes if program failed and 0 if program succeeded
*/
int InsertThreesomeToArray(primitive_pitagoras_threesome *p_primitive_pitagoras_threesome_var);

/*
Description:
The function creates a new thread.
Parameters:
LPTHREAD_START_ROUTINE p_start_routine
*/
static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id,
	LPVOID p_thread_parameters);

/*
Description:
The function inserts the new primitive threesome to the right location (ordering).
Parameters:
primitive_pitagoras_threesome **p_p_first_threesome
primitive_pitagoras_threesome *p_primitive_pitagoras_threesome_var
*/
void order(primitive_pitagoras_threesome **p_p_first_threesome, primitive_pitagoras_threesome *p_primitive_pitagoras_threesome_var);