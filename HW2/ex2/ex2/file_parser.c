#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include "file_parser.h"
#include "string_parser.h"
#pragma warning(disable : 4996)

test_args *ExtractTests(char *p_tests_file_path)
{
	if (NULL == p_tests_file_path)
		return ERROR_NULL;
	FILE *p_tests_file=0;
	errno_t err;
	if (err = fopen_s(&p_tests_file, p_tests_file_path, "r"))
		return ERROR_OPENING_FILE;
	char test_row[MAX_LENGTH_OF_TEST_LINE];
	test_args *p_first_test_args = 0;
	test_args *p_last_test_args = 0;
	while (fgets(test_row, MAX_LENGTH_OF_TEST_LINE, p_tests_file) != NULL)
	{
		if (NULL == p_first_test_args) // First entry to while
		{
			static test_args test_args_var;
			test_args_var = GetTestArgs(test_row);
			p_first_test_args = &test_args_var;
			p_last_test_args = &test_args_var;
		}
		else
		{
			static test_args test_args_var;
			test_args_var = GetTestArgs(test_row);
			(*p_last_test_args).p_next_test_args = &test_args_var;
			p_last_test_args = (*p_last_test_args).p_next_test_args;
		}
	}
	return p_first_test_args;
}

test_args GetTestArgs(char *p_test_row)
{
	if (NULL == p_test_row)
		exit(ERROR_NULL);
	static test_args test_args_var;
	test_args_var.p_next_test_args = 0;
	static test_args *p_test_args_var = &test_args_var;

	#pragma region delete line jump if exists
	if (p_test_row[strlen(p_test_row) - 1] == '\n')
	{
		p_test_row[strlen(p_test_row) - 1] = NULL;
	}
	#pragma endregion

	#pragma region parse row
	char *token = strtok(p_test_row, TEST_ARGS_SEPARATOR);
	char *test = (char*)malloc(1);
	test[0] = 0;
	char *next_token;
	while (token != NULL)
	{
		next_token = strtok(NULL, TEST_ARGS_SEPARATOR);
		if (NULL == next_token) // Last arg.
		{
			test[strlen(test) - 1] = NULL; // Delete last space
			p_test_args_var->p_expected_result_path = (char*)malloc(strlen(token));
			strcpy(p_test_args_var->p_expected_result_path, token);
			break;
		}
		else
		{
			int before_realloc = strlen(test);
			test = (char*)realloc(test, strlen(test) + 1 + strlen(token));
			test[before_realloc] = 0;
			strcat(test, token);
			strcat(test, " ");
		}
		token = next_token;
	}
	p_test_args_var->p_test_command = test;
	#pragma endregion

	return *p_test_args_var;
}

int WriteOutputFile(test_args *p_test_args_var, char *p_result_file_path)
{
	if (NULL == p_result_file_path || NULL == p_test_args_var)
		return ERROR_NULL;
	FILE *p_output_file = 0;
	errno_t err;
	if (err = fopen_s(&p_output_file, p_result_file_path, "w"))
		return ERROR_OPENING_FILE;

	while (p_test_args_var != NULL)
	{
		char output_row[MAX_LENGTH_OF_OUTPUT_LINE];
		sprintf(output_row, "test #%d : %s\n", (*p_test_args_var).test_num, (*p_test_args_var).p_output_result);
		fputs(output_row, p_output_file);
		p_test_args_var = p_test_args_var->p_next_test_args;
	}

	fclose(p_output_file);

	return 0;
}

int SameFile(char *path1, char *path2)
{
	if (NULL == path1 || NULL == path2)
		return ERROR_NULL;
	FILE *p_file1 = 0;
	FILE *p_file2 = 0;
	errno_t err1;
	errno_t err2;
	if (err1 = fopen_s(&p_file1, path1, "r"))
		return ERROR_OPENING_FILE;
	if (err2 = fopen_s(&p_file2, path2, "r"))
		return ERROR_OPENING_FILE;
	char test_row1[MAX_LENGTH_OF_TEST_LINE];
	char test_row2[MAX_LENGTH_OF_TEST_LINE];
	while (1)
	{
		BOOL end1 = fgets(test_row1, MAX_LENGTH_OF_TEST_LINE, p_file1) == NULL;
		BOOL end2 = fgets(test_row2, MAX_LENGTH_OF_TEST_LINE, p_file2) == NULL;
		if (end1 == true && end2 == true)
		{
			return 0;
		}
		else if (end1 == true || end2 == true)
		{
			return -1;
		}
		else
		{
			if (strcmp(test_row1, test_row2) != 0)
			{
				return -1;
			}
		}
	}
	if (fgets(test_row1, MAX_LENGTH_OF_TEST_LINE, p_file2) != NULL)
	{
		return -1;
	}
	return 0;
}