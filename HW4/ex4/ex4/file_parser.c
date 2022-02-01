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
#include "file_parser.h"

static FILE *p_log_file = 0;

int LogFileInitializer(char* log_file_path)
{
	if (NULL == log_file_path)
		return ERROR_NULL;
	errno_t err;
	if (err = fopen_s(&p_log_file, log_file_path, "w"))
		return ERROR_OPENING_FILE;
	return 0;
}

int LogNotify(char* message)
{
	fprintf(p_log_file, message);
	return 0;
}

int CloseLog()
{
	fclose(p_log_file);
	return 0;
}