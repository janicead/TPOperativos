#ifndef TEMPORAL_H_
#define TEMPORAL_H_
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <string.h>
#include <commons/log.h>

/* PARA SWITCHEAR */
#define CMD_SELECT 0
#define CMD_INSERT 1
#define CMD_CREATE 2
#define CMD_DESCRIBE 3
#define CMD_DROP 4
#define CMD_JOURNAL 5
#define CMD_ADD 6
#define CMD_RUN 7
#define CMD_METRICS 8
#define CMD_EXIT 9
#define CMD_NOENCONTRADO -1

int comandoDeString(char* key);
int validarComando(char* linea, t_log* logger);
char** obtenerParametros(char*, int);
int chequearValoresNULL(char** , int );
int verificarQueDigaPalabraEspecifica(char*, char*, int);
int verificarCantidadParametrosPasados(char* linea);


#endif /* TEMPORAL_H_ */
