#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include "config.h"
#include "TestStructParser.h"

/*
Description:
	The function receives a path to a test file which is written according to ex2 template.
	The function returns a nested list of test_args
	(not all values are configured after the function. only p_test_command, and p_expected_result_path)
Parameters:
	char *p_tests_file_path (input)
Returns:
	test_args * which points to the first test_args in the nested list.
*/
test_args *ExtractTests(char *p_tests_file_path);

/*
Description:
	The function receives a string which is written according to ex2 template of a test_row.
	The function test_args
	(not all values are configured after the function. only p_test_command, and p_expected_result_path)
Parameters:
	char *p_test_row (input)
Returns:
	test_args.
*/
test_args GetTestArgs(char *p_test_row);

/*
Description:
	The function receieves a path to write to the output of the tests (according to ex2 template),
	and a nested list of the arguments on each test (parsed fully).
Parameters:
	test_args *p_test_args_var (input)
	char *p_result_file_path (output)
Returns:
	int (Succeed / Failed)
*/
int WriteOutputFile(test_args *p_test_args_var, char *p_result_file_path);

/*
Description:
	The function receieves paths to two files and checks whether they are identical.
Parameters:
	char *path1 (input)
	char *path2 (input)
Returns:
	int (Succeed = 0 / Failed)
*/
int SameFile(char *path1, char *path2);