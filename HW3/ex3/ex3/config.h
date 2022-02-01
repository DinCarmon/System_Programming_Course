#pragma once
#define PROGRAM_NUM_OF_PARAMETERS 4
#define ERROR_CODE_NOT_ENOUGH_ARGUMENTS -1
#define ERROR_CODE_TOO_MANY_ARGUMENTS -2
#define ERROR_NULL -3
#define ERROR_OPENING_FILE -4
#define ERROR_THREAD -5
#define ERROR_CLOSING_HANDLE -6
#define ERROR_CREATING_MUTEX_HANDLE -7
#define ERROR_MUTEX_WAIT -8
#define ERROR_MUTEX_RELEASE -9
#define ERROR_DYNAMIC_MEMORY -10
#define MUTEX_OR_SEMAPHORE_ERROR -11
#define ERROR_WAIT_OBJECT -12
#define MAX_RUN_THREADS 100
#define MAX_OUTPUT_BUFFER_SIZE 100
#define MAX_NUMBER_UPPER_BOUND 1000

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