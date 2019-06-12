/*
 * lfsSerializacion.h
 *
 *  Created on: 27 abr. 2019
 *      Author: utnso
 */

#ifndef LFSSERIALIZACION_H_
#define LFSSERIALIZACION_H_


#include <stdio.h> // Por dependencia de readline en algunas distros de linux :)
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <readline/readline.h> // Para usar readline

//#################################
typedef struct
{
	uint32_t longPath;
	char *Path;
}t_ValidarPath;

t_ValidarPath *definirT_ValidarPath(char *unPath);
char *serializarT_ValidarPath(t_ValidarPath *structASerializar);
/*LIBERA LA MEMORIA DE LO Q DEVUELVE*/
t_ValidarPath *deserializarAT_ValidarPath(char *t_ValidarPathSerializado);

void freeT_ValidarPath(t_ValidarPath *unStruct);

//#################################
typedef struct
{
	uint32_t NcantidadBytes;
	uint32_t longPath;
	char *Path;
}t_CrearArchivo;

t_CrearArchivo *definirT_CrearArchivo(char *unPath, int nCantidadDeBytes);//
char *serializarT_CrearArchivo(t_CrearArchivo *structASerializar);
t_CrearArchivo *deserializarAT_CrearArchivo(char *t_CrearArchivoSerializado);

void freeT_CrearArchivo(t_CrearArchivo *unStruct);

//#################################
typedef struct
{
	uint32_t Offset;
	uint32_t Size;
	uint32_t longPath;
	char *Path;
}t_ObtenerDatos;

t_ObtenerDatos *definirT_ObtenerDatos(char *unPath, int offset, int size);
char *serializarT_ObtenerDatos(t_ObtenerDatos *structASerializar);
t_ObtenerDatos *deserializarAT_ObtenerDatos(char *t_ObtenerDatosSerializado);

void freeT_ObtenerDatos(t_ObtenerDatos *unStruct);

//#################################
typedef struct
{
	uint32_t Offset;
	uint32_t Size;
	uint32_t longPath;
	uint32_t longBuffer;
	char *Path;
	char *Buffer;
}t_GuardarDatos;

t_GuardarDatos *definirT_GuardarDatos(char *unPath, int offset, int size, char *buffer);
char *serializarT_GuardarDatos(t_GuardarDatos *structASerializar);
t_GuardarDatos *deserializarAT_GuardarDatos(char *t_GuardarDatosSerializado);

void freeT_GuardarDatos(t_GuardarDatos *unStruct);

//#################################
/*PayloadRespuesta, seria el PAYLOAD "Datos" del paquete recibido, osea la RESPUESTA
 * q es una estructura t_ValidarPath, serializado
 * USAR FREE()  CON LO Q DEVUELVE*/
char *deserializarRespuesta(char *PayloadRespuesta);
//#################################
//### OTRAS FUNCIONES ######################################
char **splitSegunTamanioCorte(char *s,int longS,int tamanioCorte);
int cuantosBloquesNecesitoPara(int nCantidadBytes,int tamanioBloque);//char *archivo);
void freeArrayDePunteros(char **unArrayDePunteros);

//##########################################################

#endif
