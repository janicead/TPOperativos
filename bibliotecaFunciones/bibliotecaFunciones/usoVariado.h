#ifndef BIBLIOTECAFUNCIONES_USOVARIADO_H_
#define BIBLIOTECAFUNCIONES_USOVARIADO_H_
#include <stdio.h>
#include "consola.h"

typedef struct{
	char* request;
	int quantum;
	long int dondeSeQuedo;
	FILE *archivo;
	int esPrimeraVez;
}t_request;

/*
 * Definicion de registro
 * */

typedef struct{
	unsigned long int timestamp;
	u_int16_t key;
	char* value;
}t_registro;

/*
 * Definicion de Metadata_tabla
 * */

typedef struct{
	char* nombre;
	char* tipoConsistencia;
	int numeroParticiones;
	int compactionTime;
}t_metadata_tabla;

int pasarMilisegundosASegundos(int milisegundos);
void printearArrayDeChars(char** arrayDeChars);
void printearArrayDeInts(int* arrayDeInts);
/* NO CUENTA EL ELEMENTO = NULL */
int longitudArrayDePunteros(char **unArrayDePunteros);
void hacerFreeArray(void**);
int tamanioArray(void** array);
long int cantLugaresEnArchivo(FILE* archivo);


#endif /* BIBLIOTECAFUNCIONES_USOVARIADO_H_ */
