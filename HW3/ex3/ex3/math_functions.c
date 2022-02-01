#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"
#include "math_functions.h"

primitive_pitagoras_threesome PrimitiveThreesomeCreator(int m, int n)
{
	primitive_pitagoras_threesome primitive_pitagoras_threesome_var;
	primitive_pitagoras_threesome_var.m = m;
	primitive_pitagoras_threesome_var.n = n;
	primitive_pitagoras_threesome_var.a = m * m - n * n;
	primitive_pitagoras_threesome_var.b = 2 * m * n;
	primitive_pitagoras_threesome_var.c = m * m + n * n;
	primitive_pitagoras_threesome_var.p_next_primitive_pitagoras_threesome = 0;
	return primitive_pitagoras_threesome_var;
}

int GreatestCommonDivider(int x, int y)
{
	
	int gcd;
	for (int i = 1; i <= x && i <= y; ++i)
	{
		// Checks if i is factor of both integers
		if (x%i == 0 && y%i == 0)
			gcd = i;
	}
	return gcd;
	
	//if (0 != y)
	//	return GreatestCommonDivider(y, x%y);
	//return x;
}

void FreeThreesome(primitive_pitagoras_threesome *p_first_primitive_pitagoras_threesome)
{
	if (NULL == p_first_primitive_pitagoras_threesome)
		return;

	primitive_pitagoras_threesome *p_primitive_pitagoras_threesome_var = p_first_primitive_pitagoras_threesome;
	primitive_pitagoras_threesome *p_primitive_pitagoras_threesome_next_var = p_first_primitive_pitagoras_threesome->p_next_primitive_pitagoras_threesome;
	while (p_primitive_pitagoras_threesome_var != NULL)
	{
		free(p_primitive_pitagoras_threesome_var);
		if (p_primitive_pitagoras_threesome_next_var == NULL)
			break;
		p_primitive_pitagoras_threesome_var = p_primitive_pitagoras_threesome_next_var;
		p_primitive_pitagoras_threesome_next_var = p_primitive_pitagoras_threesome_next_var->p_next_primitive_pitagoras_threesome;
	}
}