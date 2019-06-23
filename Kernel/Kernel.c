#include "Kernel.h"

void pruebaParser(){
	crear_lql_run("/home/utnso/workspace/tp-2019-1c-BEFGN/prueba.lql");
	crear_lql_run("/home/utnso/workspace/tp-2019-1c-BEFGN/falla.lql");
	crear_lql_run("/home/utnso/workspace/tp-2019-1c-BEFGN/prueba2.lql");
	return;
}

void iniciarValoresParaTest(){
	t_tabla* tabla = (t_tabla*) malloc(sizeof(t_tabla));
	tabla->consistencia = "SC";
	tabla->nombre_tabla = "prueba";
	agregar_tabla(tabla);
	t_tabla* tabla2 = (t_tabla*) malloc(sizeof(t_tabla));
	tabla2->consistencia = "SC";
	tabla2->nombre_tabla = "personajes";
	agregar_tabla(tabla2);
	t_tabla* tabla3 = (t_tabla*) malloc(sizeof(t_tabla));
	tabla3->consistencia = "EC";
	tabla3->nombre_tabla = "Hash";
	agregar_tabla(tabla3);
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
	}
}

int main(void) {
	iniciar();
	crear_hilos_iniciales();
	iniciarValoresParaTest();
	//conectarAMemoria();
	pruebaParser();

	//CONEXION A MEMORIAS
	pthread_t recibirMemoriasYConectarme;
	tablaDeGossip =  list_create();
	memoriasALasQueMeConecte =  list_create();

	conectarmeAMP();
	pthread_create(&recibirMemoriasYConectarme, NULL,(void*) recibirMemorias, NULL);
	pthread_detach(recibirMemoriasYConectarme);
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
	//puertoMemoria = int_to_string(configKernel.puerto_memoria);
	return;
}

void crear_hilos_iniciales(){
	pthread_create(&consola,NULL,setConsole,NULL);
	pthread_create(&timer_thread,NULL,timer,NULL);
	pthread_detach(timer_thread);
	pthread_create(&config_observer,NULL,observer_config,NULL);
	pthread_detach(config_observer);
	return;
}

void conectarAMemoria(){
	//char * buffer[1024];
	//int socketMemoria = conectarAlServidor(configKernel.ip_memoria,puertoMemoria, loggerKernel);
	//send(socketMemoria,"Hola Memoria",strlen("Hola Memoria"),0);
	//int bytes = recv(socketMemoria,buffer,1024,0);
	//buffer[bytes] = '\0';
	//printf("%s%",buffer);
}
