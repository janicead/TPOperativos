#ifndef SRC_SOCKETSMEMORIA_H_
#define SRC_SOCKETSMEMORIA_H_
#include <stdio.h> // Por dependencia de readline en algunas distros de linux :)
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <time.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include "configMemoria.h"
#include "segmentacionPaginada.h"
#include <bibliotecaFunciones/sockets.h>
#include <bibliotecaFunciones/lfsSerializacion.h>

fd_set master;   // conjunto maestro de descriptores de fichero
fd_set copy;
struct sockaddr_in servidorMemoria;     // dirección del servidor
struct sockaddr_in clienteMemoria; // dirección del cliente
int servidorEscuchaMemoria;
       // para setsockopt() SO_REUSEADDR, más abajo
int kernel;

//t_memoriaConectada* tablaDeGossip [150];
void definirValorKernel();
void realizarGossip();
void iniciarEscuchaMemoria();
void serCliente(char* ip, int puerto);
void conectarmeAEsaMemoria(int puerto,char* ip, t_log* logger);
void realizarMultiplexacion(int socketEscuchando);
void hacermeClienteDeMisServers();
int aceptarConexiones(int socket, t_log* logger);
void exitGracefully(int return_nr, t_log* logger, int servidorEscucha);
void gestionarPaquetes(t_PaqueteDeDatos* package, int socketEmisor);





#endif /* SRC_SOCKETSMEMORIA_H_ */
