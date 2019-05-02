#ifndef CONSOLAMEMORIA_H_
#define CONSOLAMEMORIA_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include "memoria_commons.h"
#include <commons/string.h>

void setConsole();
char** obtenerParametros(char* linea, int indice);
void freeParametros(char** parametros);

#endif /* CONSOLAMEMORIA_H_ */
