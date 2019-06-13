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
	uint32_t longString;
	char *String;
}t_UnString;

t_UnString *definirT_UnString(char *unString);
char *serializarT_UnString(t_UnString *structASerializar);
/*LIBERA LA MEMORIA DE LO Q DEVUELVE*/
t_UnString *deserializarT_UnString(char *t_UnStringSerializado);

void freeT_UnString(t_UnString *unStruct);

//#################################
typedef struct
{
	uint32_t KEY;
	uint32_t longNombre;
	char *nombreTabla;
}t_SELECT;

t_SELECT *definirT_SELECT(char* nombreTabla, int unaKey);//
char *serializarT_SELECT(t_SELECT *structASerializar);
t_SELECT *deserializarT_SELECT(char *t_SELECTSerializado);

void freeT_SELECT(t_SELECT *unStruct);

//#################################
typedef struct
{
	uint32_t KEY;
	uint32_t timeStamp;
	uint32_t longNombre;
	uint32_t longValue;
	char *nombreTabla;
	char *Value;
}t_INSERT;

t_INSERT *definirT_INSERT(char *nombreTabla, int unaKey, char* unValue, int timeStamp);
char *serializarT_INSERT(t_INSERT *structASerializar);
t_INSERT *deserializarT_INSERT(char *t_INSERTSerializado);

void freeT_INSERT(t_INSERT *unStruct);

//#################################
typedef struct
{
	uint32_t nParticiones;
	uint32_t tiempoCompactacion;
	uint32_t longNombre;
	uint32_t longConsistencia;
	char *nombreTabla;
	char *tipoConsistencia;
}t_CREATE;

t_CREATE *definirT_CREATE(char *nombreTabla, char* tipoConsistencia, int nroParticiones, int tiempoCompactacion);
char *serializarT_CREATE(t_CREATE *structASerializar);
t_CREATE *deserializarT_CREATE(char *t_CREATESerializado);

void freeT_CREATE(t_CREATE *unStruct);

//#################################
typedef struct
{
	uint32_t longNombre;
	char *nombreTabla;
}t_DESCRIBE;

t_DESCRIBE *definirT_DESCRIBE(char *unNombreTabla);
char *serializarT_DESCRIBE(t_DESCRIBE *structASerializar);
/*LIBERA LA MEMORIA DE LO Q DEVUELVE*/
t_DESCRIBE *deserializarT_DESCRIBE(char *t_DESCRIBESerializado);

void freeT_DESCRIBE(t_DESCRIBE *unStruct);

//#################################
typedef struct
{
	uint32_t longNombre;
	char *nombreTabla;
}t_DROP;

t_DROP *definirT_DROP(char *unNombreTabla);
char *serializarT_DROP(t_DROP *structASerializar);
/*LIBERA LA MEMORIA DE LO Q DEVUELVE*/
t_DROP *deserializarT_DROP(char *t_DROPSerializado);

void freeT_DROP(t_DROP *unStruct);

//#################################

//#################################


//#################################
/*PayloadRespuesta, seria el PAYLOAD "Datos" del paquete recibido, osea la RESPUESTA
 * q es una estructura t_ValidarPath, serializado
 * USAR FREE()  CON LO Q DEVUELVE*/
char *deserializarRespuesta(char *PayloadRespuesta);
//#################################

//##########################################################

#endif
