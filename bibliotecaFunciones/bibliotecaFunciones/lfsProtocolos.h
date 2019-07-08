#ifndef LFSPROTOCOLOS_H_
#define LFSPROTOCOLOS_H_

#include <stdio.h> // Por dependencia de readline en algunas distros de linux :)
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <readline/readline.h> // Para usar readline

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

//#include <bibliotecaFunciones/sockets.h>
#include "sockets.h"
//#include <bibliotecaFunciones/lfsSerializacion.h>
#include "lfsSerializacion.h"


//#### MEMORIA -> LFS  #################
int realizarHandshakeAlLFS(t_log *logger,int socketServer, char *msjEnviado);

char *opSELECT(int socketReceptor, char* unNombreTabla, int unaKey);
char *opINSERT(int socketReceptor, char* unNombreTabla, int unaKey, char* unValue, int unTimestamp);
char *opCREATE(int socketReceptor, char* unNombreTabla, char* unaConsistencia, int unNParticiones, int unTiempoCompactacion);
char *opDESCRIBE(int socketReceptor, char* unNombreTabla);
char *opDROP(int socketReceptor, char* unNombreTabla);
char* opJOURNAL(int socketReceptor);
//######################################

//######################################

//######################################

#endif /* LFSPROTOCOLOS_H_ */
