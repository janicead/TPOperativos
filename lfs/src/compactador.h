#ifndef COMPACTADOR_H_
#define COMPACTADOR_H_

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

#include <commons/log.h>
#include <commons/collections/list.h>
//#include <commons/config.h>
//#include <commons/bitarray.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/txt.h>

#include <bibliotecaFunciones/lfsSerializacion.h>
#include <bibliotecaFunciones/sockets.h>

#include "lfs.h"
#include "varios.h"
#include "fs.h"
/*
typedef struct
{
	char *nombreTabla;
	t_list *registros;
	t_list *temporales; //NOMBRES DE ARCHIVOS .tmp .tmpc
	pthread_t hiloIDCompactador; //o un bool SW de un break en el hilo
}t_Tabla;*/

//#############################################
t_list *obtenerArchivoComoLista(char *unNombreTablaArchivo);
char *dumpearUnaListaDeRegistros(t_list *unaLista);
void freeListaDeRegistros(t_list *unaListaRegistros);
void hiloDUMP(void);
void realizarDUMP(void);
void persistirRegistrarDUMP(t_Tabla *unaTabla,char *laTablaDUMPEADA);
char *getNombreArchivoTEMP(t_Tabla *unaTabla);
int cuantosArchivosTempHayEn(t_list *unaListaTemporales);
void agregarArchivoTempALista(t_list *unaLista,char *nombreArchivo, int unTipo);

int numeroDeParticion(int cantParticiones, int unaKey);

//#############################################

#endif /* COMPACTADOR_H_ */
