#include <stdbool.h>
#include <stdio.h>

#define MAX_LENGTH_OF_DESTINATION_ROW_IN_FILE 100
#define MAX_LENGTH_OF_PILOT_ROW_IN_FILE 100
#define FILE_PILOT_SEPARATOR ","
#define NUM_OF_AIRPLANE_TYPES 3
#define AIRPLANE_MODEL_LENGTH 3
#define LENGTH_OF_PILOT_INFO_ARRAY 4
#define ERROR_NO_AIRPLANE_TYPE_TO_DESTINATION -1
#define ERROR_CREATE_AIRPLANE_LIST -2
#define ERROR_NULL -3
#define ERROR_NO_AIRPLANE_WITH_SELECTED_MODEL_TYPE -4
#define ERROR_DELETED_AIRPLANE_ISNT_IN_AIRPLANE_LIST -5
#define ERROR_CODE_NOT_ENOUGH_ARGUMENTS -6
#define ERROR_CODE_TOO_MANY_ARGUMENTS -7
#define PROGRAM_NUM_OF_PARAMETERS 3
#define ALLOWED_PILOT_RANKS "Captain", "First Officer"
// array_length formola
#define len(arr)  (sizeof(arr) / sizeof((arr)[0]))

/*
Description:
	The function checks whether str is in the array of strings.
Parameters:
	char** p_array_of_strings (input)
		* each array ends with null, and each sub_array ends with null
	char str[] (input)
Returns:
	True/ False
*/
extern bool StringInArray(char **p_array_of_strings, char str[]);