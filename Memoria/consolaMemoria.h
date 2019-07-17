#ifndef CONSOLAMEMORIA_H_
#define CONSOLAMEMORIA_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include "configMemoria.h"
#include "segmentacionPaginada.h"
#include <bibliotecaFunciones/usoVariado.h>

void* crearConsolaMemoria();
bool pasarAUint16(const char *str, uint16_t *res);
int buscarFinalValue(char** value);
char* quitarEspacioFalsoMemoria(char* value);

#endif /* CONSOLAMEMORIA_H_ */
