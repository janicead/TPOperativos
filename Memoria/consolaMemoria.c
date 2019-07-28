#include "consolaMemoria.h"


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
			log_error(loggerMemoria, "Soy la MEMORIA '%d'.", configMemoria.numeroDeMemoria);
		}
		else if(string_equals_ignore_case(linea, "BORRAR TODO")){
			borrarTodo();
		}
		else{
			if(string_equals_ignore_case(operacion[0],"INSERT")){
				int tamanio = tamanioArray((void**)operacion);
				char*lineadup = string_new();
				if(tamanio<5){
					linea = strdup(operacion[0]);
					free(lineadup);
				}
				else {
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
							char* valor  = SELECTMemoria(operacion[1], *key, 0);
							if(string_equals_ignore_case(valor, "NO_EXISTE_TABLA")){
								log_error(loggerMemoria, "La tabla '%s' no existe.", operacion[1]);
								free(valor);
							}
							else if(string_equals_ignore_case(valor, "NO_EXISTE_KEY")){
								log_error(loggerMemoria, "La key '%s' de la tabla '%s', no existe.", operacion[2], operacion[1]);
								free(valor);
							}
							else if(string_equals_ignore_case(valor,"LFS_CAIDO")){
								log_error(loggerMemoria, "Se desconecto el LFS.");
							}
							else if(string_equals_ignore_case(valor,"FULL")){
								log_error(loggerMemoria, "La MEMORIA esta FULL, debe hacer JOURNAL y reingresar la request.");
							}
							else{
								log_info(loggerMemoria, "La respuesta de la request SELECT %s %s -> %s", operacion[1], operacion[2], valor);
								free(valor);
							}
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
						printf("\n");
						int nroParticiones = atoi(operacion[3]);
						int compactionTime = atoi(operacion[4]);
						char* valor = CREATEMemoria(operacion[1], operacion[2], nroParticiones, compactionTime);
						if(string_equals_ignore_case(valor, "YA_EXISTE_TABLA")){
							log_error(loggerMemoria, "La tabla '%s' ya existe.", operacion[1]);
						}else if(string_equals_ignore_case(valor, "NO_HAY_ESPACIO")){
							log_error(loggerMemoria, "En estos momentos no hay espacio suficiente.");
						}
						else if(string_equals_ignore_case(valor,"LFS_CAIDO")){
							log_error(loggerMemoria, "Se desconecto el LFS.");
						}
						else {
							log_info(loggerMemoria, "La tabla '%s' fue creada correctamente.", operacion[1]);
							free(valor);
						}

						break;
					case CMD_DESCRIBE:
						if(operacion[1]!=NULL){
							char* valor = DESCRIBEMemoria(operacion[1]);
							if(string_equals_ignore_case(valor, "NO_EXISTE_TABLA")){
								log_error(loggerMemoria, "La tabla '%s' ya existe.", operacion[1]);
								free(valor);
							}
							else if(string_equals_ignore_case(valor,"LFS_CAIDO")){
								log_error(loggerMemoria, "Se desconecto el LFS.");
							}
							else{
							char * buffer = respuestaDESCRIBEaPrintear(valor);
							printf("EL BUFFER %s \n", buffer);
							free(valor);
							free(buffer);
							}
						}
						else{
							char* valor = DESCRIBETodasLasTablasMemoria();
							if(string_equals_ignore_case(valor, "NO_EXISTEN_TABLAS")){
								log_error(loggerMemoria, "No existen tablas en este momento.");
								free(valor);
							}
							else if(string_equals_ignore_case(valor,"LFS_CAIDO")){
								log_error(loggerMemoria, "Se desconecto el LFS.");
							}
							else{
							char * buffer = respuestaDESCRIBEaPrintear(valor);
							printf("EL BUFFER %s \n", buffer);
							free(valor);
							free(buffer);
							}
						}
						break;
					case CMD_DROP:
						pthread_mutex_lock(&semMemoriaPrincipal);
						char* value = DROPMemoria(operacion[1]);
						pthread_mutex_lock(&semConfig);
						int retardoMemoriaPrincipal2 = configMemoria.retardoAccesoMemoriaPrincipal;
						pthread_mutex_unlock(&semConfig);
						sleep(retardoMemoriaPrincipal2);
						pthread_mutex_unlock(&semMemoriaPrincipal);
						if(string_equals_ignore_case(value, "NO_EXISTE_TABLA")){
							log_error(loggerMemoria,"La tabla '%s'no existe.", operacion[1] );
							free(value);
						}
						else if(string_equals_ignore_case(value,"LFS_CAIDO")){
							log_error(loggerMemoria, "Se desconecto el LFS.");
						}
						else{
							log_info(loggerMemoria, "Se ha borrado la tabla '%s'.", operacion[1]);
							free(value);
						}
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
		char* valor = quitarComillas(value);
		return valor;
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
