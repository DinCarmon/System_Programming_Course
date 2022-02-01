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
#include "file_parser.h"

#pragma warning(disable : 4996)

char* MessageBuilder1Param(const char* prefix, char* param1)
{
	char* param1_copy = (char*)malloc(SEND_STR_SIZE);
	char* raw_message = (char*)malloc(SEND_STR_SIZE);
	*raw_message = 0;
	*param1_copy = 0;
	strncat(raw_message, prefix, strlen(prefix));
	strncat(param1_copy, param1, strlen(param1));
	char * delimeter1 = ";";
	char * delimeter2 = " ";
	char * pch;
	pch = strtok(param1_copy, delimeter2);
	while (pch != NULL)
	{
		strncat(raw_message, pch, strlen(pch)); // param
		strncat(raw_message, delimeter1, strlen(delimeter1)); // ;
		strncat(raw_message, delimeter2, strlen(delimeter2)); // space
		strncat(raw_message, delimeter1, strlen(delimeter1)); // ;
		pch = strtok(NULL, delimeter2);
	}
	raw_message[strlen(raw_message)-3] = 0; // without the final delimeters
	return raw_message;
}

char* MessageBuilder3Param(char* prefix, char* param1, char* param2, char* param3)
{
	char * delimeter1 = ";";
	char* raw_message = MessageBuilder1Param(prefix, param1);
	strncat(raw_message, delimeter1, strlen(delimeter1));
	raw_message = MessageBuilder1Param(raw_message, param2);
	strncat(raw_message, delimeter1, strlen(delimeter1));
	raw_message = MessageBuilder1Param(raw_message, param3);
	return raw_message;
}

int MessageDeparse(char** p_prefix, char** p_message_params, char* message)
{
	if (strchr(message, ':') == NULL)
	{
		*p_prefix = message;
		return NO_PARAMS_MESSAGE;
	}

	*p_prefix = strtok(message, ":");
	*p_message_params = message + strlen(*p_prefix) + 1;//strtok(NULL, ":");

	RemoveChar(*p_message_params, ';');

	return 0;
}

void RemoveChar(char str[], char rm)
{
	size_t  i = 0;
	while (i < strlen(str))
	{
		if (str[i] == rm)
		{
			for (size_t  j = i; j < strlen(str); j++)
			{
				str[j] = str[j + 1];
			}
		}
		else i++;
	}
}