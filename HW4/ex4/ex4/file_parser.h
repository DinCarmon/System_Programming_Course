#pragma once
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include "socket_config.h"
#include "socket_send_recv.h"
#include "server.h"
#include "ThreadAndMutexGeneral.h"
#include "config.h"
#include "GameRun.h"

/*
Description:
Initialize log file
Parameters:
char* log_file_path
Returns:
0 if no errors occured, error code otherwise
*/
int LogFileInitializer(char* log_file_path);

/*
Description:
update log file
Parameters:
char* message
Returns:
0 if no errors occured, error code otherwise
*/
int LogNotify(char* message);

/*
Description:
closes log file
Parameters:
None
Returns:
0 if no errors occured, error code otherwise
*/
int CloseLog();