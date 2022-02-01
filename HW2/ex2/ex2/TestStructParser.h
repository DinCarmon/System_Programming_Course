#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"


typedef struct test_args_s test_args;

#ifndef test_args_s
struct test_args_s
{
	int test_num;
	int thread_timeout_flag;
	char *p_test_command;
	char *p_expected_result_path;
	char *p_output_result;
	test_args *p_next_test_args;
};
#endif

/*
Description:
	The function receives a test_args variable and updates its parameters according to a received parameters
Parameters:
	test_args *p_test_args_var (output)
	int exit_code_test_process (input. what was the exitcode of the testcmd)
	int succeed (it testcmd did not crush/ timeout. Is the output as expected?)
Returns:
	int (function succeded/failed)
*/
int UpdateOutputInStruct(test_args *p_test_args_var, int exit_code_test_process, int succeed);