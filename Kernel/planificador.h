#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "configKernel.h"
#include "kernel_commons.h"
#include "parser.h"
#include <commons/collections/list.h>
#include <stdbool.h>

FILE* abrirArchivo(char* path);
void parsear(FILE* archivo);
t_tabla* devuelve_tabla(char* nombre);
void lql_select(t_LQL_operacion* operacion);
void free_tabla(t_tabla* tabla);
t_memoria* obtener_memoria(char* consistencia);

#endif /* PLANIFICADOR_H_ */
