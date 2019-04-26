#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

int conectarAlServidor(char * ip, char * port);
void escuchar(int socketServer);

#endif /* SOCKETS_H_ */
