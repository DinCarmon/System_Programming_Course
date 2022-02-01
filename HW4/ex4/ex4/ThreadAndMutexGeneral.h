#pragma once

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <stdbool.h>
#include "config.h"

/*
Description:
Mutex creator 
Parameters:
None
Returns:
Mutex handle
*/
HANDLE BasicMutexCreate();

/*
Description:
Mutex wait function
Parameters:
HANDLE *mutex_var
Returns:
None
*/
void BasicMutexWait(HANDLE *mutex_var);

/*
Description:
Mutex release function
Parameters:
HANDLE *mutex_var
Returns:
None
*/
void BasicMutexRelease(HANDLE mutex_var);