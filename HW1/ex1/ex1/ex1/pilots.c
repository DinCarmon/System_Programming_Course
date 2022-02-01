#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "pilots.h"
#include "config.h"
#pragma warning(disable : 4996)


int GetPilots(char *p_input_path, pilot **p_p_first_pilot)
{
	if (NULL == p_input_path || NULL == p_p_first_pilot)
		return ERROR_NULL;
	FILE *p_file;
	errno_t err;
	if (err = fopen_s(&p_file, p_input_path, "r"))
		return -1;
	char row_of_pilot_file[MAX_LENGTH_OF_PILOT_ROW_IN_FILE];
	bool first_enter_to_loop = true;
	pilot *p_last_pilot = 0;
	while (fgets(row_of_pilot_file, MAX_LENGTH_OF_PILOT_ROW_IN_FILE, p_file) != NULL)
	{
		if (row_of_pilot_file[strlen(row_of_pilot_file) - 1] == '\n')
		{
			row_of_pilot_file[strlen(row_of_pilot_file) - 1] = NULL;
		}
		pilot *p_pilot;
		CreateNewPilot(&p_pilot ,row_of_pilot_file);
		if (first_enter_to_loop)
		{
			*p_p_first_pilot = p_pilot;
			p_last_pilot = p_pilot;
		}
		else
		{
			p_last_pilot->p_next_pilot = p_pilot;
			p_last_pilot = p_pilot;
		}
		first_enter_to_loop = false;
	}
	fclose(p_file);
	return 0;
}

int CreateNewPilot(pilot **p_p_pilot ,char *line)
{
	if (NULL == line)
		return ERROR_NULL;
	pilot *p_pilot = (pilot*)malloc(sizeof(pilot));
	*p_p_pilot = p_pilot;
	if (NULL == p_pilot)
	{
		printf("Fatal error: memory allocation failed!\n");
		exit(-1);
	}
	char *token = strtok(line, FILE_PILOT_SEPARATOR);
	int count = 0;
	while (token != NULL)
	{
		if (count == 0)
		{
			p_pilot->name = (char*)malloc(strlen(token));
			strcpy(p_pilot->name, token);
		}
		if (count == 1)
			strcpy((*p_pilot).model, token + 1/*space after ,*/);
		if (count == 2)
			(*p_pilot).num_of_flight_hours = atof(token + 1/*space after ,*/);
		if (count == 3)
		{
			p_pilot->rank = (char*)malloc(strlen(token+1));
			strcpy(p_pilot->rank , token + 1/*space after ,*/);
			if (StringInArray((char*[]){ ALLOWED_PILOT_RANKS }, p_pilot->rank) == false)
			{
				printf("Invalid rank for pilot");
				exit(-1);
			}
		}
		token = strtok(NULL, FILE_PILOT_SEPARATOR);
		count++;
	}
	p_pilot->p_next_pilot = 0;
	return 0;
}

void DeletePilots(pilot **p_p_first_pilot, pilot *p_deleted_pilot)
{
	if (NULL == p_deleted_pilot || NULL == p_p_first_pilot)
		return ERROR_NULL;
	if (*p_p_first_pilot == p_deleted_pilot)
	{
		*p_p_first_pilot = (**p_p_first_pilot).p_next_pilot;
	}
	else
	{
		pilot *p_selected_pilot = *p_p_first_pilot;
		while (NULL != p_selected_pilot)
		{
			if ((*p_selected_pilot).p_next_pilot == p_deleted_pilot)
			{
				(*p_selected_pilot).p_next_pilot = (*(*p_selected_pilot).p_next_pilot).p_next_pilot;
				return;
			}
			p_selected_pilot = (*p_selected_pilot).p_next_pilot;
		}
	}
}

void ClearPilotList(pilot *p_first_pilot)
{
	pilot *p_selected_pilot = p_first_pilot;
	while (p_selected_pilot != NULL)
	{
		pilot *p_freed_pilot = p_selected_pilot;
		p_selected_pilot = (*p_selected_pilot).p_next_pilot;
		FreePilot(p_freed_pilot);
	}
}

void FreePilot(pilot* p_pilot)
{
	free(p_pilot);
}