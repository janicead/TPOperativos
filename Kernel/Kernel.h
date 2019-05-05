#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "configKernel.h"
#include <sockets.h>
#include "consolaKernel.h"
#include "planificador.h"
#include "kernel_commons.h"
#include <types.h>


char* puertoMemoria;
char* ipMemoria;

void iniciar();
void conectarAMemoria();


#endif /* KERNEL_H_ */
