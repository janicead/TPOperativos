#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "configKernel.h"

int socketKernel;

void multiplexacion(int);
int aceptarConexiones(int);

#endif /* KERNEL_H_ */
