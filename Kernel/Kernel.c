#include "Kernel.h"

int main(void) {
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
