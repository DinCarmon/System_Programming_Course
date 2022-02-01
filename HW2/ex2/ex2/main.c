/*
Authors:
Din Carmon, 000000000
Yonatan Rimon, 000000000
*/

/*
Project:
ex2
*/

/*
Description:
A system of programming check and validation.
The program runs many tests with different arguments and compares the outputs to the expected ones.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"
#include "tester.h"
#include "TestStructParser.h"
#include "file_parser.h"

int main(int argc, char *argv[])
{
	#pragma region CheckForValidNumOfArguments
	int valid_num_of_arguments = ValidNumOfProgramParameters(argc);
	if (valid_num_of_arguments != 0)
		return valid_num_of_arguments;
	#pragma endregion

	#pragma region ExtractTests
	char *p_tests_file_path = argv[1];
	test_args *p_first_test = (test_args *)ExtractTests(p_tests_file_path);
	#pragma endregion

	#pragma region ExecuteTestsAndHandleOutput 
	TestsRunner(p_first_test);
	char *p_result_file_path = argv[2];
	WriteOutputFile(p_first_test, p_result_file_path);
	#pragma endregion

	return 0;
}