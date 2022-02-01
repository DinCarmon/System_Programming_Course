#include "TestStructParser.h"
#pragma warning(disable : 4996)

int UpdateOutputInStruct(test_args *p_test_args_var, int exit_code_test_process, int succeed)
{
	if (NULL == p_test_args_var)
		return ERROR_NULL;

	char *succeeded_output = "Succeeded";
	char *timeout_output = "Timed Out";
	char *crash_output = "Crashed";
	char *fail_output = "Failed";

	if ((*p_test_args_var).thread_timeout_flag == 1) //timeout
	{
		p_test_args_var->p_output_result = (char*)malloc(strlen(timeout_output));
		(*p_test_args_var).p_output_result = timeout_output;
	}
	else
	{
		if (exit_code_test_process == 0) //succeeded or failed
		{
			if (succeed == 0) //succeeded
			{
				p_test_args_var->p_output_result = (char*)malloc(strlen(succeeded_output));
				(*p_test_args_var).p_output_result = succeeded_output;
			}
			else //failed
			{
				p_test_args_var->p_output_result = (char*)malloc(strlen(fail_output));
				(*p_test_args_var).p_output_result = fail_output;
			}
		}
		else //crashed
		{
			char exit_code_test_process_str[MAX_LENGTH_OF_OUTPUT_LINE];
			sprintf(exit_code_test_process_str, "%d", exit_code_test_process);
			p_test_args_var->p_output_result = (char*)malloc(strlen(timeout_output) + strlen(exit_code_test_process_str) + 1);
			strcpy(p_test_args_var->p_output_result, crash_output);
			strcat(p_test_args_var->p_output_result, " ");
			strcat(p_test_args_var->p_output_result, exit_code_test_process_str);
		}
	}

	return 0;
}
