#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define ANSI_COLOR_RED     "\33[0:31m\\]"
#define ANSI_COLOR_YELLOW  "\033[22;32m"
#define ANSI_COLOR_RESET   "\033[22;30m"


#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <Windows.h>
#include "string_parser.h"
#include "socket_config.h"
#include "socket_send_recv.h"
#include "client.h"
#include "GameRun.h"
#include "file_parser.h"
#include "config.h"

#pragma warning(disable : 4996)

static SOCKET MainSocket = INVALID_SOCKET;

static HANDLE recv_queue_mutex;
static HANDLE send_queue_mutex;
static send_recv_queue send_queue;
static send_recv_queue recv_queue;
static client_thread_params thread_param;

static HANDLE hConsole;

static int game_ended = 0;
static int game_started = 0;
static int my_turn = 0;
static int first_recv_from_server_message = 1;
static int username_try = 1;
static int wait_for_turn_switch = 0;
static int wait_for_first_turn_switch = 1;
static int socket_closed = 0;

static HANDLE printing_board_mutex;

static char* log_tmp;

void ClientRun(char *argv[])
{
	#pragma region General Initialization
	printing_board_mutex = BasicMutexCreate();
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	send_queue.p_first_send_recv_queuer = 0;
	send_queue.p_end_send_recv_queuer = 0;
	SOCKADDR_IN service;
	thread_param.client_log_path = argv[2];
	thread_param.server_port_num = atoi(argv[3]);
	thread_param.input_mode = argv[4];
	int human_mode = 1;
	if (strcmp(thread_param.input_mode, "file") == 0)
	{
		thread_param.input_file_path = argv[5];
		human_mode = 0;
	}
	else
	{
		thread_param.input_file_path = NULL;
	}

	LogFileInitializer(thread_param.client_log_path);

	recv_queue_mutex = BasicMutexCreate();
	send_queue_mutex = BasicMutexCreate();
	#pragma endregion
	
	#pragma region Initialize Winsock
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (StartupRes != NO_ERROR)
	{
		int error_code = WSAGetLastError();
		// Report we could not find a usable WinSock DLL.    
		printf("error %ld at WSAStartup( ), ending program.\n", error_code);
		exit(error_code);
	}
	/* The WinSock DLL is acceptable. Proceed. */
	#pragma endregion

	#pragma region Create a socket.    
	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (MainSocket == INVALID_SOCKET)
	{
		int error_code = WSAGetLastError();
		printf("Error at socket( ): %ld\n", error_code);
		Cleanup_WSA();
		exit(error_code);
	}
	#pragma endregion

	#pragma region createServerIpAdress
	unsigned long server_address = inet_addr(SERVER_ADDRESS_STR);
	if (server_address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
			SERVER_ADDRESS_STR);
		Cleanup_Socket_And_WSA();
		exit(ERROR_CODE_INVALID_SERVER_ADDRESS_STR);
	}
	#pragma endregion

	#pragma region connect
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = server_address;
	service.sin_port = htons(thread_param.server_port_num);
	if (connect(MainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		printf("Failed to connect.\n");
		int error_code = WSAGetLastError();
		Cleanup_Socket_And_WSA();

		log_tmp = (char*)malloc(SEND_STR_SIZE);
		sprintf(log_tmp, "Failed connecting to server on %s:%d. Exiting\n", SERVER_ADDRESS_STR, thread_param.server_port_num);
		LogNotify(log_tmp);

		exit(error_code);
	}
	else
	{
		log_tmp = (char*)malloc(SEND_STR_SIZE);
		sprintf(log_tmp, "connected to server on %s:%d\n", SERVER_ADDRESS_STR, thread_param.server_port_num);
		LogNotify(log_tmp);
	}
	#pragma endregion

	#pragma region open send and receive threads
	HANDLE send_to_server_thread_handle = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)SendToServerThread,
		NULL,
		0,
		NULL);
	HANDLE receive_from_server_thread_handle = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ReceiveFromServerThread,
		NULL,
		0,
		NULL
	);
	HANDLE queue_analzer_handle = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ReceiveFromServerAndAnalyze,
		NULL,
		0,
		NULL
	);
	#pragma endregion

	if (human_mode == 1)
	{
		HANDLE human_mode_run_handle = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)HumanModeRun,
			NULL,
			0,
			NULL
		);
		while (!socket_closed)
		{
			BasicMutexWait(&printing_board_mutex);
			BasicMutexRelease(printing_board_mutex);
		};
		TerminateThread(HumanModeRun, NULL);
	}
	else
	{
		HANDLE file_mode_run_handle = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)FileModeRun,
			NULL,
			0,
			NULL
		);
		while (!socket_closed)
		{
			BasicMutexWait(&printing_board_mutex);
			BasicMutexRelease(printing_board_mutex);
		};
		TerminateThread(FileModeRun, NULL);
	}

	CloseLog();

	Cleanup_Socket_And_WSA();

	return;
}

void HumanModeRun()
{
	while (1)
	{
		BasicMutexWait(&printing_board_mutex);
		BasicMutexRelease(printing_board_mutex);

		char * prompt_str = (char*)malloc(SEND_STR_SIZE);
		char * send_to_server_str = (char*)malloc(SEND_STR_SIZE);
		fgets(prompt_str, strlen(prompt_str), stdin);
		int parser_ret = Parser(prompt_str, &send_to_server_str);
		if (parser_ret == ERROR_ILLEGAL_COMMAND)
		{
			printf("Error: Illegal command\n");

			log_tmp = (char*)malloc(SEND_STR_SIZE);
			sprintf(log_tmp, "Error: Illegal command\n");
			LogNotify(log_tmp);

			continue;
		}
		if (parser_ret == ERROR_GAME_HAS_NOT_STARTED)
		{
			printf("Error: Game has not started\n");
			continue;
		}
		SendToServer(send_to_server_str);

		if (socket_closed)
			break;
	}
}

void FileModeRun()
{
	while (first_recv_from_server_message == 1); // wait for welcome message

	FILE * command_file;
	char command_line[SEND_STR_SIZE];
	int first_command = 1;

	command_file = fopen(thread_param.input_file_path, "r");
	if (command_file == NULL)
		exit(ERROR_OPENING_FILE);

	while (fgets(command_line, sizeof(command_line), command_file) != NULL)
	{
		if (first_command == 0)
		{
			// username was sent
			while (username_try == 1); // wait for it to be accepted
		}

		if (ValidPlayCommand(command_line))
		{
			while (game_started == 0);
			while (wait_for_first_turn_switch == 1);
			while (my_turn == 0); // wait till my turn

			wait_for_turn_switch = 1;
		}
		printf("%s", command_line);
		char * send_to_server_str = (char*)malloc(SEND_STR_SIZE);
		int parser_ret = Parser(command_line, &send_to_server_str);
		if (parser_ret == ERROR_ILLEGAL_COMMAND)
		{
			printf("Illegal command\n");

			log_tmp = (char*)malloc(SEND_STR_SIZE);
			sprintf(log_tmp, "Error: Illegal command\n");
			LogNotify(log_tmp);

			continue;
		}
		if (parser_ret == ERROR_GAME_HAS_NOT_STARTED)
		{
			printf("Error: Game has not started\n");
			continue;
		}
		SendToServer(send_to_server_str);
		first_command = 0;

		if (ValidPlayCommand(command_line))
		{
			while (wait_for_turn_switch != 0); // wait for the play to be made
		}

		if (socket_closed)
			break;
	}

	fclose(command_file);
}

int Parser(char* prompt_str, char** p_send_to_server_str)
{
	if (prompt_str[strlen(prompt_str) - 1] == '\n')
	{
		prompt_str[strlen(prompt_str) - 1] = NULL; // get rid of '\n'
	}
	
	if (username_try == 0)
	{
		if (StringStartsWith("play ", prompt_str))
		{
			char* column = prompt_str + strlen("play ");
			if (strlen(column) != 1 /*not a digit*/)
			{
				return ERROR_ILLEGAL_COMMAND;
			}
			if (!(column[0] >= '0' && column[0] <= '6'))
			{
				return ERROR_ILLEGAL_COMMAND;
			}
			if (game_started == 0)
			{
				return ERROR_GAME_HAS_NOT_STARTED;
			}
			*p_send_to_server_str = MessageBuilder1Param("PLAY_REQUEST:", column);
		}
		else if (StringStartsWith("message ", prompt_str))
		{
			char* message_pass_to_other_user = prompt_str + strlen("message ");
			*p_send_to_server_str = MessageBuilder1Param("SEND_MESSAGE:", message_pass_to_other_user);
		}
		else if (STRINGS_ARE_EQUAL("exit", prompt_str))
		{
			// exit
			socket_closed = 1;
			closesocket(MainSocket);
		}
		else
		{
			return ERROR_ILLEGAL_COMMAND;
		}
	}
	else
	{
		strcpy(*p_send_to_server_str, prompt_str);
	}
	return 0;
}

void SendToServer(char* message)
{
	send_recv_queuer* p_send_queuer_var = (send_recv_queuer*)malloc(sizeof(send_recv_queuer));
	p_send_queuer_var->message = message;
	p_send_queuer_var->p_next_send_recv_queuer = 0;

	BasicMutexWait(&send_queue_mutex);
	p_send_queuer_var->p_last_send_recv_queuer = send_queue.p_end_send_recv_queuer;
	if (send_queue.p_end_send_recv_queuer == NULL) //empty queue
	{
		send_queue.p_first_send_recv_queuer = p_send_queuer_var;
	}
	else
	{
		send_queue.p_end_send_recv_queuer->p_next_send_recv_queuer = p_send_queuer_var;
	}
	send_queue.p_end_send_recv_queuer = p_send_queuer_var;
	BasicMutexRelease(send_queue_mutex);
	return;
}

void ReceiveFromServerAndAnalyze()
{
	while (1)
	{
		BasicMutexWait(&printing_board_mutex);
		BasicMutexRelease(printing_board_mutex);

		BasicMutexWait(&recv_queue_mutex);
		if (recv_queue.p_end_send_recv_queuer != NULL)
		{
			char* message = recv_queue.p_end_send_recv_queuer->message;
			if (recv_queue.p_end_send_recv_queuer->p_last_send_recv_queuer == NULL) // only one queuer
			{
				recv_queue.p_first_send_recv_queuer = NULL;
			}
			else
			{
				recv_queue.p_end_send_recv_queuer->p_last_send_recv_queuer->p_next_send_recv_queuer = 0;
			}
			recv_queue.p_end_send_recv_queuer = recv_queue.p_end_send_recv_queuer->p_last_send_recv_queuer;
			
			log_tmp = (char*)malloc(SEND_STR_SIZE);
			sprintf(log_tmp, "Received from server: %s\n", message);
			LogNotify(log_tmp);

			AnalyzeServerResponse(message);
		}
		BasicMutexRelease(recv_queue_mutex);

		if (socket_closed)
			break;
	}
	
}

void check_if_done()
{
	while (!socket_closed);
	CloseLog();

	Cleanup_Socket_And_WSA();
}

void SendToServerThread()
{
	while (1)
	{
		BasicMutexWait(&printing_board_mutex);
		BasicMutexRelease(printing_board_mutex);

		char* message = 0;
		BasicMutexWait(&send_queue_mutex);
		if (send_queue.p_first_send_recv_queuer != NULL)
		{
			message = send_queue.p_first_send_recv_queuer->message;
			if (send_queue.p_first_send_recv_queuer->
				p_next_send_recv_queuer == NULL) // only one queuer
			{
				send_queue.p_end_send_recv_queuer = 0;
			}
			else
			{
				send_queue.p_first_send_recv_queuer->
					p_next_send_recv_queuer->p_last_send_recv_queuer = 0;
			}
			send_queue.p_first_send_recv_queuer = send_queue.p_first_send_recv_queuer->
				p_next_send_recv_queuer;
			BasicMutexRelease(send_queue_mutex);

			log_tmp = (char*)malloc(SEND_STR_SIZE);
			sprintf(log_tmp, "Sent to server: %s\n", message);
			LogNotify(log_tmp);

			BasicSend(message, MainSocket);
		}
		else
		{
			BasicMutexRelease(send_queue_mutex);
		}
		
		if (socket_closed)
		{
			break;
		}
			
	}
}

void ReceiveFromServerThread()
{
	while (1)
	{


		char *message = 0;
		TransferResult_t RecvRes;
		RecvRes = ReceiveString(&message, MainSocket);
		
		send_recv_queuer* p_recv_queuer_var = (send_recv_queuer*)malloc(sizeof(send_recv_queuer));
		p_recv_queuer_var->message = message;
		p_recv_queuer_var->p_last_send_recv_queuer = 0;

		BasicMutexWait(&recv_queue_mutex);
		p_recv_queuer_var->p_next_send_recv_queuer = recv_queue.p_first_send_recv_queuer;
		if (recv_queue.p_first_send_recv_queuer == NULL)
		{
			// empty queue
			recv_queue.p_end_send_recv_queuer = p_recv_queuer_var;
		}
		else
		{
			p_recv_queuer_var->p_next_send_recv_queuer->p_last_send_recv_queuer = p_recv_queuer_var;
		}
		recv_queue.p_first_send_recv_queuer = p_recv_queuer_var;
		BasicMutexRelease(recv_queue_mutex);
		
		if (socket_closed)
			break;
	}
}

void AnalyzeServerResponse(char* message)
{
	char *message_prefix;
	char *message_params;
	if (first_recv_from_server_message)
	{
		first_recv_from_server_message = 0;
		printf(message);

	}
	else
	{
		int message_without_params = MessageDeparse(&message_prefix, &message_params, message);

		if (STRINGS_ARE_EQUAL(message_prefix, "GAME_STARTED"))
		{
			printf("Game is on!");
			game_started = 1;
		}
		else if (STRINGS_ARE_EQUAL(message_prefix, "BOARD_VIEW"))
		{
			int board[BOARD_HEIGHT][BOARD_WIDTH];
			BoardDeparse(message_params, board);
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			PrintBoard(board, hConsole);

		}
		else if (STRINGS_ARE_EQUAL(message_prefix, "TURN_SWITCH"))
		{
			printf("%s's turn", message_params);
			my_turn = 1 - my_turn; // change turn
			wait_for_turn_switch = 0;
			wait_for_first_turn_switch = 0;
		}
		else if (STRINGS_ARE_EQUAL(message_prefix, "PLAY_ACCEPTED"))
		{
			printf("Well played");
		}
		else if (STRINGS_ARE_EQUAL(message_prefix, "PLAY_DECLINED"))
		{
			printf("Error: %s", message_params);
		}
		else if (STRINGS_ARE_EQUAL(message_prefix, "GAME_ENDED"))
		{
			if (STRINGS_ARE_EQUAL("Tie", message_params))
			{
				printf("Game ended. Everybody wins!");
			}
			else
			{
				printf("Game ended. The winner is %s!", message_params);
			}
			BasicSend("Game ended:!", MainSocket);
			game_ended = 1;
			socket_closed = 1;
		}
		else if (STRINGS_ARE_EQUAL(message_prefix, "RECEIVE_MESSAGE"))
		{
			printf(message_params);
		}
		else if (STRINGS_ARE_EQUAL(message_prefix, "NEW_USER_ACCEPTED"))
		{
			printf("%s, you are client #%s", message_prefix, message_params);
			my_turn = 2 - (message_params[0] - '0');
			my_turn = 1 - my_turn; // a turn switch is going to be sent at the start of the game and this is its cancelation
			username_try = 0;
		}
		else if (STRINGS_ARE_EQUAL(message_prefix, "NEW_USER_DECLINED"))
		{
			printf("Request to join was refused");
			//exit
			socket_closed = 1;
			closesocket(MainSocket);
			
		}
		else
		{
			printf(message);
		}
		printf("\n");
	}
}

static void Cleanup_WSA()
{
	if (WSACleanup() == SOCKET_ERROR)
	{
		int error_code = WSAGetLastError();
		exit(error_code);
	}
	return;
}

static void Cleanup_Socket_And_WSA()
{
	int error_code = 0;
	if (closesocket(MainSocket) == SOCKET_ERROR)
	{
		error_code = WSAGetLastError();
		Cleanup_WSA();
	}
	if (error_code != 0)
	{
		exit(error_code);
	}
}

/***********************************************************
* This function prints the board, and uses O as the holes.
* The disks are presented by red or yellow backgrounds.
* Input: A 2D array representing the board and the console handle
* Output: Prints the board, no return value
************************************************************/
void PrintBoard(int board[][BOARD_WIDTH], HANDLE consoleHandle)
{
	BasicMutexWait(&printing_board_mutex);

	//char * board_str = (char*)malloc(3000);
	//board_str[0] = 0;

	int row, column;

	//Draw the board
	for (row = 0; row < BOARD_HEIGHT; row++)
	{
		for (column = 0; column < BOARD_WIDTH; column++)
		{
			//strncat(board_str, "| ", strlen("| "));
			printf("| ");
			if (board[row][column] == RED_PLAYER)
			{
				//strncat(board_str, ANSI_COLOR_RED, strlen(ANSI_COLOR_RED));
				SetConsoleTextAttribute(consoleHandle, RED);
			}
			else if (board[row][column] == YELLOW_PLAYER)
			{
				//strncat(board_str, ANSI_COLOR_YELLOW, strlen(ANSI_COLOR_YELLOW));
				SetConsoleTextAttribute(consoleHandle, YELLOW);
			}

			//strncat(board_str, "O", strlen("O"));
			printf("O");

			//strncat(board_str, ANSI_COLOR_RESET, strlen(ANSI_COLOR_RESET));
			SetConsoleTextAttribute(consoleHandle, BLACK);
			//strncat(board_str, " ", strlen(" "));
 			printf(" ");
		}
		//strncat(board_str, "\n", strlen("\n"));
		printf("\n");

		//Draw dividing line between the rows
		for (column = 0; column < BOARD_WIDTH; column++)
		{
			//strncat(board_str, "----", strlen("----"));
			printf("----");
		}
		//strncat(board_str, "\n", strlen("\n"));
		printf("\n");
	}
	//printf(board_str);
	BasicMutexRelease(printing_board_mutex);
}

void BoardDeparse(char* message_params, int board_to_print[][BOARD_WIDTH])
{
	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			board_to_print[i][j] = message_params[i*BOARD_WIDTH + j] - '0';
		}
	}
	return;
}

int ValidPlayCommand(char* command_line)
{
	int deleted_raw_jump = 0;
	if (command_line[strlen(command_line) - 1] == '\n')
	{
		command_line[strlen(command_line) - 1] = NULL; // get rid of '\n'
		deleted_raw_jump = 1;
	}
	int valid_play_command = 0;
	if (StringStartsWith("play ", command_line))
	{
		char* column = command_line + strlen("play ");
		if (strlen(column) == 1 /*a digit*/ &&
			(column[0] >= '0' && column[0] <= '6'))
		{
			valid_play_command = 1;
		}
	}
	if (deleted_raw_jump)
	{
		command_line[strlen(command_line)] = '\n';
	}

	return valid_play_command;
}