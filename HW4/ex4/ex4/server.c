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
#include "file_parser.h"
#include "string_parser.h"

static HANDLE users_communication_threads_handles[NUM_OF_PLAYERS];
static user_communication_thread_params users_communication_params_array[NUM_OF_PLAYERS];
static HANDLE users_communication_params_mutex_array[NUM_OF_PLAYERS];
static server_params server_params_var;
static SOCKET MainSocket = INVALID_SOCKET;
static int game_is_on = 0;
static int first_player_checkup = 0;
static int done_checking_first_connection = 0;
static int client_refused = 0;
static HANDLE username_insertion_mutex;
static HANDLE game_handle;
static game_params game_params_var;

void ServerRun(char *argv[])
{
	#pragma region General Initialization
	SOCKADDR_IN service;
	int bindRes;
	int ListenRes;

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (int socket_thread_handles_index = 0;
		socket_thread_handles_index < NUM_OF_PLAYERS;
		socket_thread_handles_index++)
		users_communication_threads_handles[socket_thread_handles_index] = NULL;

	for (int i = 0; i < NUM_OF_PLAYERS; i++)
	{
		users_communication_params_array[i].user_name = (char*)malloc(USER_NAME_MAX_LENGTH);
		users_communication_params_array[i].user_name = "";
		users_communication_params_mutex_array[i] = BasicMutexCreate();
	}

	username_insertion_mutex = BasicMutexCreate();
	server_params_var.mutex_var = BasicMutexCreate();
	#pragma endregion

	#pragma region ReceiveServerArguments
	server_params_var.server_log_file_path = argv[2];
	LogFileInitializer(server_params_var.server_log_file_path);
	server_params_var.server_port_num = atoi(argv[3]);
	#pragma endregion

	#pragma region Initialize Winsock
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (StartupRes != NO_ERROR)
	{
		int error_code = WSAGetLastError();
		// Report we could not find a usable WinSock DLL.    
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

	#pragma region createServerIpAddress
	server_params_var.server_address = inet_addr(SERVER_ADDRESS_STR);
	if (server_params_var.server_address == INADDR_NONE)
	{
		Cleanup_Socket_And_WSA();
		exit(ERROR_CODE_INVALID_SERVER_ADDRESS_STR);
	}
	#pragma endregion

	#pragma region Create Service And Bind.  
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = server_params_var.server_address;
	service.sin_port = htons(server_params_var.server_port_num);
	bindRes = bind(MainSocket, (SOCKADDR*)&service, sizeof(service));
	if (bindRes == SOCKET_ERROR)
	{
		int error_code = WSAGetLastError();
		Cleanup_Socket_And_WSA();
		exit(error_code);
	}
	#pragma endregion

	#pragma region Listen on the Socket.
	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR)
	{
		int error_code = WSAGetLastError();
		Cleanup_Socket_And_WSA();
		exit(error_code);
	}
	#pragma endregion

	server_params_var.num_of_connected_players = 0;
	first_player_checkup = 0;

	HANDLE human_mode_run_handle = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)checkIfGameEndedThread,
		NULL,
		0,
		NULL
	);

	while (1)
	{
		user_communication_thread_params user_communication_param;
		int socket_thread_handles_index;

		BasicMutexWait(&(server_params_var.mutex_var));
		printf("Num of Connected Players: %d\n", server_params_var.num_of_connected_players);
		BasicMutexRelease(server_params_var.mutex_var);
		
		#pragma region Accept A new Connection
		SOCKET PlayerSocket = accept(MainSocket, NULL, NULL);
		if (PlayerSocket == INVALID_SOCKET)
		{
			int error_code = WSAGetLastError();
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
			continue;
		}
		#pragma endregion

		BasicMutexWait(&(server_params_var.mutex_var));
		BasicMutexWait(&(users_communication_params_mutex_array[0]));
		BasicMutexWait(&(users_communication_params_mutex_array[1]));

		#pragma region Decline third and further connections
		if (client_refused)
			server_params_var.num_of_connected_players = 1;

		if (server_params_var.num_of_connected_players == NUM_OF_PLAYERS)
		{
			SendString(GAME_IS_OCCUPIED_MESSAGE, PlayerSocket);
			shutdown(PlayerSocket, SD_BOTH);
			closesocket(PlayerSocket);
			BasicMutexRelease((server_params_var.mutex_var));
			BasicMutexRelease((users_communication_params_mutex_array[0]));
			BasicMutexRelease((users_communication_params_mutex_array[1]));
			continue;
		}
		#pragma endregion

		#pragma region Accept a new Player
		if (!client_refused)
			printf("Client Connected.\n");
		
		socket_thread_handles_index = server_params_var.num_of_connected_players;
		server_params_var.num_of_connected_players++;
		
		if (client_refused)
		{
			client_refused = 0;
		}

		if (server_params_var.num_of_connected_players == NUM_OF_PLAYERS)
		{
			TransferResult_t RecvRes;
			char *tmp = 0;
			RecvRes = SendString("", users_communication_params_array[0].PlayerSocket);
			if (RecvRes == TRNS_FAILED)
			{
				socket_thread_handles_index--;
				server_params_var.num_of_connected_players--;
			}
		}


		user_communication_param.PlayerSocket = PlayerSocket;
		user_communication_param.p_user_num = server_params_var.num_of_connected_players;
		users_communication_params_array[socket_thread_handles_index] = user_communication_param;

		users_communication_threads_handles[socket_thread_handles_index] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)UserCommunicationInitializationThread,
			&(users_communication_params_array[socket_thread_handles_index]),
			0,
			NULL
		);
		#pragma endregion

		BasicMutexRelease((server_params_var.mutex_var));
		BasicMutexRelease((users_communication_params_mutex_array[0]));
		BasicMutexRelease((users_communication_params_mutex_array[1]));
	}
}

void checkIfGameEndedThread()
{
	while (1)
	{
		if (game_params_var.game_ended)
		{
			server_params_var.num_of_connected_players = 0;
			game_is_on = 0;
			game_params_var.game_ended = 0;
		}
	}
}

//UserCommunicationThread thread is the thread that opens for each successful client connection and "talks" to the client.
static DWORD UserCommunicationInitializationThread(user_communication_thread_params *user_communication_param)
{
	char *SendStr = (char*)malloc(SEND_STR_SIZE);
	char *SendStr2 = (char*)malloc(SEND_STR_SIZE);
	char *tmp = 0;

	TransferResult_t RecvRes;

	BasicMutexWait(&(users_communication_params_mutex_array[user_communication_param->p_user_num - 1]));
	
	BasicSend(WELCOME_MESSAGE, user_communication_param->PlayerSocket);
	printf("welcome message was sent to client\n");
	
	#pragma region Get a valid username
	tmp = 0; // so it will update according to the new received.
	RecvRes = ReceiveString(&tmp, user_communication_param->PlayerSocket);
	printf("Received username from client: %s\n", tmp);
	// mutex to make sure not the same username
	BasicMutexWait(&(username_insertion_mutex));
	user_communication_param->user_name = tmp;


	//check if the username name is the same as the other
	if (STRINGS_ARE_EQUAL(user_communication_param->user_name,
		users_communication_params_array[2 - (user_communication_param->p_user_num)].user_name))
	{
		SendStr = MessageBuilder1Param("NEW_USER_DECLINED:", "username is taken or an empty username. try another username");
		LogNotify(SendStr);
		LogNotify("\n");

		BasicMutexRelease((username_insertion_mutex));
		BasicSend(SendStr, user_communication_param->PlayerSocket);
		client_refused = 1;
	}
	else
	{
		BasicMutexRelease((username_insertion_mutex));
	}
	#pragma endregion

	if (client_refused)
	{
		//BasicMutexRelease((server_params_var.mutex_var));
		BasicMutexRelease(users_communication_params_mutex_array[user_communication_param->p_user_num - 1]);
		closesocket(user_communication_param->PlayerSocket);
	}
	else
	{
		BasicMutexWait(&(server_params_var.mutex_var));

		sprintf(SendStr2, "%d", server_params_var.num_of_connected_players);
		SendStr = MessageBuilder1Param("NEW_USER_ACCEPTED:", SendStr2);
		LogNotify(SendStr);
		LogNotify("\n");
		BasicMutexRelease((server_params_var.mutex_var));

		BasicSend(SendStr, user_communication_param->PlayerSocket);

		BasicMutexRelease(users_communication_params_mutex_array[user_communication_param->p_user_num - 1]);

		user_communication_param->is_ready_to_play = 1;

		BasicMutexWait(&(server_params_var.mutex_var));
		BasicMutexWait(&(users_communication_params_mutex_array[0]));
		BasicMutexWait(&(users_communication_params_mutex_array[1]));

		#pragma region If ready Start a game
		if (server_params_var.num_of_connected_players == NUM_OF_PLAYERS &&
			!game_is_on &&
			(users_communication_params_array[0]).is_ready_to_play == 1 &&
			(users_communication_params_array[1]).is_ready_to_play == 1)
		{
			game_is_on = 1;

			game_params_var.server_params_var = server_params_var;
			game_params_var.user_communication_thread_params_array = users_communication_params_array;
			game_params_var.game_ended = 0;

			game_handle = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)GameRunning,
				&(game_params_var),
				0,
				NULL
			);
		}
		if (server_params_var.num_of_connected_players != NUM_OF_PLAYERS)
		{
			game_is_on = 0;
		}
		#pragma endregion

		BasicMutexRelease((server_params_var.mutex_var));
		BasicMutexRelease((users_communication_params_mutex_array[0]));
		BasicMutexRelease((users_communication_params_mutex_array[1]));
	}

	return 0;
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