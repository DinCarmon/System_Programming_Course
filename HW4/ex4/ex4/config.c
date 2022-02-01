#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"

void ValidParameters(int argc, char *argv[])
{
	int valid_mode = 0;
	if (argc < MIN_NUM_OF_PARAMETERS + 1/*the name of the program*/ ||
		argc > MAX_NUM_OF_PARAMETERS + 1/*the name of the program*/)
	{
		printf(INVALID_NUM_OF_PARAMETER_ERROR_MESSAGE);
		exit(ERROR_CODE_WRONG_NUM_OF_PARAM);
	}
	if (strcmp(argv[1], "server") == 0)
	{
		if (argc != SERVER_NUM_OF_PARAMETERS + 1/*the name of the program*/)
		{
			printf(INVALID_NUM_OF_PARAMETER_ERROR_MESSAGE);
			exit(ERROR_CODE_WRONG_NUM_OF_PARAM);
		}
		valid_mode = 1;
	}
	if (strcmp(argv[1], "client") == 0)
	{
		if (argc != CLIENT_FILE_MODE_NUM_OF_PARAMETERS + 1/*the name of the program*/ &&
			argc != CLIENT_USER_MODE_NUM_OF_PARAMETERS + 1/*the name of the program*/)
		{
			printf(INVALID_NUM_OF_PARAMETER_ERROR_MESSAGE);
			exit(ERROR_CODE_WRONG_NUM_OF_PARAM);
		}
		if (strcmp(argv[4], "file") == 0)
		{
			if (argc != CLIENT_FILE_MODE_NUM_OF_PARAMETERS + 1/*the name of the program*/)
			{
				printf(INVALID_NUM_OF_PARAMETER_ERROR_MESSAGE);
				exit(ERROR_CODE_WRONG_NUM_OF_PARAM);
			}
			valid_mode = 1;
		}
		if (strcmp(argv[4], "human") == 0)
		{
			if (argc != CLIENT_USER_MODE_NUM_OF_PARAMETERS + 1/*the name of the program*/)
			{
				printf(INVALID_NUM_OF_PARAMETER_ERROR_MESSAGE);
				exit(ERROR_CODE_WRONG_NUM_OF_PARAM);
			}
			valid_mode = 1;
		}
	}
	if (0 == valid_mode)
		exit(ERROR_CODE_INVALID_MODE);
	return;
}