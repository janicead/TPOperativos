#include "Kernel.h"

void pruebaParser(){
	crear_lql_run("/home/utnso/workspace/tp-2019-1c-BEFGN/prueba.lql");
	crear_lql_run("/home/utnso/workspace/tp-2019-1c-BEFGN/falla.lql");
	crear_lql_run("/home/utnso/workspace/tp-2019-1c-BEFGN/prueba2.lql");
	return;
}

void iniciarValoresParaTest(){
	t_tabla* tabla = (t_tabla*) malloc(sizeof(t_tabla));
	tabla->consistencia = "SHC";
	tabla->nombre_tabla = "hash";
	agregar_tabla(tabla);
	t_tabla* tabla2 = (t_tabla*) malloc(sizeof(t_tabla));
	tabla2->consistencia = "SC";
	tabla2->nombre_tabla = "personajes";
	agregar_tabla(tabla2);
	t_tabla* tabla3 = (t_tabla*) malloc(sizeof(t_tabla));
	tabla3->consistencia = "EC";
	tabla3->nombre_tabla = "random";
	agregar_tabla(tabla3);
	/*crear_lql_add("sc",1);
	agregar_memoria(5,"1",0);
	agregar_memoria(6,"2",1);
	agregar_memoria(7,"3",2);
	agregar_memoria(8,"4",3);
	agregar_memoria(8,"4",4);
	crear_lql_add("sc",0);
	crear_lql_add("shc",1);
	crear_lql_add("shc",2);
	crear_lql_add("shc",3);
	crear_lql_add("ec",1);
	crear_lql_add("ec",2);
	crear_lql_add("ec",3);
	while(queue_is_empty(queue_ready)){
		ejecutar();
	}*/
}

int main(void) {
	iniciar();
	crear_hilos_iniciales();
	iniciarValoresParaTest();
	//pruebaParser();
	pthread_join(consola,NULL);
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
	tablaDeGossip = list_create();

	int t = tamanioArray((void**)configKernel.puertosDeSeeds);
	conectarAMemoria(configKernel.ip_memoria,configKernel.puerto_memoria);
	/*for(int i= 0; i< t; i ++){
		printf("Ip de seeds %d es %s\n", i, configKernel.ipDeSeeds[i]);
		printf("Ip de seeds %d es %d\n", i, configKernel.puertosDeSeeds[i]);
		conectarAMemoria(configKernel.ipDeSeeds[i],configKernel.puertosDeSeeds[i]);
	}*/
	t_memoria* mem = list_get(memorias,0);
	char* resp = opDESCRIBE(mem->socket_mem,"");
	puts(resp);
	free(resp);
	return;
}

void crear_hilos_iniciales(){
	pthread_create(&consola,NULL,setConsole,NULL);
	pthread_create(&timer_thread,NULL,metrics_timer,NULL);
	pthread_detach(timer_thread);
	pthread_create(&config_observer,NULL,observer_config,NULL);
	pthread_detach(config_observer);
	//pthread_create(&metadata_refresh,NULL,refresh_metadata_timer,NULL);
	//pthread_detach(metadata_refresh);
	return;
}
