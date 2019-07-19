#include "consola.h"


/* PARA SWITCHEAR */

typedef struct { char *key; int val; } t_comando;

static t_comando tablaComandos[] = {
    { "SELECT", CMD_SELECT },
	{ "INSERT", CMD_INSERT },
	{ "CREATE", CMD_CREATE },
	{ "DESCRIBE", CMD_DESCRIBE },
	{ "DROP", CMD_DROP },
	{ "JOURNAL", CMD_JOURNAL },
	{ "ADD", CMD_ADD },
	{ "RUN", CMD_RUN },
	{ "METRICS", CMD_METRICS },
	{ "EXIT", CMD_EXIT }
};


int comandoDeString(char *key)
{
    int i;
    for (i=0; i < 10; i++) {
        if (!strcasecmp(tablaComandos[i].key, key))
        	return tablaComandos[i].val;
    }
    return CMD_NOENCONTRADO;
}

int validarComando(char* linea, t_log* logger){
	char **operacion;
	int cantArgumentos, valido = 1;
	operacion = string_split(linea," ");
	cantArgumentos = longitudArrayDePunteros(operacion);
	if(strcmp(linea, "") == 0){
		valido = 0;
		log_error(logger, "Ingrese una operacion\n");
		return valido;
	}
	switch(comandoDeString(operacion[0])){
		case CMD_EXIT:
			if(cantArgumentos != 1){
				valido = 0;
				log_error(logger, "Esta operación no admite parámetros\n");
			}
			break;

		case CMD_SELECT:
			if(cantArgumentos != 3){
				log_error(logger, "Falta, o hay exceso, de parametros de SELECT\n");
				valido = 0;
			}
			break;
		case CMD_INSERT:
			if(cantArgumentos != 5){
				log_error(logger, "Falta, o hay exceso, de parametros de INSERT\n");
				valido = 0;
			}
			break;
		case CMD_CREATE:
			if(cantArgumentos != 5){
				log_error(logger, "Falta, o hay exceso, de parametros de CREATE\n");
				valido = 0;
			}
			break;

		case CMD_DESCRIBE:
			if(cantArgumentos > 3){
				log_error(logger, "Falta, o hay exceso, de parametros de DESCRIBE\n");
				valido = 0;
			}
			break;
		case CMD_DROP:
			if(cantArgumentos != 2){
				log_error(logger, "Falta, o hay exceso, de parametros de DROP\n");
				valido = 0;
			}
			break;
		case CMD_JOURNAL:
			if(cantArgumentos != 1){
				log_error(logger, "Hay exceso de parametros de JOURNAL\n");
				valido = 0;
			}
			break;
		case CMD_METRICS:
			if(cantArgumentos != 1){
				log_error(logger, "Hay exceso de parametros de METRICS\n");
				valido = 0;
			}
			break;
		case CMD_ADD:
			if(cantArgumentos != 5){
				log_error(logger, "Falta, o hay exceso, de parametros de ADD\n");
				valido = 0;
			}
			else if(verificarQueDigaPalabraEspecifica(linea,"MEMORY",1)!=0 ||verificarQueDigaPalabraEspecifica(linea,"TO",3)!=0 ){
				log_error(logger, "La sintaxis de ADD no es correcta\n");
				valido=0;
			}
			break;
		case CMD_RUN:
			if(cantArgumentos != 2){
				log_error(logger, "Falta, o hay exceso, de parametros de RUN\n");
				valido = 0;
			}
			break;
	}

	hacerFreeArray(operacion);
	free(operacion);
	return valido;
}


int chequearValoresNULL(char** valor, int cantElementos){
	int i= 0;
	for(i=0;i<cantElementos;i++){
		if (valor[i]==NULL){
			return 1;
		}
	}
	return 0;
}
char** obtenerParametros(char* linea, int cantElementos){
	char** parametros = string_n_split(linea,2," ");
	return parametros;
}


int verificarCantidadParametrosPasados(char* linea){
	char ** parametros = string_split(linea, " ");
	int tamanio = tamanioArray(parametros);
	for(int i = 0; i<tamanio; i++){
		free(parametros[i]);
	}
	free(parametros);
	return tamanio;
}

int verificarQueDigaPalabraEspecifica(char* linea, char* palabra, int posicion){
	char** parametros = string_split(linea, " ");
	if(strcmp(parametros[posicion],palabra)!=0){
		return 1;
	}
	return 0;
}

