#ifndef SRC_SOCKETSKERNEL_H_
#define SRC_SOCKETSKERNEL_H_

#include <stdio.h> // Por dependencia de readline en algunas distros de linux :)
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <bibliotecaFunciones/sockets.h>
#include "configKernel.h"
#include <bibliotecaFunciones/usoVariado.h>

t_list * memoriasALasQueMeConecte;
int memoria;
void definirValorMP();
void conectarmeAMP();
void recibirMemorias();
void recibirMemoriasTablaDeGossipKernel(int emisor,t_identidad identidad);
int verificarMensajeMemoriasTablaGossipKernel(char* mensaje);
void agregarATablaDeGossipKernel(int puerto, char* ipServidor, int memoria);
void conectarmeAMemorias();
int conectarmeAMemoriaEspecifica(int puerto,char* ip);
void agregarAMemoriasConectadasAKernel(int puerto, char* ip, int memoria);
#endif /* SRC_SOCKETSKERNEL_H_ */
