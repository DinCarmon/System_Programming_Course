#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <stdbool.h>
#include "ThreadAndMutexGeneral.h"
#include "config.h"

HANDLE BasicMutexCreate()
{
	HANDLE * p_mutex_var = (HANDLE*)malloc(sizeof(HANDLE));
	*p_mutex_var = CreateMutex(
		NULL,	/* default security attributes */
		FALSE,	/* initially not owned */
		NULL);	/* unnamed mutex */
	if (NULL == *p_mutex_var)
	{
		printf("Error when creating mutex: %d\n", GetLastError());
		exit(MUTEX_OR_SEMAPHORE_ERROR);
	}
	return *p_mutex_var;
}

void BasicMutexWait(HANDLE *mutex_var)
{
	int wait_code = WaitForSingleObject(*mutex_var, INFINITE);
	if (wait_code != WAIT_OBJECT_0)
	{
		printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
		exit(ERROR_WAIT_OBJECT);
	}
	return;
}


void BasicMutexRelease(HANDLE mutex_var)
{
	bool ret_val = ReleaseMutex(mutex_var);
	if (ret_val == FALSE)
	{
		printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
		exit(1);
	}
	return;
}



