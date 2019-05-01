#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "configKernel.h"
#include "kernel_commons.h"
#include "parser.h"
#include <commons/collections/list.h>

FILE* abrirArchivo(char* path);
void parsear(FILE* archivo);

#endif /* PLANIFICADOR_H_ */
