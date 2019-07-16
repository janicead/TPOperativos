#ifndef CONSOLAKERNEL_H_
#define CONSOLAKERNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include "planificador.h"

void* setConsole();
char** obtenerParametros(char* linea, int indice);
void freeParametros(char** parametros);


//FUNCIONES QUE SE EJECUTAN SEGUN EL COMANDO QUE SE INGRESE EN LA CONSOLA
void crear_lql_select(char** parametros);
void crear_lql_insert(char** parametros);
void crear_lql_create(char** parametros);
void crear_lql_describe(char** parametros);
void crear_lql_drop(char** parametros);
void crear_lql_journal();
void crear_lql_add(char* criterio, int nro_memoria);
void crear_lql_run(char* path);
void crear_lql_metrics();


#endif /* CONSOLAKERNEL_H_ */
