#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include "config.h"

typedef struct primitive_pitagoras_threesome_s primitive_pitagoras_threesome;

#ifndef primitive_pitagoras_threesome_s
struct primitive_pitagoras_threesome_s
{
	int m;
	int n;
	int a;
	int b;
	int c;
	primitive_pitagoras_threesome *p_next_primitive_pitagoras_threesome;
};
#endif

/*
Description:
The function calculates the primitive threesome and returns it.
Parameters:
int m
int n
Returns:
primitive_pitagoras_threesome var
*/
primitive_pitagoras_threesome PrimitiveThreesomeCreator(int m, int n);

/*
Description:
The function calculates and returns the gcd of x and y
Parameters:
int x
int y
Returns:
gcd(x, y)
*/
int GreatestCommonDivider(int x, int y);

/*
Description:
The function free the freesomes.
Parameters:
primitive_pitagoras_threesome *p_first_primitive_pitagoras_threesome
*/
void FreeThreesome(primitive_pitagoras_threesome *p_first_primitive_pitagoras_threesome);