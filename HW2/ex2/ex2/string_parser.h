#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include "config.h"

/*
Description:
	The function receives str of an exe file, and adds escapecharacter for it to be accepted correctly by the compiler.
Parameters:
	char* str (input)
Returns:
	a "corrected" string
*/
char *AddEscapeCharacter(char* str);