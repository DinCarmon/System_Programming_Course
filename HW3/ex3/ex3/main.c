/*
Authors:
Din Carmon, 209325026
Yonatan Rimon, 208541078
*/

/*
Project:
ex3
*/

/*
Description:

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"
#include "math_functions.h" 
#include "file_parser.h"

static primitive_pitagoras_threesome *p_first_primitive_pitagoras_threesome;

int main(int argc, char *argv[])
{
	#pragma region CheckForValidNumOfArguments
		int valid_num_of_arguments = ValidNumOfProgramParameters(argc);
		if (valid_num_of_arguments != 0)
			return valid_num_of_arguments;
	#pragma endregion

	#pragma region ExtractParameters
		int max_number_of_m = atoi(argv[1]);
		int num_of_computation_threads = atoi(argv[2]);
		int output_buffer_size = atoi(argv[3]);
		char *output_path = argv[4];
	#pragma endregion

	#pragma region FindAndOrderPrimitivePitagorasThreesomes
	FindAndOrderPrimitivePitagorasThreesomes(&p_first_primitive_pitagoras_threesome, max_number_of_m,
			num_of_computation_threads,
			output_buffer_size);
	#pragma endregion

	#pragma region WriteToOutput
		int write_err = WriteToOutput(p_first_primitive_pitagoras_threesome, output_path);
	#pragma endregion

	#pragma region FreeListOfThreesomes
		FreeThreesome(p_first_primitive_pitagoras_threesome);
	#pragma endregion
		
	return 0;
}