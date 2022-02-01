/*
Authors:
Din Carmon, 000000000
Yonatan Rimon, 000000000
*/

/*
Project:
ex4
*/

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
	#pragma region CheckForValidArguments
	ValidParameters(argc, argv);
	#pragma endregion

	#pragma region RunAccordingToMode
	if (strcmp(argv[1], "server") == 0)
	{
		ServerRun(argv);
	}
	if (strcmp(argv[1], "client") == 0)
	{
		ClientRun(argv);
	}
	#pragma endregion

	printf(PROGRAM_FINISHED_SUCCESFULLY_MESSAGE);
	return 0;
}