#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

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
#include "string_parser.h"

/*
Description:
Message builder with 1 parameter 
Parameters:
const char* prefix, char* param1
Returns:
Message
*/
char* MessageBuilder1Param(const char* prefix, char* param1);

/*
Description:
Message builder with 3 parameters
Parameters:
char* prefix, char* param1, char* param2, char* param3
Returns:
Message
*/
char* MessageBuilder3Param(char* prefix, char* param1, char* param2, char* param3);

/*
Description:
Message deparser
Parameters:
char** p_prefix, char** p_message_params, char* message
Returns:
0 if no errors, error code otherwise
*/
int MessageDeparse(char** p_prefix, char** p_message_params, char* message);

/*
Description:
Removes character from string
Parameters:
char str[], char rm
Returns:
None
*/
void RemoveChar(char str[], char rm);