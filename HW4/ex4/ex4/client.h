#pragma once


#include "config.h"
#include "GameRun.h"


void ClientRun(char *argv[]);

typedef struct client_thread_params_s client_thread_params;

#ifndef client_thread_params_s
struct client_thread_params_s
{
	char *client_log_path;
	int server_port_num;
	char *input_mode;
	char *input_file_path;
};
#endif

/*
Description:
Run client on client mode
Parameters:
None
Returns:
None
*/
void HumanModeRun();

/*
Description:
Run client on file mode
Parameters:
None
Returns:
None
*/
void FileModeRun();

/*
Description:
Enters messages to server to the message queue
Parameters:
char* message
Returns:
None
*/
void SendToServer(char* message);

/*
Description:
Send messages from the queue to the server
Parameters:
None
Returns:
None
*/
void SendToServerThread();

/*
Description:
Reads messages from server and enters them to a queue
Parameters:
None
Returns:
None
*/
void ReceiveFromServerThread();

/*
Description:
WSA cleanup
Parameters:
None
Returns:
None
*/
static void Cleanup_WSA();

/*
Description:
Socket and WSA cleanup
Parameters:
None
Returns:
None
*/
static void Cleanup_Socket_And_WSA();

/*
Description:
Parsing messages from client prompt to server
Parameters:
char* prompt_str, char** p_send_to_server_str
Returns:
0 if true, error code otherwise
*/
int Parser(char* prompt_str, char** p_send_to_server_str);

/*
Description:
Board deparser
Parameters:
char* message_params, int board[][BOARD_WIDTH]
Returns:
None
*/
void BoardDeparse(char* message_params, int board[][BOARD_WIDTH]);

/*
Description:
Board printer
Parameters:
int board[][BOARD_WIDTH], HANDLE consoleHandle
Returns:
None
*/
void PrintBoard(int board[][BOARD_WIDTH], HANDLE consoleHandle);

/*
Description:
Reads messages from queue and analyze them
Parameters:
None
Returns:
None
*/
void ReceiveFromServerAndAnalyze();

/*
Description:
Analyzes server response
Parameters:
char* message
Returns:
None
*/
void AnalyzeServerResponse(char* message);

/*
Description:
Checks if the game ended and closes client log file
Parameters:
None
Returns:
None
*/
void check_if_done();

/*
Description:
Checks if the command enterd is valid
Parameters:
char* command_line
Returns:
1 if valid, 0 otherwise
*/
int ValidPlayCommand(char* command_line);