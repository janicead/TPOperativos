#include "consolaKernel.h"

void setConsole(){
	char* linea;
	int cantidadParametros;
	while(1){
		linea = readline(">");
		if(linea){
			add_history(linea);
		}
		//INSTRUCCIÓN SELECT PARÁMETROS: [NOMBRE TABLA] [KEY]
		if(!strncmp(linea,"select",6) || !strncmp(linea,"SELECT",4)){
			cantidadParametros = 2;
			int key;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				log_error(loggerKernel,"No se ha especificado un nombre de tabla");
			}
			else if(parametros[2] == NULL || string_is_empty(parametros[2])){
				log_error(loggerKernel,"No se ha especificado ninguna key");
			}
			else{
				puts(parametros[1]);
				key = atoi(parametros[2]);
				printf("%d\n",key);
				//select(parametros[1],key);
			}
			freeParametros(parametros);
		}
		//INSTRUCCIÓN INSERT PARÁMETROS: [NOMBRE TABLA] [KEY] "[VALUE]" [TIMESTAMP]
		else if(!strncmp(linea,"insert",6) || !strncmp(linea,"INSERT",4)){
			cantidadParametros = 4;
			int key,timestamp;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				log_error(loggerKernel,"No se ha especificado un nombre de tabla");
			}
			else if(parametros[2] == NULL || string_is_empty(parametros[2])){
				log_error(loggerKernel,"No se ha especificado ninguna key");
			}
			else if(parametros[3] == NULL || string_is_empty(parametros[3])){
				log_error(loggerKernel,"No se ha especificado ningun valor");
			}
			else{
				key = atoi(parametros[2]);
				if(parametros[4] == NULL || string_is_empty(parametros[4])){
					//USAR el valor actual del Epoch UNIX.
					//timestamp =
				}
				else{
					timestamp = atoi(parametros[4]);
				}
				puts(parametros[1]);
				printf("%d\n",key);
				puts(parametros[3]); // el value viene dado con comillas
				printf("%d\n",timestamp);
				//insert(parametros[1],key,parametros[3],timestamp]
			}
			freeParametros(parametros);
		}
		//INSTRUCCIÓN CREATE PARÁMETROS: [NOMBRE TABLA] [TIPO DE CONSISTENCIA] [CANTIDAD PARTICIONES] [TIEMPO DE COMPACTACION]
		else if(!strncmp(linea,"create",6) || !strncmp(linea,"CREATE",4)){
			cantidadParametros = 4;
			int numeroParticiones,compactationTime;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				log_error(loggerKernel,"No se ha especificado un nombre de tabla");
			}
			else if(parametros[2] == NULL || string_is_empty(parametros[2])){
				log_error(loggerKernel,"No se ha especificado ningun tipo de consistencia");
			}
			else if(parametros[3] == NULL || string_is_empty(parametros[3])){
				log_error(loggerKernel,"No se ha especificado ninguna cantidad de particiones");
			}
			else if(parametros[4] == NULL || string_is_empty(parametros[4])){
				log_error(loggerKernel,"No se ha especificado ningun tiempo de compactación");
			}
			else{
				numeroParticiones = atoi(parametros[3]);
				compactationTime = atoi(parametros[4]);
				puts(parametros[1]);
				puts(parametros[2]);
				printf("%d\n",numeroParticiones);
				printf("%d\n",compactationTime);
				//create(parametros[1],parametros[2],numeroParticiones,compactationTime]
			}
			freeParametros(parametros);
		}
		//INSTRUCCIÓN DESCRIE PARÁMETROS: [NOMBRE TABLA]
		//SI NO HAY PARAMETROS SE DESCRIBEN TODAS LAS TABLAS
		else if(!strncmp(linea,"describe",8) || !strncmp(linea,"DESCRIBE",4)){
			cantidadParametros = 1;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				//describeTotal();
			}
			else{
				puts(parametros[1]);
				//describe(parametros[1]);
			}
			freeParametros(parametros);
		}
		//INSTRUCCIÓN DESCRIBE PARÁMETROS: [NOMBRE TABLA]
		else if(!strncmp(linea,"drop",4) || !strncmp(linea,"DROP",4)){
			cantidadParametros = 1;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				log_error(loggerKernel,"No se ha especificado un nombre de tabla");
			}
			else{
				puts(parametros[1]);
				//drop(parametros[1]);
			}
			freeParametros(parametros);
		}
		//INSTRUCCIÓN JOURNAL NO TIENE PARÁMETROS
		else if(string_equals_ignore_case(linea,"journal")){
			puts("JOURNAL");
			//journal();
		}
		//INSTRUCCIÓN ADD PARÁMETROS: MEMORY [NRO DE MEMORIA] TO [CRITERIO]
		else if(!strncmp(linea,"add",3) || !strncmp(linea,"ADD",3)){
			cantidadParametros = 4;
			int memoria;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[2] == NULL || string_is_empty(parametros[2])){
				log_error(loggerKernel,"No se ha especificado un número de memoria");
			}
			else if(parametros[4] == NULL || string_is_empty(parametros[4])){
				log_error(loggerKernel,"No se ha especificado ningún criterio");
			}
			else{
				memoria = atoi(parametros[2]);
				printf("%d\n",memoria);
				puts(parametros[4]);
				//add(memoria,parametros[4]);
			}
			freeParametros(parametros);
		}
		//INSTRUCCIÓN RUN PARÁMETROS: [PATH SCRIPT LQL]
		else if(!strncmp(linea,"run",3) || !strncmp(linea,"RUN",3)){
			cantidadParametros = 1;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				log_error(loggerKernel,"No se ha especificado ningún path LQL");
			}
			else{
				puts(parametros[1]);
				//run(parametros[1]);
			}
			freeParametros(parametros);
		}
		//INSTRUCCIÓN METRICS NO TIENE PARÁMETROS
		else if(string_equals_ignore_case(linea,"metrics")){
			puts("METRICAS");
			//metrics();
		}
		else if(!strncmp(linea,"exit",4) || !strncmp(linea,"EXIT",4)){
			exit_gracefully(EXIT_SUCCESS);
			free(linea);
			break;
		}
		else{
			log_error(loggerKernel,"No se ingreso ningun comando valido");
		}
		free(linea);
	}
	//return NULL;
}

char** obtenerParametros(char* linea, int indice){
		char** parametro = string_n_split(linea,indice," ");
		return parametro;
}

void freeParametros(char** parametros){
    string_iterate_lines(parametros, (void*) free);
	free(parametros);
}
