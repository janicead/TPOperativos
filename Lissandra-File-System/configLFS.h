#ifndef CONFIGLFS_H_
#define CONFIGLFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <commons/log.h>
#include "LFS_commons.h"


#define PATH_LFS_CONFIG "/home/utnso/tp-2019-1c-BEFGN/Lissandra-File-System/Config/LFS.config"

typedef struct
{
	int puerto_escucha;
	char* punto_montaje;
	int retardo;
	int tamanio_value;
	int tiempo_dump;

}t_ConfigLFS;

t_ConfigLFS configLFS;

int leerConfigLFS();
void verificarArchivoConfig();
void mostrarDatosArchivoConfig();

#endif /* CONFIGLFS_H_ */
