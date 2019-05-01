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

void setConsole();
char** obtenerParametros(char* linea, int indice);
void freeParametros(char** parametros);


#endif /* CONSOLAKERNEL_H_ */
