#include "configLFS.h"

int leerConfigLFS(){

	configure_logger_lfs();

	archivoConfigLFS = config_create(PATH_LFS_CONFIG);

	log_info(loggerLFS, "leyendo archivo de configuracion...");

	if (config_has_property(archivoConfigLFS, "PUERTO_ESCUCHA")) {
		configLFS.puerto_escucha = config_get_int_value(archivoConfigLFS,"PUERTO_ESCUCHA");
	} else {
		log_error(loggerLFS,"No se encontro la key PUERTO_ESCUCHA en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigLFS, "PUNTO_MONTAJE")) {
		configLFS.punto_montaje = config_get_string_value(archivoConfigLFS,"PUNTO_MONTAJE");
	} else {
		log_error(loggerLFS,"No se encontro la key PUNTO_MONTAJE en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigLFS, "RETARDO")) {
		configLFS.retardo = config_get_int_value(archivoConfigLFS,"RETARDO");
	} else {
		log_error(loggerLFS,"No se encontro la key RETARDO en el archivo de configuracion");
		return -1;
	}
	if (config_has_property(archivoConfigLFS, "TAMAÑO_VALUE")) {
		configLFS.tamanio_value = config_get_int_value(archivoConfigLFS,"TAMAÑO_VALUE");
	} else {
		log_error(loggerLFS,"No se encontro la key TAMAÑO_VALUE en el archivo de configuracion");
		return -1;
	}

	if (config_has_property(archivoConfigLFS, "TIEMPO_DUMP")) {
		configLFS.tiempo_dump = config_get_int_value(archivoConfigLFS,"TIEMPO_DUMP");
	} else {
		log_error(loggerLFS,"No se encontro la key TIEMPO_DUMP en el archivo de configuracion");
		return -1;
	}
	return 1;
}


void verificarArchivoConfig(){
	if(leerConfigLFS() == -1){
		exit_gracefully(EXIT_FAILURE);
		return;
	}
	return;
}

void mostrarDatosArchivoConfig(){
	printf("\n%s %d\n","PUERTO_ESCUCHA:",configLFS.puerto_escucha);
	printf("%s %s\n","PUNTO_MONTAJE: ",configLFS.punto_montaje);
	printf("%s %d\n","RETARDO: ",configLFS.retardo);
	printf("%s %d\n","TAMAÑO_VALUE: ",configLFS.tamanio_value);
	printf("%s %d\n","TIEMPO_DUMP: ",configLFS.tiempo_dump);
	return;
}




