#ifndef CONFIGMEMORIA_H_
#define CONFIGMEMORIA_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <bibliotecaFunciones/usoVariado.h>
#include <sys/inotify.h>
#include <pthread.h>
#include <semaphore.h>

t_log *loggerMemoria;
t_config *archivoConfigMemoria;
char* nroMemoriaLogger;

char* PATH_MEMORIA_CONFIG;

typedef struct
{
	int puertoDeEscucha;
	char* ipDeEscucha;
	char *ipDelFileSystem;
	int puertoDelFileSystem;
	char **ipDeSeeds;
	int puertosDeSeeds[150];
	int retardoAccesoMemoriaPrincipal;
	int retardoAccesoFileSystem;
	int tamanioMemoria;
	int tiempoJournal;
	int tiempoGossiping;
	int numeroDeMemoria;

}t_ConfigMemoria;

t_ConfigMemoria configMemoria;
pthread_mutex_t semConfig;


t_ConfigMemoria leerConfigMemoria(void);
void mostrarDatosArchivoConfig();
void configurarLoggerMemoria();
void exit_gracefully(int exitInfo);
void actualizarArchivoConfig();
void* observer_config();

#endif /* CONFIGMEMORIA_H_ */

