#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <prueba.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int socketKernel;

void escuchar();
void multiplexacion(int);
int aceptarConexiones(int);

#endif /* KERNEL_H_ */
