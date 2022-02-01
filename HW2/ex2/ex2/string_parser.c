#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include "config.h"
#include "string_parser.h"

char *AddEscapeCharacter(char* str)
{
	if (NULL == str)
		return ERROR_NULL;
	int new_size = strlen(str);
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == '"' || str[i] == '\\')
		{
			new_size++;
		}
	}
	char *fixed_str = (char*)malloc(new_size);
	int index_old = 0;
	int index_new = 0;
	while (index_new < strlen(fixed_str))
	{
		fixed_str[index_new] = str[index_old];
		if ((str[index_old] == '"' || str[index_old] == '\\'))
		{
			index_new++;
			fixed_str[index_new] = str[index_old];
		}
		index_new++;
		index_old++;
	}
	return fixed_str;
}