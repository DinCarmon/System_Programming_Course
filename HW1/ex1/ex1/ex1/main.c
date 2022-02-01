/*
Authors:
Din Carmon, 000000000
Yonatan Rimon, 000000000
*/

/*
Project:
ex1
*/

/*
Description:
A system of flight managing.
The system receieves as an input the flight destinations (file path), and the pilots information (file path),
and manage the pilots, and airplanes accordingly (output to a plan file path).
*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "airplane_db.h"
#include "pilots.h"

/*
Description:
	The function receieves the airplane and pilot list
	and creates a plan of assignments for each destination
	(of a received destination file path)
	of pilots and airplanes.
	The assignment is according to the laws in the ex1_assignment
Parameters:
	airplane **p_p_first_airplane (output. deletes used airplanes from the list)
	pilot **p_p_first_pilot (output. deletes used pilots from the list)
	char destinations_path[]
		(input. a file with the destinations seperated in new line.
		same place can be twice. a place may have no airplane to it)
	char plan_path[]
		(output. where to write the final plan.
		example of a written line: Larnaca, Nahariya, Avi Ron, Dani Matos.
		if a plan cant be writtem to all destinations writes:
		"An error occurred during execution, couldn’t complete the task!\n")
Returns:
	0 (succeded)
	-1 (failed)
*/
int CreatePlan(airplane* p_first_airplane, pilot *p_first_pilot, char destinations_path[], char plan_path[]);

/*
Description:
	The function receieves the airplane and pilot remainning list
	and creates aî assignment for receieved destination
	of pilots and airplane.
	The assignment is according to the laws in the ex1_assignment
Parameters:
	airplane **p_p_first_airplane (output. deletes used airplane from the list)
	pilot **p_p_first_pilot (output. deletes used pilots from the list)
	char *destination (input)
	bool *p_problem_planning
		(output. wheteher a problem of any kind occored during plan)
Returns:
	the assignment. example: Larnaca, Nahariya, Avi Ron, Dani Matos
	NULL if did not succeed.
*/
char *PlanForNextDestination(airplane **p_p_first_airplane, pilot **p_p_first_pilot, char *destination, bool *p_problem_planning);

/*
Description:
	The function receieves the airplane remainning list
	and creates aî assignment for receieved destination
	of an airplane which gets there.
Parameters:
	airplane **p_p_first_airplane (output. deletes used airplane from the list)
	char *destination (input)
	bool *p_problem_planning
		(output. wheteher a problem of any kind occored during operation)
Returns:
	the airplane.
	NULL if did not succeed.
*/
airplane *FindYoungestPlane(airplane **p_p_first_airplane, char *destination, bool *p_problem_planning);

/*
Description:
	The function receieves the pilot remainning list
	and creates aî assignment for a receieved model airplane
	of a pilot of the required rank which is the most unexperienced and qualified ti this model.
Parameters:
	pilot **p_p_first_pilot (output. deletes used pilot from the list)
	char *rank (input)
	char* model (input)
	bool *p_problem_planning
		(output. wheteher a problem of any kind occored during operation)
Returns:
	the pilot.
	NULL if did not succeed.
*/
pilot *FindMostUnExperiencedPilot(pilot **p_p_first_pilot, char *rank, char* model, bool *p_problem_planning);

int main(int argc, char *argv[])
{
	
	if ( argc < PROGRAM_NUM_OF_PARAMETERS+1)//project name
	{
	return ERROR_CODE_NOT_ENOUGH_ARGUMENTS;
	}
	if ( argc > PROGRAM_NUM_OF_PARAMETERS + 1)//project name
	{
		return ERROR_CODE_TOO_MANY_ARGUMENTS;
	}

	char *pilots_path = argv[1];

	char *destinations_path = argv[2];

	char *plan_path = argv[3];

	airplane *p_first_airplane;
	CreateAirplaneList(&p_first_airplane);
	pilot *p_first_pilot;
	GetPilots(pilots_path, &p_first_pilot);

	int succeded = CreatePlan(p_first_airplane, p_first_pilot, destinations_path, plan_path);

	ClearAirplaneList(p_first_airplane);
	ClearPilotList(p_first_pilot);
	return succeded;
}

int CreatePlan(airplane* p_first_airplane, pilot *p_first_pilot, char destinations_path[], char plan_path[])
{
	FILE *p_destinations_file;
	errno_t err;
	if (err = fopen_s(&p_destinations_file, destinations_path, "r"))
		return -1;
	FILE *p_plan_file;
	errno_t err2;
	if (err2 = fopen_s(&p_plan_file, plan_path, "w"))
		return -1;
	char destination[MAX_LENGTH_OF_DESTINATION_ROW_IN_FILE];
	bool problem_planning = false;
	while (fgets(destination, MAX_LENGTH_OF_DESTINATION_ROW_IN_FILE, p_destinations_file) != NULL)
	{
		if (destination[strlen(destination) - 1] == '\n')
		{
			destination[strlen(destination) - 1] = NULL;
		}
		char *plan_next_row = PlanForNextDestination(&p_first_airplane, &p_first_pilot, destination, &problem_planning);
		if (problem_planning)
			break;
		fprintf(p_plan_file, plan_next_row);
	}
	fclose(p_destinations_file);
	fclose(p_plan_file);
	if (problem_planning == true)
	{
		if (err2 = fopen_s(&p_plan_file, plan_path, "w"))
			return -1;
		fprintf(p_plan_file, "An error occurred during execution, couldn’t complete the task!\n");
	}
	return 0;
}

char *PlanForNextDestination(airplane **p_p_first_airplane, pilot **p_p_first_pilot, char *destination, bool *p_problem_planning)
{
	airplane *p_plane = FindYoungestPlane(p_p_first_airplane, destination, p_problem_planning);
	if (*p_problem_planning == true)
	{
		return NULL;
	}
	pilot *p_pilot_Captain = FindMostUnExperiencedPilot(p_p_first_pilot, "Captain", p_plane->model, p_problem_planning);
	pilot *p_pilot_FirstOfficer = FindMostUnExperiencedPilot(p_p_first_pilot, "First Officer", p_plane->model, p_problem_planning);
	if (*p_problem_planning == true)
	{
		return NULL;
	}
	char *plan_next_row = (char*)malloc(strlen(destination) + 1 + strlen(p_plane->name) + 1 + strlen(p_pilot_Captain->name) + 1 + strlen(p_pilot_FirstOfficer->name + 2));
	const char token [3]= ", ";
	*plan_next_row = 0;

	snprintf(plan_next_row, 
			strlen(destination) + 1 + strlen(p_plane->name) + 1 + strlen(p_pilot_Captain->name) + 1 + strlen(p_pilot_FirstOfficer->name) + 10,
				"%s%s%s%s%s%s%s%s", destination, token, p_plane->name, token, p_pilot_Captain->name, token, p_pilot_FirstOfficer->name, (char*)"\n\0");

	return plan_next_row;
}

airplane *FindYoungestPlane(airplane **p_p_first_airplane, char *destination, bool *p_problem_planning)
{
	airplane *p_selected_airplane = 0;
	int min_age = -1;
	for (int index = 0; index < NUM_OF_AIRPLANE_TYPES; index++)
	{
		airplane *p_model_youngest_airplane = 0;
		int model_plane_free = GetAirplane(airplane_type_array[index].model, *p_p_first_airplane, &p_model_youngest_airplane);
		if (model_plane_free == 0 && AirplaneTypeToDestination(airplane_type_array[index], destination) == true)
		{
			if (min_age == -1 || min_age > p_model_youngest_airplane->age)
			{
				p_selected_airplane = p_model_youngest_airplane;
				min_age = p_selected_airplane->age;
			}
		}
	}
	if (min_age == -1)
	{
		*p_problem_planning = true;
		return NULL;
	}
	DeleteAirplane(p_p_first_airplane, p_selected_airplane);
	return p_selected_airplane;
}

pilot *FindMostUnExperiencedPilot(pilot **p_p_first_pilot, char *rank, char* model, bool *p_problem_planning)
{
	pilot *p_most_experienced_pilot = 0;
	if (NULL == p_p_first_pilot)
	{
		*p_problem_planning = true;
		return NULL;
	}
	float min_num_of_flight_hours = -1;
	pilot *p_selected_pilot = *p_p_first_pilot;
	while (NULL != p_selected_pilot)
	{
		if (strcmp(model, (*p_selected_pilot).model) == 0 && strcmp(rank ,p_selected_pilot->rank)==0)
		{
			if (min_num_of_flight_hours==-1 || (*p_selected_pilot).num_of_flight_hours < min_num_of_flight_hours)
			{
				min_num_of_flight_hours = (*p_selected_pilot).num_of_flight_hours;
				p_most_experienced_pilot = p_selected_pilot;
			}
		}
		p_selected_pilot = (*p_selected_pilot).p_next_pilot;
	}
	if (min_num_of_flight_hours == -1)
	{
		*p_problem_planning = true;
		return NULL;
	}
	DeletePilots(p_p_first_pilot, p_most_experienced_pilot);
	return p_most_experienced_pilot;
}

