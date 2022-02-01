#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"
#include "math_functions.h"

/*
Description:
The function receives a pointer to the first primitive pitagoras threesome and a txt file output path
and returns the threesomes printed in the output txt file.
Parameters:
primitive_pitagoras_threesome *p_first_primitive_pitagoras_threesome
char *output_path
Returns:
Error codes ERROR_NULL or ERROR_OPENING_FILE if function failed, 0 if succeeded
*/
int WriteToOutput(primitive_pitagoras_threesome *p_first_primitive_pitagoras_threesome, char *output_path);