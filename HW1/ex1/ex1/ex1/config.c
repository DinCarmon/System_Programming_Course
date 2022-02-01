#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

bool StringInArray(char **p_array_of_strings, char str[])
{
	if (NULL == p_array_of_strings || NULL == str)
		return false;
	for (char *str_in_array = *p_array_of_strings; str_in_array; str_in_array = *++p_array_of_strings)
	{
		if (strcmp(str_in_array, str) == 0)
		{
			return true;
		}
	}
	return false;
}