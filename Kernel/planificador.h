#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "configKernel.h"
#include "kernel_commons.h"
#include "parser.h"
#include <commons/collections/list.h>
#include <stdbool.h>
#include "consolaKernel.h"
#include <bibliotecaFunciones/lfsProtocolos.h>


void* ejecutar();

//FUNCIONES SOBRE LAS MEMORIAS
t_memoria* obtener_memoria_consistencia(char* consistencia,int key);
t_memoria* obtener_memoria_por_id(int id);
bool memoria_existente(t_list* l_memorias,int id);
t_memoria* hash_memory(int key);
t_memoria* random_memory(t_list* lista);

//FUNCIONES DE COMANDOS
void lql_select(t_LQL_operacion* operacion);
void lql_insert(t_LQL_operacion* op);
void lql_create(t_LQL_operacion* op);
void lql_describe(t_LQL_operacion* op);
void lql_drop(t_LQL_operacion* op);
void lql_journal(t_list* list_mem, t_LQL_operacion* op);
void lql_add(t_LQL_operacion* op);
void lql_run(FILE* archivo, t_LQL_operacion* op);
void lql_metrics();
void* refresh_metadata_timer();

//FUNCIONES PARA LAS MÉTRICAS
void* metrics_timer();
double tiempoPromedioSelect();
double tiempoPromedioInsert();
int cantidadSelects();
int cantidadInserts();
int porcentajeSelectsInserts(int cant_selects_inserts_ejecutados);
void memoryLoad();

//FUNCIONES EXTRA
FILE* abrirArchivo(char* path);
t_tabla* devuelve_tabla(char* nombre);

#endif /* PLANIFICADOR_H_ */
