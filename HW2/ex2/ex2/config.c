#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"
#include "tester.h"

int ValidNumOfProgramParameters(int argc)
{
	if (argc < PROGRAM_NUM_OF_PARAMETERS + 1/*the name of the program*/)
	{
		return ERROR_CODE_NOT_ENOUGH_ARGUMENTS;
	}
	if (argc > PROGRAM_NUM_OF_PARAMETERS + 1/*the name of the program*/)
	{
		return ERROR_CODE_TOO_MANY_ARGUMENTS;
	}
	return 0;
}