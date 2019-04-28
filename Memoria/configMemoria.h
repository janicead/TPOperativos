#ifndef CONFIGMEMORIA_H_
#define CONFIGMEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <commons/log.h>
#include <commons/string.h>
#include "memoria_commons.h"


#define PATH_MEMORIA_CONFIG "/home/utnso/tp-2019-1c-BEFGN/Memoria/Config/memoria.config"

typedef struct
{
	int puerto;
	char* ip_fs; //ip del file system
	int puerto_fs;
	char** ip_seeds;
	char** puerto_seeds;
	int retardo_mem;
	int retardo_fs;
	int tam_mem;
	int retardo_journal;
	int retardo_gossiping;
	int memory_number;

}t_ConfigMemoria;

t_ConfigMemoria configMemoria;

int leerConfigMemoria();
void verificarArchivoConfigMemoria();
void mostrarDatosArchivoConfigMemoria();
/**
 * 		PARA LIBERAR LA MEMORIA DE LOS CHAR**
 */
void freeDatosConfigMemoria();

#endif /* CONFIGMEMORIA_H_ */
