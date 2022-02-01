#pragma once

#define MAX_LOOPS 3


typedef struct server_params_s server_params;
#ifndef server_params_s
struct server_params_s
{
	char *server_log_file_path;
	int server_port_num;
	unsigned long server_address;
	int num_of_connected_players;
	HANDLE mutex_var;
};
#endif

typedef struct server_thread_params_s user_communication_thread_params;
#ifndef server_thread_params_s
struct server_thread_params_s
{
	SOCKET PlayerSocket;
	int p_user_num;
	char* user_name;
	int is_ready_to_play;
};
#endif

/*
Description:
The servers main program
Parameters:
char *argv[]
Returns:
None
*/
void ServerRun(char *argv[]);

/*
Description:
Thread that handles communication with a client
Parameters:
user_communication_thread_params *thread_param)
Returns:
None
*/
static DWORD UserCommunicationInitializationThread(user_communication_thread_params *thread_param);

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
Thread that checks if the game has ended
Parameters:
None
Returns:
None
*/
void checkIfGameEndedThread();