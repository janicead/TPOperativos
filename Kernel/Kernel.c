#include "Kernel.h"

int main(int argc, char* argv[]) {
	if(argc == 1){
		puts("Error no ingresó ningún archivo de configuración");
		return EXIT_FAILURE;
	}
	if(argc > 2){
		puts("ERROR ingresó más de un parámetro");
		return EXIT_FAILURE;
	}
	setear_path_config(argv[1]);
	iniciar();
	crear_hilos_iniciales();
	exit_gracefully(EXIT_SUCCESS);
}

void iniciar(){
	configure_logger_kernel();
	iniciar_semaforos();
	inicializarIds();
	srandom(time(NULL));
	crear_colas();
	crear_listas();
	verificarArchivoConfigKernel();
	tablaDeGossipKernel = list_create();
	memoriasALasQueMeConecte= list_create();
	conectarAMemoria(configKernel.ip_memoria,configKernel.puerto_memoria);
	t_memoria* mem = list_get(memorias,0);
	char* resp = opDESCRIBE(mem->socket_mem,"");
	describe_global(resp,true);
	free(resp);
	return;
}

void crear_hilos_iniciales(){
	pthread_create(&consola,NULL,setConsole,NULL);
	pthread_create(&timer_thread,NULL,metrics_timer,NULL);
	pthread_detach(timer_thread);
	pthread_create(&config_observer,NULL,observer_config,NULL);
	pthread_detach(config_observer);
	pthread_create(&gossipKernel,NULL,(void*)gossipDeKernel,NULL);
	pthread_detach(gossipKernel);
	pthread_create(&metadata_refresh,NULL,refresh_metadata_timer,NULL);
	pthread_detach(metadata_refresh);
	pthread_join(consola,NULL);
	return;
}

void setear_path_config(char* path){
	PATH_KERNEL_CONFIG = (char*)malloc(strlen(path+1));
	strcpy(PATH_KERNEL_CONFIG,path);
}
