#include "varios.h"


void freeArrayDePunteros(char **unArrayDePunteros)
{
	int i;

	for(i=0; unArrayDePunteros[i] != NULL ;i++)
	{
		free(unArrayDePunteros[i]);
	}

	free(unArrayDePunteros);
}

int longitudArrayDePunteros(char **unArrayDePunteros)
{
	int i;

	for(i=0; unArrayDePunteros[i] != NULL ;i++);

	return i;
}
