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

		if(string_is_empty(linea)){
			log_error(loggerMemoria, "No se ingresó ningún comando.");
		}

		else if(string_equals_ignore_case(operacion[0],"INSERT")){

			int tamanio = tamanioArray((void**)operacion);
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
			hacerFreeArray((void**)operacion);
			free(operacion);
			operacion= string_split(stringFinal, " ");
			int tamanio2 = tamanioArray((void**)operacion);

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
					if (pasarAUint16(operacion[2], key)){
						char *ptr;
						unsigned long int timestamp = strtoul(operacion[4], &ptr, 10);
						char* value = quitarEspacioFalsoMemoria(operacion[3]);
						pthread_mutex_lock(&semMemoriaPrincipal);
						INSERTMemoria(operacion[1], *key, value, timestamp);
						free(value);
						pthread_mutex_lock(&semConfig);
						int retardoMemoriaPrincipal1 = configMemoria.retardoAccesoMemoriaPrincipal;
						pthread_mutex_unlock(&semConfig);
						sleep(retardoMemoriaPrincipal1);
						pthread_mutex_unlock(&semMemoriaPrincipal);
					}
					mostrarElementosMemoriaPrincipal(memoriaPrincipal);
					mostrarElementosTablaSegmentos();
					mostrarDatosMarcos();
					break;
				case CMD_CREATE:
					printf("COMANDO CREATE\n");
					int nroParticiones = atoi(operacion[3]);
					int compactionTime = atoi(operacion[4]);
					CREATEMemoria(operacion[1], operacion[2], nroParticiones, compactionTime);
					break;
				case CMD_DESCRIBE:
					printf("COMANDO DESCRIBE\n");
					break;
				case CMD_DROP:
					pthread_mutex_lock(&semMemoriaPrincipal);
					char* value = DROPMemoria(operacion[1]);
					pthread_mutex_lock(&semConfig);
					int retardoMemoriaPrincipal2 = configMemoria.retardoAccesoMemoriaPrincipal;
					pthread_mutex_unlock(&semConfig);
					sleep(retardoMemoriaPrincipal2);
					pthread_mutex_unlock(&semMemoriaPrincipal);
					log_info(loggerMemoria,value);
					free(value);
					break;
				case CMD_JOURNAL:
					pthread_mutex_lock(&semMemoriaPrincipal);
					JOURNALMemoria();
					pthread_mutex_lock(&semConfig);
					int retardoMemoriaPrincipal3 = configMemoria.retardoAccesoMemoriaPrincipal;
					pthread_mutex_unlock(&semConfig);
					sleep(retardoMemoriaPrincipal3);
					pthread_mutex_unlock(&semMemoriaPrincipal);
					break;
				case CMD_NOENCONTRADO:default:
					log_error(loggerMemoria, "No se reconoce el comando.");
					break;
			}
		}

		free(linea);
		hacerFreeArray((void**)operacion);
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

char* quitarEspacioFalsoMemoria(char* value){
	char* operacionFinal = string_new();
	char** valuearray = string_split(value, ";");
	int tamanio = tamanioArray((void**)valuearray);
	if(tamanio== 0){
		free(operacionFinal);
		hacerFreeArray((void**)valuearray);
		free(valuearray);
		return quitarComillas(value);
	}else {
	for(int i = 0; i < tamanio; i ++){
		string_append(&operacionFinal, valuearray[i]);
		if(i!=tamanio-1){
		string_append(&operacionFinal, " ");}

	}
	hacerFreeArray((void**)valuearray);
	free(valuearray);
	char** a = string_split(operacionFinal, "\"");
	free(operacionFinal);
	char* v =malloc(tamanioDadoPorLFS);
	strcpy(v, a[0]);
	hacerFreeArray((void**)a);
	free(a);
	return v;
	}
}

