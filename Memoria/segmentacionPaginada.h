#ifndef SEGMENTACIONPAGINADA_H_
#define SEGMENTACIONPAGINADA_H_

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>
#include <commons/temporal.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <pthread.h>
#include <semaphore.h>
#include <bibliotecaFunciones/usoVariado.h>

int socketLFS;


typedef struct {
	int numeroPag;
	int numeroMarco;
	int flagModificado;
	uint16_t key;
	int contadorVecesSolicitado;
}t_pagina;

typedef struct{
	t_list * tablaPaginas;
	char* nombreTabla;
}t_segmento;

typedef struct{
	t_list* tablaPaginas;
	int numeroPag;
	char* nombreTabla;
} t_LRU;

typedef struct {
	char* nombreTabla;
	t_registro* registro;
}t_JOURNAL;

t_list* tablaDeSegmentos; //elementos de tipo t_segmento
int cantMaxMarcos;
int cantMarcosIngresados;
int obtenerValue;
int tamanioUnRegistro;
int* marcosOcupados;
int tamanioDadoPorLFS;
t_list* listaJournal;
void* memoriaPrincipal;
int tamanioMemoriaPrincipal;


//----------------------------------------GENERALES--------------------------------------------------------------//
void definirTamanioMemoriaPrincipal( int tamanioValueDadoXLFS);
int tamanioLista(t_list * lista);
char* recibirRespuestaSELECTMemoriaLfs();
int obtenerTimeStamp();
//-------------------------------------BORRADO------------------------------------------------------------------//
void destructor2(t_pagina * pagina);
void destructor(t_segmento* segmento);
void borrarElementos();
void borrarTodo();
//------------------------------------------SEGMENTOS----------------------------------------------------------//
int buscarTablaSegmentos(char* nombreTabla);
t_segmento* guardarEnTablaDeSegmentos(char* nombreTabla);
void mostrarElementosTablaSegmentos();
//----------------------------------------PAGINAS-------------------------------------------------------------//
char* buscarTablaPaginas(t_list* tabla, uint16_t key);
void guardarEnTablaDePaginas(t_segmento * segmento, int nroMarco,uint16_t key, int flagModificado );
void mostrarElementosTablaPaginas(t_list * lista);
//------------------------------------------MEMORIA----------------------------------------------------------//
t_registro* buscarEnMemoriaPrincipal( int nroMarco);
int buscarEspacioLibreEnMP();
void settearMarcoEnMP(int nroMarco, int nroDeseado);
void guardarEnMPLugarEspecifico(uint16_t key, char* value, int nroMarco);
int guardarEnMemoria(char* nombreTabla, uint16_t key, char* value);
void mostrarElementosMemoriaPrincipal();
//---------------------------------------LRU----------------------------------------------------------------//
t_LRU * LRU ();
//-------------------------------------JOURNAL-------------------------------------------------------------//
void mostrarElementosListaJournal();
char* convertirAStringListaJournal();
void iniciarJournal();
//---------------------------------------REQUESTS--------------------------------------------------------//
char* SELECTMemoria(char * nombreTabla, uint16_t key, int flagModificado);

#endif /* SEGMENTACIONPAGINADA_H_ */
