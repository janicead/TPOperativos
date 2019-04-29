#ifndef CONSOLALFS_H_
#define CONSOLALFS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include "LFS_commons.h"

void setConsole();
char** obtenerParametros(char* linea, int indice);
void freeParametros(char** parametros);

#endif /* CONSOLALFS_H_ */
