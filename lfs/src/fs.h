#ifndef FS_H_
#define FS_H_

#include <stdio.h> // Por dependencia de readline en algunas distros de linux :)
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <readline/readline.h> // Para usar readline
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <libgen.h>
#include <dirent.h> //PARA ENLISTAR

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/txt.h>

//#include <futbolitoCommons/futbolitoSerializacion.h>
//#include <futbolitoCommons/futbolitoSocket.h>
#include <bibliotecaFunciones/lfsSerializacion.h>
#include <bibliotecaFunciones/sockets.h>

#include "lfs.h"



//#define maximoDeConexiones 10

//## CONFIG #################
t_config *archivoConfig;

typedef struct
{
	int puertoEscucha;
	char *puntoMontaje;
	int retardo;
}t_ConfigMDJ;

//## METADATA ########################
t_config *archivoMetadata;

typedef struct
{
	int tamanioBloque;
	int cantidadBloques;
	char *numeroMagico;
}t_Metadata;

//## METADATA ARCHIVO ########################
t_config *metadataUnArchivo;

typedef struct
{
	int Tamanio;
	char **Bloques;
}t_MetadataUnArchivo;

//## FILE SYSTEM ########################
typedef struct
{
	char *PATH;
	int longPath;
	int ID;
}t_Directorio;

typedef struct
{
	int ID;
	int tamanioArchivo;
	char **Bloques;
}t_FCB;

//##########################
t_config *archivoMetadataTabla;

typedef struct
{
	char *Consistency;
	int Partitions;
	int Compaction_Time;
}t_MetadataTabla;

//##########################

//VARIABLES GLOBALES
t_ConfigMDJ configMDJ;
t_log * logger;
t_Metadata metadata;
t_bitarray *bitsDeBloques;

char *discoLogico;


//PROTOTIPOS DE FUNCIONES

void exitGracefully(int);

void iniciarFileSystemLISSANDRA(void);
void leerConfigMetadata(int showMetadata);

//void realizarHandshakeAlCliente(int, char*, char*);

//void empaquetarEnviarMensaje(int , int , int , char *);
//void definirHeader(t_PaqueteDeDatos*, int , int );
//t_PaqueteDeDatos* recibirPaquete(int);

void realizarMultiplexacion(int unSocketConectado);
void realizarProtocoloDelPackage(t_PaqueteDeDatos *packageRecibido, int socketEmisor);

char *validarArchivoEnFS(char *path);
void crearArchivoEnFS(char *unPathArchivo);
void borrarArchivoEnFS(char *pathArchivo);

/*LA RESPUESTA SIEMPRE SERA UN CHAR* DE TAMANIO VARIABLE, no maneja estructuras, en el caso de
 * pasar un archivo, a fin de cuenta se manda una cadena de caracteres */
//void enviarRespuesta(int socketReceptor, int protocoloID, char *respuesta);

void obtenerBitArrayPersistido(void);
void mostrarBitsDeBloques(int nBloquesPorLinea);
//void crearBitArrayDeCero(void);
/*SE SOBRE ESCRIBE EL ARCHIVO, arrancando desde el inicio*/
void escribirLineaEn(char *archivoPath, char *linea);
char *leerBloqueDe(char *pathArchivoBloque,int nBytes);

void setBitDeBloque(int indexBloque,int nuevoValor);
bool hayEspacioDisponible(int cantBytesParaGuardar);
/*DEVUELVE EL INDEX CORESPONDIENTE*/
int getPrimerBloqueLibre(void);
//int cuantosBloquesNecesitoPara(int nCantidadBytes,int tamanioBloque);//char *archivo);
/*FREE() A LO Q RETORNE*/
char *asignarBloques(int cantidadDeBytes);
void liberarBloquesDe(char *nombreAbsolutoArchivoMeta);
void reAsignarBloquesDe(char *nombreAbsolutoArchivoMeta);

t_MetadataUnArchivo *leerMetadataDeArchivo(char *unNombreAbsoluto,int showLoLeido);
void freeT_MetadataUnArchivo(t_MetadataUnArchivo *unStruct);

//char **splitSegunTamanioCorte(char *s,int longS,int tamanioCorte);
void guardarBufferArchivoEn(char *pathAbsoluto,char* bufferArchivo,int longBuffer);
char *getTodoElArchivoBufferDe(char *pathAbsoluto);

//SERIALIZACION
/*
typedef struct
{
	uint32_t longPath;
	char *Path;
}t_Path;*/
//### OTRAS FUNCIONES ######################################
char **splitSegunTamanioCorte(char *s,int longS,int tamanioCorte);
int cuantosBloquesNecesitoPara(int nCantidadBytes,int tamanioBloque);//char *archivo);
//void freeArrayDePunteros(char **unArrayDePunteros);


//t_Path *deserializarPath(char *);

//###### OPERACIONES FS ######
char *existeTablaEnFS(char* unNombreTabla);
char *crearTablaEnFS(t_CREATE *unCREATE);
char *obtenerDatosDeArchivoEnFS(char *unPath);
char *guardarDatosEnArchivoEnFS(char *unPathArchivo, char *todoElArchivo);
char *eliminarArchivoEnFS(char *unPathArchivo);
t_MetadataTabla *obtenerMetadataTabla(char *unNombreTabla);
void freeT_MetadataTabla(t_MetadataTabla *unStruct);

//##########################
char **enlistarElPath(char *unPath);
char **enlistarCarpetaTabla(char *unNombreTabla);

void cargarTablasPersistidasEnMEMTABLE(void);

#endif /* FS_H_ */
