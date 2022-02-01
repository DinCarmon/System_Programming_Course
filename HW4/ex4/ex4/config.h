#pragma once

#define MIN_NUM_OF_PARAMETERS 3
#define MAX_NUM_OF_PARAMETERS 5
#define SERVER_NUM_OF_PARAMETERS 3
#define CLIENT_USER_MODE_NUM_OF_PARAMETERS 4
#define CLIENT_FILE_MODE_NUM_OF_PARAMETERS 5
#define USER_NAME_MAX_LENGTH 100

#define INVALID_NUM_OF_PARAMETER_ERROR_MESSAGE "invalid num of parameters"
#define PROGRAM_FINISHED_SUCCESFULLY_MESSAGE "program finished successfully"
#define GAME_IS_OCCUPIED_MESSAGE "NEW_USER_DECLINED"
#define WELCOME_MESSAGE "Welcome to this server!\nEnter username: "
#define SERVER_ADDRESS_STR "127.0.0.1"

#define NUM_OF_PLAYERS 2

#define ERROR_CODE_WRONG_NUM_OF_PARAM -1
#define ERROR_CODE_INVALID_MODE -2
#define ERROR_CODE_INVALID_SERVER_ADDRESS_STR -3
#define ERROR_CODE_SOCKET -4
#define INVALID_GAME_PLAY -5
#define ERROR_NULL -6
#define ERROR_OPENING_FILE -7
#define NO_PARAMS_MESSAGE -8
#define MUTEX_OR_SEMAPHORE_ERROR -11
#define ERROR_WAIT_OBJECT -12
#define ERROR_ILLEGAL_COMMAND -13
#define ERROR_GAME_HAS_NOT_STARTED -14
#define PLAYER_DISCONNECTED -15


#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )
#define StringStartsWith( Prefix, Str) (strncmp(Prefix, Str, strlen(Prefix)) == 0)

/*
Description:
Checks if program parameters are valid
Parameters:
int argc, char *argv[]
Returns:
None
*/
void ValidParameters(int argc, char *argv[]);