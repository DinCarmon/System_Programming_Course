#include <stdbool.h>
#include <stdio.h>
#include "config.h"

typedef struct airplane_type_s airplane_type;
typedef struct airplane_s airplane;
extern const airplane_type airplane_type_array[3];

struct airplane_type_s
{
	char model[AIRPLANE_MODEL_LENGTH + 1/*null space*/];
	char **p_p_destinations;
};
struct airplane_s
{
	char *name;
	char model[AIRPLANE_MODEL_LENGTH + 1/*null space*/];
	float age; //TODO: how to make it only positive?
	airplane *p_next_airplane;
};

/*
Description:
	The function receives an airplane_type pointer and updates its value to
    an airplane_type which has the destination in its p_p_destinations.
Parameters: 
	char destination_name[] (input)
	airplane_type * p_airplane_type (output)
Returns:
	0 (succeded)
	ERROR_NO_AIRPLANE_TYPE_TO_DESTINATION (unsucceded)
*/
extern int GetAirplaneType(airplane_type *p_airplane_type, char destination_name[]);

/*
Description:
	The function receives an airplane_type, and checks whether it has the specified destination.
Parameters:
	airplane_type airplane_type_var (input)
	char destination_name[] (input)
Returns:
	True/ False
*/
extern bool AirplaneTypeToDestination(airplane_type airplane_type_var, char destination_name[]);

/*
Description:
	The function creates a nested list of airplanes according to Ex1_section4
Parameters:
	airplane ** p_p_airplane_var (input)
Returns:
	0 (succeded)
	ERROR_CREATE_AIRPLANE_LIST (unsucceded)
*/
extern int CreateAirplaneList(airplane **p_p_airplane_var);

/*
Description:
The function creates a new Airplane object
Parameters:
char *name, int name_size_in_bytes, char model[AIRPLANE_MODEL_LENGTH + 1], float age, airplane *p_next_airplane
Returns:
airplane *p_plane
*/
extern airplane *CreateAirplane(char *name, int name_size_in_bytes, char model[AIRPLANE_MODEL_LENGTH + 1/*null space*/], float age, airplane *p_next_airplane);

/*
Description:
The function returns a pointer to an Airplane according to Ex1_section5
Parameters:
char airplane_model[], airplane *p_first_airplane, airplane **p_p_youngest_airplane
Returns:
0 (succeded)
ERROR_NO_AIRPLANE_WITH_SELECTED_MODEL_TYPE (unsucceded)
*/
extern int GetAirplane(char airplane_model[], airplane *p_first_airplane, airplane **p_p_youngest_airplane);

/*
Description:
The function deletes an Airplane according to Ex1_section6
Parameters:
airplane **p_p_first_airplane, airplane *p_deleted_airplane
Returns:
0 (succeded)
ERROR_DELETED_AIRPLANE_ISNT_IN_AIRPLANE_LIST (unsucceded)
*/
extern int DeleteAirplane(airplane **p_p_first_airplane, airplane *p_deleted_airplane);

/*
Description:
The function clears dynamic memory space of an Airplane list according to Ex1_section7
Parameters:
airplane *p_first_airplane
Returns:
void
*/
extern void ClearAirplaneList(airplane *p_first_airplane);

/*
Description:
The function free's dynamic memory space
Parameters:
airplane *p_plane
Returns:
void
*/
extern void FreeAirplane(airplane *p_plane);
