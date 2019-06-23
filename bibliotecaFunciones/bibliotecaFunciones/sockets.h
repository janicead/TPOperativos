#ifndef BIBLIOTECAFUNCIONES_SOCKETS_H_
#define BIBLIOTECAFUNCIONES_SOCKETS_H_
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <arpa/inet.h>
#include <netdb.h>



typedef enum identidad{
	MEMORIA,LFS,KERNEL,KERNELOMEMORIA} t_identidad;

typedef struct{
	t_identidad identidad;
	char* mensaje;
	uint32_t longMensaje;
}t_handShake;

typedef struct
{
	uint32_t ID;
	uint32_t longDatos;
	char *Datos;
}t_PaqueteDeDatos;

typedef struct {
	char* ip;
	int puerto;
	int numeroDeMemoria;
}t_memoriaTablaDeGossip;


t_list * tablaDeGossip;

int conectarAlServidorEric(char * ip, char * port);
int crearServidor();
void escuchar(int servidor,struct sockaddr_in dirSocket);
int conectarAlServidor( char* ip, int puerto, t_log* logger);
void emitirMensaje(int cliente);
void corteStringAntesDeEnter (char* mensaje, int largo);
void receptarMensaje(int cliente);

void freePackage(t_PaqueteDeDatos *);

//HANDSHAKEEEEEEEEEEEEEEEEEEEEEE
void realizarHandShake(int servidor, t_identidad identidad, char*mensaje);
t_handShake* crearPunteroHandShake(t_handShake handShake);
char* serializarHandShake(t_handShake *handShake);
void empaquetarEnviarMensaje(int socketReceptor, int unID, int longitudDatos, char *datos);
void definirHeader(t_PaqueteDeDatos *unPackage,int unID, int unaLongitudData);
void freeHandShake(t_handShake* handShake);
void enviarMemoriasTablaGossip(int servidor, t_identidad identidad, char*mensaje);
int recibirHandShakeMemoria(int emisor,t_identidad identidad, t_log* logger);
char* verificarMensajeDeMemoria(char* mensaje,t_identidad identidad);
char* chequear(char** mensaje);
char** cortaPalabra(char* mensaje);
int verificarMensajeDeKernel(char* mensaje,t_identidad identidad);
t_PaqueteDeDatos *recibirPaquete(int socketEmisor);
t_handShake* deserializarHandShake(char *handShakeSerializado);
/////////////////////////////CONEXION KERNEL Y MEMORIA///////////////////////////

void mostrarmeMemoriasTablaGossip();
int verificarMensajeMemoriasTablaGossip(char* mensaje, t_log* logger);
void recibirMemoriasTablaDeGossip(int emisor,t_identidad identidad, t_log* logger);
char* memoriasTablaDeGossip();
int cantMemoriasTablaDeGossip();
void agregarATablaDeGossip(int puerto, char* ip, int memoria);
int revisarQueNoEsteEnLaLista(int nroMemoria);
char * quitarComillas(char* ip);





#endif /* BIBLIOTECAFUNCIONES_SOCKETS_H_ */
