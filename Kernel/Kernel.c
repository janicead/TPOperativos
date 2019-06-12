#include "Kernel.h"

void pruebaParser(){
	FILE* script = abrirArchivo("/home/utnso/workspace/tp-2019-1c-BEFGN/prueba.lql");
	FILE* script2 = abrirArchivo("/home/utnso/workspace/tp-2019-1c-BEFGN/prueba2.lql");
	lql_run(script2);
	lql_run(script);
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
	agregar_memoria(5);
	agregar_memoria(6);
	agregar_memoria(7);
	agregar_memoria(8);
	strong_consistency = obtener_memoria_por_id(0);
	strong_consistency->valida = true;
	list_add(strong_hash_consistency,obtener_memoria_por_id(1));
	list_add(strong_hash_consistency,obtener_memoria_por_id(2));
	list_add(strong_hash_consistency,obtener_memoria_por_id(3));
	list_add(eventual_consistency,obtener_memoria_por_id(1));
	list_add(eventual_consistency,obtener_memoria_por_id(2));
	list_add(eventual_consistency,obtener_memoria_por_id(3));
}

int main(void) {
	iniciar();
	iniciarValoresParaTest();
	//conectarAMemoria();
	crear_hilos_iniciales();
	pruebaParser();
	pthread_join(consola,NULL);
	exit_gracefully(EXIT_SUCCESS);
}

void iniciar(){
	iniciar_semaforos();
	verificarArchivoConfigKernel();
	mostrarDatosArchivoConfigKernel();
	puertoMemoria = int_to_string(configKernel.puerto_memoria);
	inicializarIds();
	srandom(time(NULL));
	crear_colas();
	crear_listas();
	return;
}

void crear_hilos_iniciales(){
	pthread_create(&consola,NULL,setConsole,NULL);
	pthread_create(&timer_thread,NULL,timer,NULL);
	pthread_detach(timer_thread);
	hilos = malloc(configKernel.multiprocesamiento * sizeof(pthread_t));
	for(int i = 0; i<configKernel.multiprocesamiento; i++){
		pthread_create(&hilos[i],NULL,ejecutar,NULL);
		pthread_detach(hilos[i]);
	}
	return;
}

void conectarAMemoria(){
	char * buffer[1024];
	int socketMemoria = conectarAlServidor(configKernel.ip_memoria,puertoMemoria, loggerKernel);
	send(socketMemoria,"Hola Memoria",strlen("Hola Memoria"),0);
	int bytes = recv(socketMemoria,buffer,1024,0);
	buffer[bytes] = '\0';
	printf("%s%",buffer);
}
