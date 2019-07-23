#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/temporal.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <bibliotecaFunciones/consola.h>
#include <bibliotecaFunciones/consola.h>
#include <bibliotecaFunciones/sockets.h>
#include "consolaMemoria.h"
#include "configMemoria.h"
#include "socketsMemoria.h"
#include "segmentacionPaginada.h"


//HILOS//
pthread_t multiplexacionMemoria;
pthread_t config_observer;
pthread_t hacerJournal;
pthread_t consola;
pthread_t gossip;
pthread_t conexionLFS;

int socketLFS;

void crearTablaGossip();
void crearHilos();
#endif /* MEMORIA_H_ */
