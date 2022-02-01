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

static send_recv_queue send_queues_array[NUM_OF_PLAYERS];
static send_recv_queue recv_queue;
static HANDLE send_to_users_threads_handles[NUM_OF_PLAYERS];
static HANDLE receive_from_users_thread_handles[NUM_OF_PLAYERS];
static int game_ended = 0;
static int user_num_turn = 1;
static int player1_disconnected = 0;
static int player2_disconnected = 0;
static int num_of_clients_received_game_ended = 0;
static int end_counter = 0;
static int end_connection_final = 0;
static int board[BOARD_HEIGHT][BOARD_WIDTH]; // 0 for empty

DWORD GameRunning(game_params* p_game_params_var)
{
	#pragma region General Initialization
	user_num_turn = 1;
	game_ended = 0;
	send_queues_array[0].p_first_send_recv_queuer = 0;
	send_queues_array[1].p_first_send_recv_queuer = 0;
	send_queues_array[0].p_end_send_recv_queuer = 0;
	send_queues_array[1].p_end_send_recv_queuer = 0;
	recv_queue.p_first_send_recv_queuer = 0;
	recv_queue.p_end_send_recv_queuer = 0;

	for (int i = 0; i < NUM_OF_PLAYERS; i++)
	{
		send_to_users_threads_handles[i] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)SendToUsersThread,
			&(p_game_params_var->user_communication_thread_params_array[i]),
			0,
			NULL);
		receive_from_users_thread_handles[i] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)ReceiveFromUserThread,
			&(p_game_params_var->user_communication_thread_params_array[i]),
			0,
			NULL
		);
	}

	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			board[i][j] = EMPTY;
		}
	}
	#pragma endregion

	MessageToUsersGameStarted();
	MessageToUsersBoardView();
	MessageToUsersTurnSwitch(p_game_params_var);

	// each loop is a command.
	player1_disconnected = 0;
	player2_disconnected = 0;

	while ((game_ended == 0  || !(player1_disconnected || player2_disconnected)))
	{	
		int* p_message_user_num = (int*)malloc(sizeof(int));
		char *message = ReceiveNextCommandFromUsers(p_message_user_num);
		if (player1_disconnected)
		{
			char *SendStr = MessageBuilder1Param("GAME_ENDED:", "you because the other player exited");
			SendToUser(SendStr, 1);
			LogNotify(SendStr);
			CloseLog();
			break;
		}
		if (player2_disconnected)
		{
			char *SendStr = MessageBuilder1Param("GAME_ENDED:", "you because the other player exited");
			SendToUser(SendStr, 0);
			LogNotify(SendStr);
			CloseLog();
			break;
		}
		char *message_prefix = "";
		char *message_params = "";
		int invalid_message = MessageDeparse(&message_prefix, &message_params, message);
		if (invalid_message != 0)
		{
			SendToUser("Error: Illegal command", (*p_message_user_num) - 1);
			continue;
		}

		if (STRINGS_ARE_EQUAL(message_prefix, "PLAY_REQUEST"))
		{
			if (*p_message_user_num != user_num_turn)
			{
				SendToUser(MessageBuilder1Param("PLAY_DECLINED:","Not your turn"), (*p_message_user_num) - 1);
				continue;
			}
			int valid_move = playMove(atoi(message_params), user_num_turn);
			if (valid_move == INVALID_GAME_PLAY)
			{
				SendToUser(MessageBuilder1Param("PLAY_DECLINED:","Illegal move"), user_num_turn-1);
				continue;
			}
			else
			{
				SendToUser("PLAY_ACCEPTED", user_num_turn - 1);
				user_num_turn = 3 - user_num_turn; //change turn
				MessageToUsersBoardView();
				CheckIfGameEnded(p_game_params_var);
				if (game_ended != 0)
				{
					CloseLog();
					break;
				}
				else
				{
					MessageToUsersTurnSwitch(p_game_params_var);
				}
			}
		}
		else if (STRINGS_ARE_EQUAL(message_prefix, "SEND_MESSAGE"))
		{
			SendToUser(MessageBuilder3Param("RECEIVE_MESSAGE:",
											p_game_params_var->user_communication_thread_params_array[*p_message_user_num - 1].user_name, ": ",
											message_params),
						2 - *p_message_user_num /*other user*/);
		}
		else
		{
			SendToUser("Error: Illegal command", (*p_message_user_num) - 1);
			continue;
		}
	}

	//exit properly
	while (!(player1_disconnected && player2_disconnected));
	
	
	closesocket(p_game_params_var->user_communication_thread_params_array[0].PlayerSocket);
	closesocket(p_game_params_var->user_communication_thread_params_array[1].PlayerSocket);

	p_game_params_var->game_ended = 1;

	return 0;
}

int checkIfBoardFull()
{
	int full = 0;
	for (int j = 0; j < BOARD_WIDTH; j++)
	{
		if (board[BOARD_HEIGHT - 1][j] != 0)
		{
			full++;
		}
	}
	if (full == BOARD_WIDTH)
	{
		return TIE;
	}
	return 0;
}

int checkIfSomeoneWon()
{
	int ret = checkIfSomeoneWonHorizontally();
	if (ret != 0)
	{
		return ret;
	}
	ret = checkIfSomeoneWonVertically();
	if (ret != 0)
	{
		return ret;
	}
	ret = checkIfSomeoneWonDiagonally();
	return ret;
}

int checkIfSomeoneWonHorizontally()
{
	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < BOARD_WIDTH - 3; j++)
		{
			if (board[i][j] != EMPTY &&
				board[i][j] == board[i][j + 1] &&
				board[i][j] == board[i][j + 2] &&
				board[i][j] == board[i][j + 3])
			{
				return board[i][j];
			}
		}
	}
	return 0;
}

int checkIfSomeoneWonDiagonally()
{
	for (int i = 0; i < BOARD_HEIGHT-3; i++)
	{
		for (int j = 0; j < BOARD_WIDTH - 3; j++)
		{
			if (board[i][j] != EMPTY &&
				board[i][j] == board[i+1][j + 1] &&
				board[i][j] == board[i+2][j + 2] &&
				board[i][j] == board[i+3][j + 3])
			{
				return board[i][j];
			}
		}
	}
	for (int i = 0; i < BOARD_HEIGHT - 3; i++)
	{
		for (int j = 3; j < BOARD_WIDTH; j++)
		{
			if (board[i][j] != EMPTY &&
				board[i][j] == board[i + 1][j - 1] &&
				board[i][j] == board[i + 2][j - 2] &&
				board[i][j] == board[i + 3][j - 3])
			{
				return board[i][j];
			}
		}
	}
	return 0;
}

int checkIfSomeoneWonVertically()
{
	for (int i = 0; i < BOARD_HEIGHT-3; i++)
	{
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			if (board[i][j] != EMPTY &&
				board[i][j] == board[i+1][j] &&
				board[i][j] == board[i+2][j] &&
				board[i][j] == board[i+3][j])
			{
				return board[i][j];
			}
		}
	}
	return 0;
}

int playMove(int col, int p_user_num)
{
	for (int i = BOARD_HEIGHT - 1 ; i >= 0; i--)
	{
		if (board[i][col] == EMPTY)
		{
			board[i][col] = p_user_num;
			return 0;
		}
	}
	return INVALID_GAME_PLAY;
}

void ReceiveFromUserThread(user_communication_thread_params user_communication_param_var)
{
	SOCKET sd = user_communication_param_var.PlayerSocket;
	while (1)
	{
		char *message = 0;
		int *p_user_num = (int*)malloc(sizeof(int));

		TransferResult_t RecvRes;
		RecvRes = ReceiveString(&message, sd);
		if (RecvRes == TRNS_FAILED)
		{
			if (user_communication_param_var.p_user_num == 1)
			{
				player1_disconnected = 1;
			}
			if (user_communication_param_var.p_user_num == 2)
			{
				player2_disconnected = 1;
			}
			return;
		}
		
		send_recv_queuer* p_recv_queuer_var = (send_recv_queuer*)malloc(sizeof(send_recv_queuer));
		p_recv_queuer_var->message = message;
		*p_user_num = user_communication_param_var.p_user_num;
		p_recv_queuer_var->p_user_num = p_user_num;
		p_recv_queuer_var->p_last_send_recv_queuer = 0;

		p_recv_queuer_var->p_next_send_recv_queuer = recv_queue.p_first_send_recv_queuer;
		if (recv_queue.p_first_send_recv_queuer == NULL)
		{
			// empty queue
			recv_queue.p_end_send_recv_queuer = p_recv_queuer_var;
		}
		else
		{
			recv_queue.p_first_send_recv_queuer->p_last_send_recv_queuer = p_recv_queuer_var;
		}
		recv_queue.p_first_send_recv_queuer = p_recv_queuer_var;
	}
}

void SendToUsersThread(user_communication_thread_params user_communication_param_var)
{
	int user_index = user_communication_param_var.p_user_num - 1;
	SOCKET sd = user_communication_param_var.PlayerSocket;
	int counter = 0;
	while (1)
	{
		if (send_queues_array[user_index].p_first_send_recv_queuer != NULL)
		{
			counter = 0;
			char* message = send_queues_array[user_index].p_first_send_recv_queuer->message;
			BasicSend(message, sd);
			if (send_queues_array[user_index].p_first_send_recv_queuer->
				p_next_send_recv_queuer == NULL) // only one queuer
			{
				send_queues_array[user_index].p_end_send_recv_queuer = 0;
			}
			else
			{
				send_queues_array[user_index].p_first_send_recv_queuer->
					p_next_send_recv_queuer->p_last_send_recv_queuer = 0;
			}
			send_queues_array[user_index].p_first_send_recv_queuer = send_queues_array[user_index].p_first_send_recv_queuer->
				p_next_send_recv_queuer;
			
		}
		
		if (user_communication_param_var.p_user_num == 1)
		{
			if (player1_disconnected)
			{
				return;
			}
		}
		if (user_communication_param_var.p_user_num == 2)
		{
			if (player2_disconnected)
			{
				return;
			}
		}
		/*if (num_of_clients_received_game_ended == 2)
			break;*/
	}
}

char* ReceiveNextCommandFromUsers(int* p_user_num)
{
	while (1)
	{
		if (recv_queue.p_end_send_recv_queuer != NULL)
		{
			char* message = recv_queue.p_end_send_recv_queuer->message;
			*p_user_num = *(recv_queue.p_end_send_recv_queuer->p_user_num);

			if (recv_queue.p_end_send_recv_queuer->p_last_send_recv_queuer == NULL) // only one queuer
			{
				recv_queue.p_first_send_recv_queuer = 0;
			}
			else
			{
				recv_queue.p_end_send_recv_queuer->p_last_send_recv_queuer->p_next_send_recv_queuer = 0;
			}
			recv_queue.p_end_send_recv_queuer = recv_queue.p_end_send_recv_queuer->p_last_send_recv_queuer;

			return message;
		}
		if (player1_disconnected || player2_disconnected)
		{
			return 0;
		}
		/*if (num_of_clients_received_game_ended == 2)
			break;*/
	}
	return 0;
}

void SendToUser(char* message, int user_index)
{
	send_recv_queuer* p_send_queuer_var = (send_recv_queuer*)malloc(sizeof(send_recv_queuer));
	p_send_queuer_var->message = message;
	p_send_queuer_var->p_user_num = (int*)malloc(sizeof(int));
	*(p_send_queuer_var->p_user_num) = user_index + 1;
	p_send_queuer_var->p_next_send_recv_queuer = 0;

	p_send_queuer_var->p_last_send_recv_queuer = send_queues_array[user_index].p_end_send_recv_queuer;
	if (send_queues_array[user_index].p_end_send_recv_queuer == NULL) //empty queue
	{
		send_queues_array[user_index].p_first_send_recv_queuer = p_send_queuer_var;
	}
	else
	{
		send_queues_array[user_index].p_end_send_recv_queuer->p_next_send_recv_queuer = p_send_queuer_var;
	}
	send_queues_array[user_index].p_end_send_recv_queuer = p_send_queuer_var;
	return;
}

char* BoardParse()
{
	char* board_str = (char*)malloc(BOARD_HEIGHT*BOARD_WIDTH + 1);
	board_str[BOARD_HEIGHT*BOARD_WIDTH] = 0;
	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			board_str[i*BOARD_WIDTH + j] = board[i][j] + '0';
		}
	}
	return board_str;
	
}

void MessageToUsersGameStarted()
{
	SendToUser("GAME_STARTED", 0);
	SendToUser("GAME_STARTED", 1);
}

void MessageToUsersBoardView()
{
	char* board_str = MessageBuilder1Param("BOARD_VIEW:", BoardParse());
	SendToUser(board_str, 0);
	SendToUser(board_str, 1);
}

void MessageToUsersTurnSwitch(game_params* p_game_params_var)
{
	SendToUser(MessageBuilder1Param("TURN_SWITCH:", p_game_params_var->user_communication_thread_params_array[user_num_turn - 1].user_name), 0);
	SendToUser(MessageBuilder1Param("TURN_SWITCH:", p_game_params_var->user_communication_thread_params_array[user_num_turn - 1].user_name), 1);
}

void MessageToUsersGameEnded(char* score_result)
{
	SendToUser(MessageBuilder1Param("GAME_ENDED:", score_result), 0);
	SendToUser(MessageBuilder1Param("GAME_ENDED:", score_result), 1);

}

void CheckIfGameEnded(game_params* p_game_params_var)
{
	game_ended = checkIfSomeoneWon();
	if (game_ended != 0)
	{
		MessageToUsersGameEnded(p_game_params_var->user_communication_thread_params_array[game_ended - 1].user_name);
		return;
	}
	game_ended = checkIfBoardFull();
	if (game_ended != 0)
	{
		MessageToUsersGameEnded("Tie");
		return;
	}
	return;
}