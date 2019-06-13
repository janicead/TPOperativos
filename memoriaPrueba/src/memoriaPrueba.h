#ifndef MEMORIAPRUEBA_H_
#define MEMORIAPRUEBA_H_

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

//#include <futbolitoCommons/futbolitoSerializacion.h>
//#include <futbolitoCommons/futbolitoSocket.h>
//#include <bibliotecaFunciones/lfsSocket.h>

//#include <bibliotecaFunciones/sockets.h>
//#include <bibliotecaFunciones/lfsSerializacion.h>
#include <bibliotecaFunciones/lfsProtocolos.h>


// Definimos algunas constantes para nuestro código
//#define IP "tp0.utnso.com"
#define IP "127.0.0.1"
#define PUERTO "35124"

// Definimos algunas variables globales
t_log * logger;

// A continuacion estan las estructuras con las que nos vamos a manejar.
typedef struct  {
  int id_mensaje;
  int legajo;
  char nombre[40];
  char apellido [40];
} __attribute__((packed)) Alumno;
/*
typedef struct {
	int ID;
	int longDatos;
} __attribute__((packed)) ContentHeader;
*/

// Finalmente, los prototipos de las funciones que vamos a implementar
void configure_logger();
int  connect_to_server(char * ip, char * port);
void wait_hello(int socket);
Alumno read_hello();
void send_hello(int socket, Alumno alumno);
void * wait_content(int socket);
void send_md5(int socket, void * content);
void wait_confirmation(int socket);
void exit_gracefully(int return_nr);


/*
*/
/*
typedef struct
{
	uint32_t ID;
	uint32_t longDatos;
	char *Datos;
}t_PaqueteDeDatos;
*/


void leerConfigMDJ(void);
void iniciarEscucha(void);
//void realizarHandshakeAlLFS(int, char*);
void aceptarConexiones(int);

//FUNCIONES HACIA LFS
/*
char *opSELECT(int socketReceptor, char* unNombreTabla, int unaKey);
char *opINSERT(int socketReceptor, char* unNombreTabla, int unaKey, char* unValue, int unTimestamp);
char *opCREATE(int socketReceptor, char* unNombreTabla, char* unaConsistencia, int unNParticiones, int unTiempoCompactacion);
char *opDESCRIBE(int socketReceptor, char* unNombreTabla);
char *opDROP(int socketReceptor, char* unNombreTabla);
*/

//void empaquetarEnviarMensaje(int , int , int , char *);
//void definirHeader(t_PaqueteDeDatos*, int , int );
//t_PaqueteDeDatos* recibirPaquete(int);


//SERIALIZACION
/*
typedef struct
{
	uint32_t longPath;
	char *Path;
}t_Path;*/

//char *serializarPath(t_Path *);
//t_Path *definirt_Path(char *);







#endif /* MEMORIAPRUEBA_H_ */
