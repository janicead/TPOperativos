#include "configMemoria.h"

int leerConfigMemoria(){

	configure_logger_memoria();

	archivoConfigMemoria = config_create(PATH_MEMORIA_CONFIG);

	log_info(loggerMemoria, "leyendo archivo de configuracion...");

	if (config_has_property(archivoConfigMemoria, "PUERTO")) {
		configMemoria.puerto = config_get_int_value(archivoConfigMemoria,"PUERTO");
	} else {
		log_error(loggerMemoria,"No se encontro la key PUERTO en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigMemoria, "IP_FS")) {
		configMemoria.ip_fs = config_get_string_value(archivoConfigMemoria,"IP_FS");
	} else {
		log_error(loggerMemoria,"No se encontro la key IP_FS en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigMemoria, "PUERTO_FS")) {
		configMemoria.puerto_fs = config_get_int_value(archivoConfigMemoria,"PUERTO_FS");
	} else {
		log_error(loggerMemoria,"No se encontro la key PUERTO_FS en el archivo de configuracion");
		return -1;
	}
	if (config_has_property(archivoConfigMemoria, "IP_SEEDS")) {
		configMemoria.ip_seeds = config_get_array_value(archivoConfigMemoria,"IP_SEEDS");
	} else {
		log_error(loggerMemoria,"No se encontro la key IP_SEEDS en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigMemoria, "PUERTO_SEEDS")) {
		configMemoria.puerto_seeds = config_get_array_value(archivoConfigMemoria,"PUERTO_SEEDS");
	} else {
		log_error(loggerMemoria,"No se encontro la key PUERTO_SEEDS en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigMemoria, "RETARDO_MEM")) {
		configMemoria.retardo_mem = config_get_int_value(archivoConfigMemoria,"RETARDO_MEM");
	} else {
		log_error(loggerMemoria,"No se encontro la key RETARDO_MEM en el archivo de configuracion");
		return -1;
	}
	if (config_has_property(archivoConfigMemoria, "RETARDO_FS")) {
		configMemoria.retardo_fs = config_get_int_value(archivoConfigMemoria,"RETARDO_FS");
	} else {
		log_error(loggerMemoria,"No se encontro la key RETARDO_FS en el archivo de configuracion");
		return -1;
	}
	if (config_has_property(archivoConfigMemoria, "TAM_MEM")) {
		configMemoria.tam_mem = config_get_int_value(archivoConfigMemoria,"TAM_MEM");
	} else {
		log_error(loggerMemoria,"No se encontro la key TAM_MEM en el archivo de configuracion");
		return -1;
	}
	if (config_has_property(archivoConfigMemoria, "RETARDO_JOURNAL")) {
		configMemoria.retardo_journal = config_get_int_value(archivoConfigMemoria,"RETARDO_JOURNAL");
	} else {
		log_error(loggerMemoria,"No se encontro la key RETARDO_JOURNAL en el archivo de configuracion");
		return -1;
	}
	if (config_has_property(archivoConfigMemoria, "RETARDO_GOSSIPING")) {
		configMemoria.retardo_gossiping = config_get_int_value(archivoConfigMemoria,"RETARDO_GOSSIPING");
	} else {
		log_error(loggerMemoria,"No se encontro la key RETARDO_GOSSIPING en el archivo de configuracion");
		return -1;
	}
	if (config_has_property(archivoConfigMemoria, "MEMORY_NUMBER")) {
		configMemoria.memory_number = config_get_int_value(archivoConfigMemoria,"MEMORY_NUMBER");
	} else {
		log_error(loggerMemoria,"No se encontro la key MEMORY_NUMBER en el archivo de configuracion");
		return -1;
	}
	return 1;
}

void freeDatosConfigMemoria(){
    string_iterate_lines(configMemoria.puerto_seeds, (void*) free);
	free(configMemoria.puerto_seeds);
    string_iterate_lines(configMemoria.ip_seeds, (void*) free);
	free(configMemoria.ip_seeds);
}

void verificarArchivoConfigMemoria(){
	if(leerConfigMemoria() == -1){
		exit_gracefully(EXIT_FAILURE);
		return;
	}
	return;
}

void mostrarDatosArchivoConfigMemoria(){
	int i = 0, o = 0;
	printf("\n%s %d\n","PUERTO:",configMemoria.puerto);
	printf("%s %s\n","IP_FS: ",configMemoria.ip_fs);
	printf("%s %d\n","PUERTO_FS:",configMemoria.puerto_fs);
	printf("%s","IP_SEEDS: ");
	while(configMemoria.ip_seeds[i] != '\0'){
		printf("%s \t",configMemoria.ip_seeds[i]);
		i++;
	}
	printf("\n%s","PUERTO_SEEDS: ");
	while(configMemoria.puerto_seeds[o] != '\0'){
		printf("%s \t",configMemoria.puerto_seeds[o]);
		o++;
	}
	printf("\n%s %d\n","RETARDO_MEM: ",configMemoria.retardo_mem);
	printf("%s %d\n","RETARDO_FS: ",configMemoria.retardo_fs);
	printf("%s %d\n","TAM_MEM:",configMemoria.tam_mem);
	printf("%s %d\n","RETARDO_JOURNAL: ",configMemoria.retardo_journal);
	printf("%s %d\n","RETARDO_GOSSIPING: ",configMemoria.retardo_gossiping);
	printf("%s %d\n","MEMORY_NUMBER: ",configMemoria.memory_number);
	return;
}
