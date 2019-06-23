#ifndef KERNEL_COMMONS_H_
#define KERNEL_COMMONS_H_

#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <bibliotecaFunciones/sockets.h>



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

typedef struct{
	char* nombre_tabla;
	char* consistencia;
}t_tabla;

typedef struct{
	int id_mem;
	int socket_mem;
	bool valida;
}t_memoria;

char* puertoMemoria;
int idLCB, idMEM, cambioMultiProcesamiento;

//VARIABLES DE CONFIGURACIÓN
t_log* loggerKernel;
t_config* archivoConfigKernel;

//COLAS
t_queue* queue_ready;
t_queue* queue_exit;

//LISTAS
t_list* hilos_ejec;
t_list* tablas;
t_list* memorias;
t_list* strong_hash_consistency;
t_list* eventual_consistency;
t_memoria* strong_consistency;

//SEMÁFOROS
pthread_mutex_t queue_ready_sem;
pthread_mutex_t queue_exit_sem;
pthread_mutex_t config_sem;
pthread_mutex_t memorias_sem;
pthread_mutex_t strong_consistency_sem;
pthread_mutex_t strong_hash_consistency_sem;
pthread_mutex_t eventual_consistency_sem;
pthread_mutex_t tablas_sem;
pthread_mutex_t log_sem;
sem_t execute_sem;

//HILOS
pthread_t* hilos;
pthread_t consola;
pthread_t timer_thread;
pthread_t config_observer;

//FUNCIONES DE CONFIGURACIÓN INICIAL
void crear_colas();
void crear_listas();
void configure_logger_kernel();
void inicializarIds();
void iniciar_semaforos();

//FUNCIONES DE GESTIÓN DE LCBs
t_lcb* crear_lcb();
void status_lcb(t_lcb*);
void pasar_lcb_a_ready();
void pasar_lcb_a_exit(t_lcb* lcb);
void agregar_op_lcb(t_lcb* lcb,t_LQL_operacion* op);
t_LQL_operacion* obtener_op_actual(t_lcb* lcb);

//FUNCIONES DE MANEJO DE LAS TABLAS Y MEMORIAS
void agregar_tabla(t_tabla* tabla);
void agregar_memoria(int socket);

//FUNCIONES DE LIBERAR MEMORIA
void destruir_operacion(t_LQL_operacion* op);
void exit_gracefully(int exitInfo);
void free_operacion(t_LQL_operacion* op);
void free_lcb(t_lcb* lcb);
void destruir_listas();
void destruir_colas();
void destruir_semaforos();
void free_memoria(t_memoria* memoria);
void free_tabla(t_tabla* tabla);


#endif /* KERNEL_COMMONS_H_ */
