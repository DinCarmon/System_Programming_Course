#pragma once
#include "config.h"

typedef struct pilot_s pilot;

struct pilot_s
{
	char *name;
	char model[AIRPLANE_MODEL_LENGTH + 1/*null space*/];
	unsigned int num_of_flight_hours;
	char *rank;
	pilot *p_next_pilot;
};

/*
Description:
The function returns a pointer to a pilot according to Ex1_section9
Parameters:
char *p_input_path, pilot **p_p_first_pilot
Returns:
0 (succeded)
*/
extern int GetPilots(char *p_input_path, pilot **p_p_first_pilot);

/*
Description:
The function create's a new Pilot object when being called from 'GetPilot'
Parameters:
pilot **p_p_pilot, char *line
Returns:
0 (succeded)
*/
extern int CreateNewPilot(pilot **p_p_pilot, char *line);

/*
Description:
The function deletes a Pilot according to Ex1_section10
Parameters:
pilot **p_p_first_pilot, pilot *p_deleted_pilot
Returns:
0 (succeded)
*/
extern void DeletePilots(pilot **p_p_first_pilot, pilot *p_deleted_pilot);

/*
Description:
The function clears dynamic memory space of a Pilot list according to Ex1_section11
Parameters:
pilot *p_first_pilot
Returns:
void
*/
extern void ClearPilotList(pilot *p_first_pilot);

/*
Description:
The function free's dynamic memory space
Parameters:
pilot* p_pilot
Returns:
void
*/
extern void FreePilot(pilot* p_pilot);