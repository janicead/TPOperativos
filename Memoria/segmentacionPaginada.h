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
	int indiceMemoria;
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

t_list * tablaDeSegmentos; //elementos de tipo t_segmento
int cantMaxPags;
t_list* listaJournal;
t_registro* memoriaPrincipal;
int tamanioMemoriaPrincipal;

void definirTamanioMemoriaPrincipal( int tamanioValueDadoXLFS);
void mostrarElementosMemoriaPrincipal(t_registro *memoriaPrincipal);
void mostrarElementosTablaSegmentos();
void mostrarElementosTablaPaginas(t_list * lista);
void mostrarElementosListaJournal();
int buscarTablaSegmentos(char* nombreTabla);
char* buscarTablaPaginas(t_list* tabla,uint16_t key, t_registro* memoriaPrincipal);
int buscarEnTablaPaginasINSERT(t_list* tabla, uint16_t key,int timeStamp, t_registro* memoriaPrincipal , char* value);
char* buscarEnMemoriaPrincipal( int indice,t_registro* memoriaPrincipal);
int buscarEnMemoriaPrincipalTimeStamp( int indice,t_registro* memoriaPrincipal);
uint16_t buscarEnMemoriaPrincipalKey(int indice, t_registro* memoriaPrincipal);
void actualizarMemoriaPrincipal( int indice,t_registro* memoriaPrincipal, int timeStamp, char* value);
t_segmento* guardarEnTablaDeSegmentos(char* nombreTabla);
int buscarEspacioEnMP(t_registro * memoriaPrincipal);
void guardarEnTablaDePaginas(t_segmento * segmento, int indice,uint16_t key, int flagModificado);
t_LRU * LRU();
int buscarSiExistePaginaNoModificada (t_list*tablaDePaginas);
int guardarEnMemoria(char* nombreTabla, uint16_t key, char* value, t_registro* memoriaPrincipal);
int tamanioArray(void** array);
int obtenerTimeStamp();
char* recibirRespuestaSELECTMemoriaLfs();
void quitarEspaciosGuardadosEnMemoria(t_list* lista , t_registro* memoriaPrincipal);
void borrarDeMemoria (int indice, t_registro* memoriaPrincipal);
void borrarTablaDePaginas(t_list* lista);
char* SELECTMemoria(char * nombreTabla, uint16_t key, t_registro* memoriaPrincipal, int flagModificado);
void INSERTMemoria(char * nombreTabla, uint16_t key, char* value, int timeStamp, t_registro* memoriaPrincipal);
int tamanioLista(t_list * lista);

#endif /* SEGMENTACIONPAGINADA_H_ */
