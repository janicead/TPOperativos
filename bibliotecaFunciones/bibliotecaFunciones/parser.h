#ifndef BIBLIOTECAFUNCIONES_PARSER_H_
#define BIBLIOTECAFUNCIONES_PARSER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <commons/string.h>
#include "consola.h"

typedef struct {
	bool esValido;
	enum {
		SELECT,
		INSERT,
		CREATE,
		DESCRIBE,
		DROP,
		JOURNAL,
		ADD,
//		RUN, no se si run deberia ser uno de los que estan en los archivos
		METRICS
		} palabraReservada;
	union {
		struct {
			char* nombreTabla;
			int key;
			} SELECT;
		struct {
				char* nombreTabla;
				int key;
				char* value;
				int timestamp;
			} INSERT;
		struct {
				char* nombreTabla;
				char* tipoConsistencia;
				int numeroParticiones;
				int campactionTime;
			} CREATE;
		struct {
				char* nombreTabla;
			} DESCRIBE;
		struct {
				char* nombreTabla;
			} DROP;
		struct {
				int numero;
				char* criterio;
			} ADD; /*
		struct {
				char* path;
			} RUN; */
		} argumentos;
		char** liberar; //Para uso de la liberación
	} parserLQL;

parserLQL parser(char* linea);
void destruirOperacion(parserLQL operacion);



#endif /* BIBLIOTECAFUNCIONES_PARSER_H_ */
