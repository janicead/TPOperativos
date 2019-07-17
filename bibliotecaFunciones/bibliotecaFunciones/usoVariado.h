#ifndef BIBLIOTECAFUNCIONES_USOVARIADO_H_
#define BIBLIOTECAFUNCIONES_USOVARIADO_H_
#include <stdio.h>
#include "consola.h"
#include <inttypes.h>

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
void printearArrayDeInts(int arrayDeInts[150]);
/* NO CUENTA EL ELEMENTO = NULL */
int longitudArrayDePunteros(char **unArrayDePunteros);
void hacerFreeArray(void**);
int tamanioArray(void** array);
long int cantLugaresEnArchivo(FILE* archivo);
char* int_to_string(int numero);
unsigned long int obtenerTimeStamp();
int buscarFinalValue(char** value);
char* armarValue(char** value);
char * quitarComillas(char* ip);
int pasarUINT16AInt(uint16_t nro);

#endif /* BIBLIOTECAFUNCIONES_USOVARIADO_H_ */
