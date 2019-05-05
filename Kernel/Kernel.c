#include "Kernel.h"

void pruebaParser(){
	FILE* script = abrirArchivo("/home/utnso/tp-2019-1c-BEFGN/prueba.lql");
	parsear(script);
	return;
}

int main(void) {
	iniciar();
	//conectarAMemoria();
	//pruebaParser();
	/*t_tabla* tabla = (t_tabla*) malloc(sizeof(t_tabla));
	tabla->consistencia = "SC";
	tabla->nombre_tabla = "prueba";
	agregar_tabla(tabla);
	t_memoria* memoria = (t_memoria*) malloc(sizeof(t_memoria));
	memoria->id_mem = 1;
	memoria->socket_mem = 5;
	memoria->valida = true;
	sc = memoria,
	setConsole();
	free_memoria(memoria);*/
	free(puertoMemoria);
	exit_gracefully(EXIT_SUCCESS);
}

void iniciar(){
	verificarArchivoConfigKernel();
	mostrarDatosArchivoConfigKernel();
	puertoMemoria = int_to_string(configKernel.puerto_memoria);
	inicializarIds();
	crear_colas();
	crear_listas();
	return;
}

void conectarAMemoria(){
	char * buffer[1024];
	int socketMemoria = conectarAlServidor(configKernel.ip_memoria,puertoMemoria);
	send(socketMemoria,"Hola Memoria",strlen("Hola Memoria"),0);
	int bytes = recv(socketMemoria,buffer,1024,0);
	buffer[bytes] = '\0';
	printf("%s%",buffer);
}
