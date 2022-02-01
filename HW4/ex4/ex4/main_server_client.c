/*
Description:

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"
#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
	int valid_num_of_args = ValidParameters(argc, argv);
	if (0 != valid_num_of_args)
		return valid_num_of_args;

	if (!strcmp(argv[1], "server"))
	{
		ServerRun(argv);
	}
	else
	{
		ClientRun(argv);
	}

	return 0;
}