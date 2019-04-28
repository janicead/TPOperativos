#include "memoria_commons.h"

void configure_logger_memoria(){
	loggerMemoria = log_create("memoria.log","memoria",1,LOG_LEVEL_INFO);
}

void exit_gracefully(int exitInfo){
	log_destroy(loggerMemoria);
	config_destroy(archivoConfigMemoria);
	exit(exitInfo);
}
