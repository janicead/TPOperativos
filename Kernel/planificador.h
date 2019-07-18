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
bool verificar_memoria_caida(char* respuesta,t_LQL_operacion* op, int id_mem);
bool verificar_memoria_caida2(char* respuesta, int id_mem);

//FUNCIONES DE COMANDOS
void lql_select(t_LQL_operacion* operacion);
void lql_insert(t_LQL_operacion* op);
void lql_create(t_LQL_operacion* op);
void lql_describe(t_LQL_operacion* op);
void lql_drop(t_LQL_operacion* op);
void lql_journal(t_list* list_mem, t_LQL_operacion* op);
void lql_add(t_LQL_operacion* op);
void lql_run(FILE* archivo, t_LQL_operacion* op);
void lql_metrics(bool mostrarEnConsola);
void* refresh_metadata_timer();

//FUNCIONES PARA LAS MÉTRICAS
void* metrics_timer();
double tiempoPromedioSelect();
double tiempoPromedioInsert();
int cantidadSelects();
int cantidadInserts();
int porcentajeSelectsInserts(int cant_selects_inserts_ejecutados);
void memoryLoad(bool mostrarEnConsola);

//FUNCIONES EXTRA
FILE* abrirArchivo(char* path);
t_tabla* devuelve_tabla(char* nombre);
void describe_global(char* data, bool mostrarPorConsola);
void describe(char* data, bool mostrarPorConsola);

#endif /* PLANIFICADOR_H_ */
