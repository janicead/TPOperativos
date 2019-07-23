#ifndef CONFIGKERNEL_H_
#define CONFIGKERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <commons/log.h>
#include "kernel_commons.h"
#include "planificador.h"

typedef struct
{
	char* ip_memoria;
	int puerto_memoria;
	int quantum;
	int multiprocesamiento;
	int metadata_refresh;
	int sleep_execution;
	char **ipDeSeeds;
	int puertosDeSeeds[150];
	int tiempoGossiping;

}t_ConfigKernel;

t_ConfigKernel configKernel;

int leerConfigKernel();
void verificarArchivoConfigKernel();
void mostrarDatosArchivoConfigKernel();
void* observer_config();
void actualizar_multiprocesamiento();
void actualizarArchivoConfig();

#endif /* CONFIGKERNEL_H_ */
