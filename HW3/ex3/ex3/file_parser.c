#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"
#include "math_functions.h"

int WriteToOutput(primitive_pitagoras_threesome *p_first_primitive_pitagoras_threesome, char *output_path)
{
	if (NULL == p_first_primitive_pitagoras_threesome || NULL == output_path)
		return ERROR_NULL;
	FILE *p_output_file = 0;
	errno_t err;
	if (err = fopen_s(&p_output_file, output_path, "w"))
		return ERROR_OPENING_FILE;

	primitive_pitagoras_threesome *p_primitive_pitagoras_threesome_var = p_first_primitive_pitagoras_threesome;
	while (p_primitive_pitagoras_threesome_var != NULL)
	{
		fprintf(p_output_file, "%d,%d,%d\n", p_primitive_pitagoras_threesome_var->a,
											p_primitive_pitagoras_threesome_var->b,
											p_primitive_pitagoras_threesome_var->c);
		p_primitive_pitagoras_threesome_var = p_primitive_pitagoras_threesome_var->p_next_primitive_pitagoras_threesome;
	}

	fclose(p_output_file);

	return 0;
}