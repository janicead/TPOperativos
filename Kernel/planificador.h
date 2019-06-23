#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "configKernel.h"
#include "kernel_commons.h"
#include "parser.h"
#include <commons/collections/list.h>
#include <stdbool.h>


void* ejecutar();

//FUNCIONES SOBRE LAS MEMORIAS
t_memoria* obtener_memoria_consistencia(char* consistencia,int key);
t_memoria* obtener_memoria_por_id(int id);
bool memoria_existente(t_list* l_memorias,int id);
t_memoria* hash_memory(int key);
t_memoria* random_memory();

//FUNCIONES DE COMANDOS
void lql_select(t_LQL_operacion* operacion);
void lql_insert(t_LQL_operacion* op);
void lql_create(t_LQL_operacion* op);
void lql_describe(t_LQL_operacion* op);
void lql_drop(t_LQL_operacion* op);
void lql_journal(t_list* lista_mem);
void lql_add(t_LQL_operacion* op);
void lql_run(FILE* archivo);
void lql_metrics();
void* timer();

//FUNCIONES EXTRA
FILE* abrirArchivo(char* path);
t_tabla* devuelve_tabla(char* nombre);

#endif /* PLANIFICADOR_H_ */
