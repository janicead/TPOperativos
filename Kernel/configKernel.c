#include "configKernel.h"

int leerConfigKernel(){

	configure_logger_kernel();

	archivoConfigKernel = config_create(PATH_KERNEL_CONFIG);

	log_info(loggerKernel, "leyendo archivo de configuracion...");

	if (config_has_property(archivoConfigKernel, "IP_MEMORIA")) {
		configKernel.ip_memoria = config_get_string_value(archivoConfigKernel,"IP_MEMORIA");
	} else {
		log_error(loggerKernel,"No se encontro la key IP_MEMORIA en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigKernel, "PUERTO_MEMORIA")) {
		configKernel.puerto_memoria = config_get_int_value(archivoConfigKernel,"PUERTO_MEMORIA");
	} else {
		log_error(loggerKernel,"No se encontro la key PUERTO_MEMORIA en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigKernel, "QUANTUM")) {
		configKernel.quantum = config_get_int_value(archivoConfigKernel,"QUANTUM");
	} else {
		log_error(loggerKernel,"No se encontro la key QUANTUM en el archivo de configuracion");
		return -1;
	}
	if (config_has_property(archivoConfigKernel, "MULTIPROCESAMIENTO")) {
		configKernel.multiprocesamiento = config_get_int_value(archivoConfigKernel,"MULTIPROCESAMIENTO");
	} else {
		log_error(loggerKernel,"No se encontro la key MULTIPROCESAMIENTO en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigKernel, "METADATA_REFRESH")) {
		configKernel.metadata_refresh = config_get_int_value(archivoConfigKernel,"METADATA_REFRESH");
	} else {
		log_error(loggerKernel,"No se encontro la key METADATA_REFRESH en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigKernel, "SLEEP_EJECUCION")) {
		configKernel.sleep_execution = config_get_int_value(archivoConfigKernel,"SLEEP_EJECUCION");
	} else {
		log_error(loggerKernel,"No se encontro la key SLEEP_EJECUCION en el archivo de configuracion");
		return -1;
	}
	return 1;
}


void verificarArchivoConfigKernel(){
	if(leerConfigKernel() == -1){
		exit_gracefully(EXIT_FAILURE);
		return;
	}
	return;
}

void mostrarDatosArchivoConfigKernel(){
	printf("\n%s %s\n","IP_MEMORIA:",configKernel.ip_memoria);
	printf("%s %d\n","PUNTO_MEMORIA: ",configKernel.puerto_memoria);
	printf("%s %d\n","QUANTUM: ",configKernel.quantum);
	printf("%s %d\n","MULTIPROCESAMIENTO: ",configKernel.multiprocesamiento);
	printf("%s %d\n","METADATA_REFRESH: ",configKernel.metadata_refresh);
	printf("%s %d\n","SLEEP_EJECUCION: ",configKernel.sleep_execution);
	return;
}
