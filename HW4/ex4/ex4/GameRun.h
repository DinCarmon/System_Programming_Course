#pragma once
#include <stdio.h>
#include <Windows.h>
#include "config.h"
#include "server.h"

#define RED_PLAYER 1
#define YELLOW_PLAYER 2
#define EMPTY 0
#define TIE 3

#define BOARD_HEIGHT 6
#define BOARD_WIDTH  7

#define BLACK  15
#define RED    204
#define YELLOW 238

typedef struct game_params_s game_params;
#ifndef game_params_s
struct game_params_s
{
	server_params server_params_var;
	user_communication_thread_params* user_communication_thread_params_array;
	int game_ended;
};
#endif

typedef struct send_recv_queuer_s send_recv_queuer;
#ifndef send_recv_queuer_s
struct send_recv_queuer_s
{
	send_recv_queuer* p_next_send_recv_queuer;
	char* message;
	int* p_user_num;
	send_recv_queuer* p_last_send_recv_queuer;
};
#endif

typedef struct send_recv_queue_s send_recv_queue;
#ifndef send_recv_queue_s
struct send_recv_queue_s
{
	send_recv_queuer* p_first_send_recv_queuer;
	send_recv_queuer* p_end_send_recv_queuer;
	
};
#endif

/*
Description:
A thread that handles the game execution
Parameters:
game_params* game_params_var_p
Returns:
0 if no errors occured, error code otherwise
*/
DWORD  GameRunning(game_params* game_params_var_p);

/*
Description:
Enters messages from clients to a queue
Parameters:
user_communication_thread_params user_communication_param_var
Returns:
None
*/
void ReceiveFromUserThread(user_communication_thread_params user_communication_param_var);

/*
Description:
Send messages from the sending messages queue to clients 
Parameters:
user_communication_thread_params user_communication_param_var
Returns:
None
*/
void SendToUsersThread(user_communication_thread_params user_communication_param_var);

/*
Description:
Handle messages from the receive messages queue
Parameters:
int* p_user_num
Returns:
Next message in queue
*/
char* ReceiveNextCommandFromUsers(int* p_user_num);

/*
Description:
Enter messages to send messages queue
Parameters:
char* message, int user_index
Returns:
None
*/
void SendToUser(char* message, int user_index);

/*
Description:
Checks if one of the clients won, calls the next 3 functions
Parameters:
None
Returns:
1 if true
*/
int checkIfSomeoneWon();

/*
Description:
Checks if one of the clients won horizontally
Parameters:
None
Returns:
1 if true
*/
int checkIfSomeoneWonHorizontally();

/*
Description:
Checks if one of the clients won diagonally
Parameters:
None
Returns:
1 if true
*/
int checkIfSomeoneWonDiagonally();

/*
Description:
Checks if one of the clients won vertically
Parameters:
None
Returns:
1 if true
*/
int checkIfSomeoneWonVertically();

/*
Description:
play next move
Parameters:
int col, int p_user_num
Returns:
0 if no error, error code otherwise
*/
int playMove(int col, int p_user_num);

/*
Description:
Checks if board is full
Parameters:
None
Returns:
Number of the latest filled place
*/
int checkIfBoardFull();

/*
Description:
Board parser
Parameters:
None
Returns:
String representation of the board
*/
char* BoardParse();

/*
Description:
Send message to users that the game is starting
Parameters:
None
Returns:
None
*/
void MessageToUsersGameStarted();

/*
Description:
Sends the status of the board to clients
Parameters:
None
Returns:
None
*/
void MessageToUsersBoardView();

/*
Description:
Send a turn switch message to clients
Parameters:
game_params* p_game_params_var
Returns:
None
*/
void MessageToUsersTurnSwitch(game_params* p_game_params_var);

/*
Description:
Notify clients that the game has ended
Parameters:
char* score_result
Returns:
Game result
*/
void MessageToUsersGameEnded(char* score_result);

/*
Description:
Checks if the game has ended
Parameters:
game_params* p_game_params_var
Returns:
None
*/
void CheckIfGameEnded(game_params* p_game_params_var);