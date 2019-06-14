#include "consolaMemoria.h"
#include <inttypes.h>

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
		uint16_t * key;

		if(string_equals_ignore_case(operacion[0],"INSERT")){

			int tamanio = tamanioArray(operacion);
			char*lineadup = string_new();
			if(tamanio<5){
				linea = strdup(operacion[0]);
			}else {
			char* value= armarValue(operacion);
			operacion[3]= strdup(value);
			char* stringFinal= string_new();
			string_append(&stringFinal, operacion[0]);
			string_append(&stringFinal," ");
			string_append(&stringFinal,operacion[1]);
			string_append(&stringFinal," ");
			string_append(&stringFinal,operacion[2]);
			string_append(&stringFinal," ");
			string_append(&stringFinal, operacion[3]);
			string_append(&stringFinal," ");
			string_append(&stringFinal, operacion[tamanio-1]);

			operacion= string_split(stringFinal, " ");
			int tamanio2 = tamanioArray(operacion);

			for(int i = 0; i <tamanio2; i++){
				string_append(&lineadup, operacion[i]);
				string_append(&lineadup, " ");
			}

			linea = strdup(lineadup);
			}
		}

		if(validarComando(linea, loggerMemoria)){
			switch(comandoDeString(operacion[0])){
				case CMD_SELECT:
					if (pasarAUint16(operacion[2], key)){
					char* valor = SELECTMemoria(operacion[1], *key, memoriaPrincipal, 0);
					printf("Respuesta SELECT: '%s' \n", valor);
					}
					mostrarElementosMemoriaPrincipal(memoriaPrincipal);
					mostrarElementosTablaSegmentos();
					break;
				case CMD_INSERT:
					printf("");
					int timestamp = atoi(operacion[4]);
					if (pasarAUint16(operacion[2], key)){
						INSERTMemoria(operacion[1], *key, quitarEspacioFalso(operacion[3]), timestamp, memoriaPrincipal);
						printf("COMANDO INSERT\n");
					}
					mostrarElementosMemoriaPrincipal(memoriaPrincipal);
					mostrarElementosTablaSegmentos();

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
bool pasarAUint16(const char *str, uint16_t *res) {
    char *end;
    int errno = 0;
    long val = strtol(str, &end, 10);
    if (errno || end == str || *end != '\0' || val < 0 || val >= 0x10000) {
        return false;
    }
    *res = (uint16_t)val;
    return true;
}

int buscarFinalValue(char** value){
	char c = '"';
	char *ptr = malloc(2*sizeof(char));
	ptr[0] = c;
	ptr[1] = '\0';

	int cantElementos= tamanioArray(value);
	for(int i = 4; i< cantElementos; i++){
		if(string_ends_with(value[i], ptr)){
			return i;
		}
	}
	return 0;
}

char* armarValue(char** value){
	int ultimaPosicion = buscarFinalValue(value);
	char* operacionFinal = strdup(value[3]);
	for( int i = 4 ; i <= ultimaPosicion; i ++){
		string_append(&operacionFinal, ";");
		string_append(&operacionFinal, value[i]);

	}
	return operacionFinal;
}
char* quitarEspacioFalso(char* value){
	char* operacionFinal = string_new();
	char** valuearray = string_split(value, ";");
	int tamanio = tamanioArray(valuearray);
	if(tamanio== 0){
		free(operacionFinal);
		free(valuearray);
		return quitarComillas(value);
	}else {
	for(int i = 0; i < tamanio; i ++){
		string_append(&operacionFinal, valuearray[i]);
		if(i!=tamanio-1){
		string_append(&operacionFinal, " ");}

	}
	hacerFreeArray(valuearray);
	free(valuearray);
	return quitarComillas(operacionFinal);
	}
}


char* quitarComillas(char* valor){
	return string_split(valor, "\"")[0];
}






