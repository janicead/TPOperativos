#ifndef KERNEL_COMMONS_H_
#define KERNEL_COMMONS_H_

#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>

/**
* @NAME: t_LQL_operacion
* @DESC: estructura con los datos de una operacion de un script LQL
*/
typedef struct {
	bool valido;
	enum {
		SELECT,
		INSERT,
		CREATE,
		DESCRIBE,
		DROP,
		JOURNAL,
		ADD,
		RUN,
		METRICS
	} keyword;
	union {
		struct {
			char* nombre_tabla;
			int key;
		} SELECT;
		struct {
			char* nombre_tabla;
			int key;
			char* valor;
		} INSERT;
		struct {
			char* nombre_tabla;
			char* tipo_consistencia;
			int numero_particiones;
			int compactation_time;
		} CREATE;
		struct {
			char* nombre_tabla;
		} DESCRIBE;
		struct {
			char* nombre_tabla;
		} DROP;
		struct {
			char* path;
		} RUN;
		struct {
			int nro_memoria;
			char* criterio;
		} ADD;
	} argumentos;
	char** _raw; //Para uso de la liberación
} t_LQL_operacion;

typedef enum{
	NEW,
	READY,
	EXEC,
	EXIT
}t_estado;

/**
* @NAME: t_lcb
* @DESC: bloque de control de un bloque de control de un script LQL
* 	id_lcb: id único para identificar un lcb
* 	operaciones: lista de operaciones lql que posee el lcb, en el caso de ser una request por consola, la lista sólo tendrá un elemento
* 	program_counter: indica la operación siguiente a realizar de la lista de operaciones
* 	estado: indica el estado en el que se encuentra el lcb en el sistema actualmente
*
*/
typedef struct{
	int id_lcb;
	t_list* operaciones;
	int program_counter;
	t_estado estado;
}t_lcb;

int id;
t_log* loggerKernel;
t_config* archivoConfigKernel;
t_queue* queue_new,queue_ready,queue_exit;

void configure_logger_kernel();
void destruir_operacion(t_LQL_operacion* op);
void crear_colas();
t_lcb* crear_lcb();
void exit_gracefully(int exitInfo);
void inicializarId();
void free_lcb(t_lcb* lcb);

#endif /* KERNEL_COMMONS_H_ */
