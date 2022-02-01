#pragma once
#define PROGRAM_NUM_OF_PARAMETERS 2
#define MAX_LENGTH_OF_TEST_LINE 100
#define MAX_LENGTH_OF_OUTPUT_LINE 100
#define TIMEOUT_TIME 10000
#define ERROR_CODE_NOT_ENOUGH_ARGUMENTS -1
#define ERROR_CODE_TOO_MANY_ARGUMENTS -2
#define ERROR_NULL -3
#define ERROR_OPENING_FILE -4
#define ERROR_THREAD -5
#define TEST_ARGS_SEPARATOR " "
#define ERROR_CLOSING_HANDLE -6

/*
Description:
	The function receives the number of inputs the projects receives and checks whether
	it as expected.
Parameters:
	int args (input)
Returns:
	Error codes of too many/too few arguments or 0 for valid num of arguments.
*/
int ValidNumOfProgramParameters(int argc);