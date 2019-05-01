#ifndef PARSER_H_
#define PARSER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <commons/string.h>
#include "kernel_commons.h"

typedef struct {
	bool valido;
	enum {
		SELECT,
		INSERT,
		CREATE,
		DESCRIBE,
		DROP,
		JOURNAL,
		ADD,
		RUN,
		METRICS
	} keyword;
	union {
		struct {
			char* nombre_tabla;
			int key;
		} SELECT;
		struct {
			char* nombre_tabla;
			int key;
			char* valor;
		} INSERT;
		struct {
			char* nombre_tabla;
			char* tipo_consistencia;
			int numero_particiones;
			int compactation_time;
		} CREATE;
		struct {
			char* nombre_tabla;
		} DESCRIBE;
		struct {
			char* nombre_tabla;
		} DROP;
		struct {
			char* path;
		} RUN;
		struct {
			int nro_memoria;
			char* criterio;
		} ADD;
	} argumentos;
	char** _raw; //Para uso de la liberación
} t_LQL_operacion;


t_LQL_operacion parse(char* linea);
void destruir_operacion(t_LQL_operacion op);
t_LQL_operacion parse_error();

#endif /* PARSER_H_ */
