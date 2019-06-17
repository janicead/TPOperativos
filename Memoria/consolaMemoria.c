#include "consolaMemoria.h"
#include <inttypes.h>

void* crearConsolaMemoria(){
	char *linea;
	char **operacion;

	while(1){
		linea = readline(">");

		if(linea){
			add_history(linea);
		}
		operacion = string_split(linea," ");
		uint16_t * key = malloc(sizeof(uint16_t));


		if(string_equals_ignore_case(operacion[0],"INSERT")){

			int tamanio = tamanioArray(operacion);
			char*lineadup = string_new();
			if(tamanio<5){
				linea = strdup(operacion[0]);
				free(lineadup);
			}else {
			char* value= armarValue(operacion);
			char* stringFinal= string_new();
			string_append(&stringFinal, operacion[0]);
			string_append(&stringFinal," ");
			string_append(&stringFinal,operacion[1]);
			string_append(&stringFinal," ");
			string_append(&stringFinal,operacion[2]);
			string_append(&stringFinal," ");
			string_append(&stringFinal, value);
			string_append(&stringFinal," ");
			string_append(&stringFinal, operacion[tamanio-1]);
			hacerFreeArray(operacion);
			free(operacion);
			operacion= string_split(stringFinal, " ");
			int tamanio2 = tamanioArray(operacion);

			for(int i = 0; i <tamanio2; i++){
				string_append(&lineadup, operacion[i]);
				if(i!=tamanio2-1){
				string_append(&lineadup, " ");
				}
			}
			free(linea);
			linea = strdup(lineadup);
			free(value);
			free(stringFinal);
			free(lineadup);
			}
		}

		if(validarComando(linea, loggerMemoria)){
			switch(comandoDeString(operacion[0])){
				case CMD_SELECT:
					if (pasarAUint16(operacion[2], key)){
					char* valor = SELECTMemoria(operacion[1], *key, 0);
					printf("Respuesta SELECT: '%s' \n", valor);
					}
					mostrarElementosMemoriaPrincipal();
					mostrarElementosTablaSegmentos();
					mostrarDatosMarcos();
					break;
				case CMD_INSERT:
					printf("");
					int timestamp = atoi(operacion[4]);
					if (pasarAUint16(operacion[2], key)){
						char* value = quitarEspacioFalso(operacion[3]);
						INSERTMemoria(operacion[1], *key, value, timestamp);

					}
					mostrarElementosMemoriaPrincipal(memoriaPrincipal);
					mostrarElementosTablaSegmentos();
					mostrarDatosMarcos();
					printf("COMANDO INSERT\n");

					break;
				case CMD_CREATE:
					printf("COMANDO CREATE\n");
					break;
				case CMD_DESCRIBE:
					printf("COMANDO DESCRIBE\n");
					break;
				case CMD_DROP:
					/*
					DROPMemoria(operacion[1],memoriaPrincipal);
					printf("COMANDO DROP\n");

*/
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
		free(key);
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
			free(ptr);
			return i;
		}
	}
	free(ptr);
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
		hacerFreeArray(valuearray);
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
	char** a = string_split(operacionFinal, "\"");
	free(operacionFinal);
	char* v = strdup(a[0]);
	hacerFreeArray(a);
	free(a);
	return v;
	}
}


char* quitarComillas(char* valor){
	return string_split(valor, "\"")[0];
}






