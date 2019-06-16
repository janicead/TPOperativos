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


#define PATH_KERNEL_CONFIG "/home/utnso/workspace/tp-2019-1c-BEFGN/Kernel/Config/kernel.config"


typedef struct
{
	char* ip_memoria;
	int puerto_memoria;
	int quantum;
	int multiprocesamiento;
	int metadata_refresh;
	int sleep_execution;

}t_ConfigKernel;

t_ConfigKernel configKernel;

int leerConfigKernel();
void verificarArchivoConfigKernel();
void mostrarDatosArchivoConfigKernel();
void* observer_config();

#endif /* CONFIGKERNEL_H_ */
