#include "configMemoria.h"


t_ConfigMemoria leerConfigMemoria(){
	archivoConfigMemoria = config_create(configMemoriaDir);
	log_info(loggerMemoria, "Leyendo archivo de configuracion...");

	if (config_has_property(archivoConfigMemoria, "PUERTO_DE_ESCUCHA")) {
		configMemoria.puertoDeEscucha = config_get_int_value(archivoConfigMemoria,"PUERTO_DE_ESCUCHA");
	} else {
		log_error(loggerMemoria,"No se encontro la key PUERTO_DE_ESCUCHA en el archivo de configuracion");
		exit(EXIT_FAILURE);
	}
	if (config_has_property(archivoConfigMemoria, "IP_DE_ESCUCHA")) {
		configMemoria.ipDeEscucha = malloc(1+strlen(config_get_string_value(archivoConfigMemoria, "IP_DE_ESCUCHA")));
		strcpy(configMemoria.ipDeEscucha,config_get_string_value(archivoConfigMemoria,"IP_DE_ESCUCHA"));
	} else {
		log_error(loggerMemoria,"No se encontro la key IP_DE_ESCUCHA en el archivo de configuracion");
		exit(EXIT_FAILURE);
	}
	if (config_has_property(archivoConfigMemoria, "IP_DEL_FILE_SYSTEM")) {
		configMemoria.ipDelFileSystem = malloc(1+strlen(config_get_string_value(archivoConfigMemoria, "IP_DEL_FILE_SYSTEM")));
		strcpy(configMemoria.ipDelFileSystem,config_get_string_value(archivoConfigMemoria,"IP_DEL_FILE_SYSTEM"));
	} else {
		log_error(loggerMemoria,"No se encontro la key IP_DEL_FILE_SYSTEM en el archivo de configuracion");
		exit(EXIT_FAILURE);
	}
	if (config_has_property(archivoConfigMemoria, "PUERTO_DEL_FILE_SYSTEM")) {
		configMemoria.puertoDelFileSystem = config_get_int_value(archivoConfigMemoria,"PUERTO_DEL_FILE_SYSTEM");
	} else {
		log_error(loggerMemoria,"No se encontro la key PUERTO_DEL_FILE_SYSTEM en el archivo de configuracion");
		exit(EXIT_FAILURE);
		}
	if (config_has_property(archivoConfigMemoria, "IP_DE_SEEDS")) {
		configMemoria.ipDeSeeds = config_get_array_value(archivoConfigMemoria,"IP_DE_SEEDS");
	} else {
		log_error(loggerMemoria,"No se encontro la key IP_DE_SEEDS en el archivo de configuracion");
		exit(EXIT_FAILURE);
	}
	if (config_has_property(archivoConfigMemoria, "PUERTOS_DE_SEEDS")) {
		char** arrayPuertos= config_get_array_value(archivoConfigMemoria,"PUERTOS_DE_SEEDS");

		int cantPuertosSeeds = tamanioArray((void**)arrayPuertos);
		char* ar [cantPuertosSeeds+1];
		int j =0;
		while(j<cantPuertosSeeds && arrayPuertos[j]!=NULL){
			ar[j]= arrayPuertos[j];
			j++;
		}
		for(int i =0; i<cantPuertosSeeds; i++){
			int a = atoi(ar[i]);
			configMemoria.puertosDeSeeds[i]= a;
		}
		hacerFreeArray((void**)arrayPuertos);
		free(arrayPuertos);
	} else {
		log_error(loggerMemoria,"No se encontro la key PUERTOS_DE_SEEDS en el archivo de configuracion");
		exit(EXIT_FAILURE);
	}
	if (config_has_property(archivoConfigMemoria, "RETARDO_ACCESO_MEMORIA_PRINCIPAL")) {
		configMemoria.retardoAccesoMemoriaPrincipal = config_get_int_value(archivoConfigMemoria,"RETARDO_ACCESO_MEMORIA_PRINCIPAL");
	} else {
		log_error(loggerMemoria,"No se encontro la key RETARDO_ACCESO_MEMORIA_PRINCIPAL en el archivo de configuracion");
		exit(EXIT_FAILURE);
	}
	if (config_has_property(archivoConfigMemoria, "RETARDO_ACCESO_FILE_SYSTEM")) {
			configMemoria.retardoAccesoFileSystem = config_get_int_value(archivoConfigMemoria,"RETARDO_ACCESO_FILE_SYSTEM");
		} else {
			log_error(loggerMemoria,"No se encontro la key RETARDO_ACCESO_FILE_SYSTEM en el archivo de configuracion");
			exit(EXIT_FAILURE);
		}
	if (config_has_property(archivoConfigMemoria, "TAMANIO_MEMORIA")) {
			configMemoria.tamanioMemoria = config_get_int_value(archivoConfigMemoria,"TAMANIO_MEMORIA");
		} else {
			log_error(loggerMemoria,"No se encontro la key TAMANIO_MEMORIA en el archivo de configuracion");
			exit(EXIT_FAILURE);
		}
	if (config_has_property(archivoConfigMemoria, "TIEMPO_JOURNAL")) {
			configMemoria.tiempoJournal = config_get_int_value(archivoConfigMemoria,"TIEMPO_JOURNAL");
		} else {
			log_error(loggerMemoria,"No se encontro la key TIEMPO_JOURNAL en el archivo de configuracion");
			exit(EXIT_FAILURE);
		}
	if (config_has_property(archivoConfigMemoria, "TIEMPO_GOSSIPING")) {
			configMemoria.tiempoGossiping = config_get_int_value(archivoConfigMemoria,"TIEMPO_GOSSIPING");
		} else {
			log_error(loggerMemoria,"No se encontro la key TIEMPO_GOSSIPING en el archivo de configuracion");
			exit(EXIT_FAILURE);
		}
	if (config_has_property(archivoConfigMemoria, "NUMERO_DE_MEMORIA")) {
			configMemoria.numeroDeMemoria = config_get_int_value(archivoConfigMemoria,"NUMERO_DE_MEMORIA");
		} else {
			log_error(loggerMemoria,"No se encontro la key NUMERO_DE_MEMORIA en el archivo de configuracion");
			exit(EXIT_FAILURE);
		}
	config_destroy(archivoConfigMemoria);
	return configMemoria;
}
void mostrarDatosArchivoConfig(){
	printf("\n%s %d\n","PUERTO_DE_ESCUCHA: ",configMemoria.puertoDeEscucha);
	printf("%s %s\n","IP_DE_ESCUCHA: ",configMemoria.ipDeEscucha);
	printf("%s %s\n","IP_DEL_FILE_SYSTEM: ",configMemoria.ipDelFileSystem);
	printf("%s %d\n","PUERTO_DEL_FILE_SYSTEM: ",configMemoria.puertoDelFileSystem);
	printf("%s","IP_DE_SEEDS:\n" );
	printearArrayDeChars(configMemoria.ipDeSeeds);
	printf("%s","PUERTOS_DE_SEEDS:\n");
	printearArrayDeInts(configMemoria.puertosDeSeeds);
	printf("%s %d\n","RETARDO_ACCESO_MEMORIA_PRINCIPAL: ",configMemoria.retardoAccesoMemoriaPrincipal);
	printf("%s %d\n","RETARDO_ACCESO_FILE_SYSTEM: ",configMemoria.retardoAccesoFileSystem);
	printf("%s %d\n","TAMANIO_MEMORIA: ",configMemoria.tamanioMemoria);
	printf("%s %d\n","TIEMPO_JOURNAL: ",configMemoria.tiempoJournal);
	printf("%s %d\n","TIEMPO_GOSSIPING: ",configMemoria.tiempoGossiping);
	printf("%s %d\n","NUMERO_DE_MEMORIA: ",configMemoria.numeroDeMemoria);
	return;
}
void configurarLoggerMemoria(){
	FILE * memorialog =fopen("/home/utnso/workspace/tp-2019-1c-BEFGN/Memoria/Config/memoria.log","w");
	loggerMemoria = log_create("/home/utnso/workspace/tp-2019-1c-BEFGN/Memoria/Config/memoria.log","memoria",1,LOG_LEVEL_INFO);
	fclose(memorialog);
}

void exit_gracefully(int exitInfo){
	config_destroy(archivoConfigMemoria);
	log_destroy(loggerMemoria);
	exit(exitInfo);
}

void* observer_config(){
	int file_descriptor,file_observer;
	int event_size = sizeof(struct inotify_event);
	int max_cant_cambios = 1024;
	int max_file_name_size = 24;
	int buffer_size = max_cant_cambios* (event_size + max_file_name_size);
	char buffer[buffer_size];

	file_descriptor = inotify_init();
	file_observer = inotify_add_watch(file_descriptor,configMemoriaDir,IN_MODIFY);
	pthread_mutex_init(&semConfig, NULL);
	while(1){
		read(file_descriptor,buffer,buffer_size);
		pthread_mutex_lock(&semConfig);
		actualizarArchivoConfig();
		pthread_mutex_unlock(&semConfig);
	}

	inotify_rm_watch(file_descriptor,file_observer);
	close(file_descriptor);
	return NULL;
}

void actualizarArchivoConfig(){
	archivoConfigMemoria = config_create(configMemoriaDir);
	if(config_has_property(archivoConfigMemoria, "RETARDO_ACCESO_MEMORIA_PRINCIPAL")) {
		configMemoria.retardoAccesoMemoriaPrincipal = config_get_int_value(archivoConfigMemoria,"RETARDO_ACCESO_MEMORIA_PRINCIPAL");
	}
	else{
		log_error(loggerMemoria,"No se encontro la key RETARDO_ACCESO_MEMORIA_PRINCIPAL en el archivo de configuracion");
		return;
	}
	if(config_has_property(archivoConfigMemoria, "RETARDO_ACCESO_FILE_SYSTEM")) {
		configMemoria.retardoAccesoFileSystem = config_get_int_value(archivoConfigMemoria,"RETARDO_ACCESO_FILE_SYSTEM");
	}
	else{
		log_error(loggerMemoria,"No se encontro la key RETARDO_ACCESO_FILE_SYSTEM en el archivo de configuracion");
		return;
	}
	config_destroy(archivoConfigMemoria);
	mostrarDatosArchivoConfig();
}
