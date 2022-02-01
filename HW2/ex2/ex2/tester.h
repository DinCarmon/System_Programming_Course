#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include "config.h"
#include "file_parser.h"
#include "TestStructParser.h"

/*
Description:
	The function receives a nested list of test and its various arguments.
	The function updates the list. It opens each test in a process opened by another thread.
Parameters:
	test_args *p_first_test (output. The list updates its thread_timeout_flag, test_num, p_output_result)
Returns:
	int (Succeed / Failed)
*/
int TestsRunner(test_args *p_first_test);

/*
Description:
	The function receives an adress of a function to run (TestThread is expected),
	an id to update in the id of the thread, and argument for the function.
	The function opens the thread.
Parameters:
	LPTHREAD_START_ROUTINE *p_start_routine (input)
	DWORD *p_thread_id (output)
	test_args *p_test_args_var (input and output)
Returns:
	a handle to the thread
*/
static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE *p_start_routine, DWORD *p_thread_id, test_args *p_test_args_var);

/*
Description:
	The function receives an argument (expects a pointer to test_args) and runs TestRun.
Parameters:
	argument (expects a pointer to test_args. input and output)
Returns:
	a handle to the thread
*/
static DWORD WINAPI TestThread(LPVOID lpParam);

/*
Description:
	The function receives a test_args, opens a proceess which runs the test,
	and compares the output to the expected one. It updates test_args p_output_result, and thread_timeout_flag.
Parameters:
	test_args *p_test_args_var (input and output)
Returns:
	a handle to the thread
*/
static int TestRun(test_args *p_test_args_var);

/*
Description:
	The function receives a command line to run as a process, and a process info
Parameters:
	LPTSTR CommandLine (input)
	PROCESS_INFORMATION *ProcessInfoPtr (output)
Returns:
	BOOL (function succeded/failed)
*/
BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr);