#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sockets.h>
#include "memoria_commons.h"
#include "configMemoria.h"

int socketMemoria;  //Socket servidor

void multiplexacion(int);
int aceptarConexiones(int);
void escuchar();

#endif
