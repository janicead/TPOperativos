#include "consolaMemoria.h"

void* crearConsolaMemoria(){
	char *linea;
	char **operacion;
	int cantArgumentos;

	while(1){
		linea = readline(">");

		if(linea){
			add_history(linea);
		}

		operacion = string_split(linea," ");

		if(validarComando(linea, loggerMemoria)){
			switch(comandoDeString(operacion[0])){
				case CMD_SELECT:
					printf("COMANDO SELECT\n");
					break;
				case CMD_INSERT:
					printf("COMANDO INSERT\n");
					break;
				case CMD_CREATE:
					printf("COMANDO CREATE\n");
					break;
				case CMD_DESCRIBE:
					printf("COMANDO DESCRIBE\n");
					break;
				case CMD_DROP:
					printf("COMANDO DROP\n");
					break;
				case CMD_JOURNAL:
					printf("COMANDO JOURNAL\n");
					break;
				case CMD_NOENCONTRADO:default:
					log_error(loggerMemoria, "No se reconoce el comando.");
					break;
			}
		}

		free(linea);
		hacerFreeArray(operacion);
		free(operacion);
	}
	return NULL;
}

