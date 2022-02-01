#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "airplane_db.h"
#include "config.h"

const airplane_type airplane_type_array[NUM_OF_AIRPLANE_TYPES] = {
							{ "737",
						(char *[]) {"Larnaca", "Athens", "Budapest", "Zurich", "London", "Paris", "Rome", 0}
							},
							{ "747",
						(char *[]) {"London", "New York", "Bangkok", 0}
							},
							{ "787",
						(char *[]) {"London", "New York", "Los Angeles", "Hong Kong", "Miami", 0}
							}
						  };

int GetAirplaneType(airplane_type *p_airplane_type, char destination_name[])
{
	if (NULL == p_airplane_type || NULL == destination_name)
		return ERROR_NULL;
	for (int index = 0;
		index < len(airplane_type_array);
		index++)
	{
		if (AirplaneTypeToDestination(airplane_type_array[index], destination_name))
		{
			*p_airplane_type = airplane_type_array[index];
			return 0;
		}
	}
	return ERROR_NO_AIRPLANE_TYPE_TO_DESTINATION;
}

bool AirplaneTypeToDestination(airplane_type airplane_type_var, char destination_name[])
{
	if (NULL == destination_name)
		return false;
	if (StringInArray(airplane_type_var.p_p_destinations, destination_name))
	{
		return true;
	}
	return false;
}


int CreateAirplaneList(airplane **p_p_airplane_var)
{
	if (NULL == p_p_airplane_var)
		return ERROR_NULL;
	
	airplane *p_plane1= CreateAirplane( "Rehovot", sizeof("Rehovot"), "787", 0.5, 0 );
	airplane *p_plane2 = CreateAirplane( "Bat Yam", sizeof("Bat Yam"), "787", 1.5, p_plane1 );
	airplane *p_plane3 = CreateAirplane("Ashdod", sizeof("Ashdod"), "787", 1, p_plane2 );
	airplane *p_plane4 = CreateAirplane( "Jerusalem", sizeof("Jerusalem"), "747", 17, p_plane3 );
	airplane *p_plane5 = CreateAirplane( "Haifa", sizeof("Haifa"), "747", 15, p_plane4 );
	airplane *p_plane6 = CreateAirplane( "Tel-Aviv", sizeof("Tel-Aviv"), "747", 20, p_plane5 );
	airplane *p_plane7 = CreateAirplane( "Nahariya", sizeof("Nahariya"), "737", 1, p_plane6 );
	airplane *p_plane8 = CreateAirplane( "Kineret", sizeof("Kineret"), "737", 7.5, p_plane7 );
	airplane *p_plane9 = CreateAirplane( "Hadera", sizeof("Hadera"), "737", 3, p_plane8 );
	airplane *p_plane10 = CreateAirplane( "Ashkelon", sizeof("Ashkelon"), "737", 10.25, p_plane9 );
	airplane *p_plane11 = CreateAirplane( "Beit-Shean", sizeof("Beit-Shean"), "737", 5, p_plane10 );
	
	*p_p_airplane_var = p_plane11;
	return 0;
}
airplane *CreateAirplane(char *name, int name_size_in_bytes, char model[AIRPLANE_MODEL_LENGTH + 1/*null space*/], float age, airplane *p_next_airplane)
{
	airplane *p_plane = (airplane*)malloc(sizeof(airplane));
	if (NULL == p_plane)
	{
		printf("Fatal error: memory allocation failed!\n");
		exit(-1);
	}
	char *dynamic_name = (char*)malloc(name_size_in_bytes);
	strcpy_s(dynamic_name, name_size_in_bytes, name);
	p_plane->name = dynamic_name;
	p_plane->age = age;
	strcpy_s(p_plane->model, sizeof(char)*(AIRPLANE_MODEL_LENGTH + 1), model);
	p_plane->p_next_airplane = p_next_airplane;
	return p_plane;
}

int GetAirplane(char airplane_model[], airplane *p_first_airplane, airplane **p_p_youngest_airplane)
{
	if (NULL == p_first_airplane || NULL == p_p_youngest_airplane)
		return ERROR_NULL;
	float min_age = -1;
	airplane *p_selected_airplane = p_first_airplane;
	while (NULL != p_selected_airplane)
	{
		if (strcmp(airplane_model, (*p_selected_airplane).model) == 0)
		{
			if (min_age == -1 || (*p_selected_airplane).age < min_age)
			{
				min_age = (*p_selected_airplane).age;
				*p_p_youngest_airplane = p_selected_airplane;
			}
		}
		p_selected_airplane = (*p_selected_airplane).p_next_airplane;
	}
	if (min_age == -1)
		return ERROR_NO_AIRPLANE_WITH_SELECTED_MODEL_TYPE;
	return 0;
}

int DeleteAirplane(airplane **p_p_first_airplane, airplane *p_deleted_airplane)
{
	if (NULL == p_deleted_airplane || NULL == p_p_first_airplane)
		return ERROR_NULL;
	if (*p_p_first_airplane == p_deleted_airplane)
	{
		*p_p_first_airplane = (**p_p_first_airplane).p_next_airplane;
		return 0;
	}
	airplane *p_selected_airplane = *p_p_first_airplane;
	while (NULL != p_selected_airplane)
	{
		if ((*p_selected_airplane).p_next_airplane == p_deleted_airplane)
		{
			(*p_selected_airplane).p_next_airplane = (*(*p_selected_airplane).p_next_airplane).p_next_airplane;
			return 0;
		}
		p_selected_airplane = (*p_selected_airplane).p_next_airplane;
	}
	return ERROR_DELETED_AIRPLANE_ISNT_IN_AIRPLANE_LIST;
}

void ClearAirplaneList(airplane *p_first_airplane) 
{
	airplane *p_selected_airplane = p_first_airplane;
	while (p_selected_airplane != NULL)
	{
		airplane *p_freed_airplane = p_selected_airplane;
		p_selected_airplane = (*p_selected_airplane).p_next_airplane;
		FreeAirplane(p_freed_airplane);
	}
}

void FreeAirplane(airplane *p_plane)
{
	free(p_plane);
}