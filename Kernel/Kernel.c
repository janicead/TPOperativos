#include "Kernel.h"

void pruebaParser(){
	FILE* script = abrirArchivo("/home/utnso/tp-2019-1c-BEFGN/prueba.lql");
	parsear(script);
	return;
}

int main(void) {
	iniciar();
	//conectarAMemoria();
	pruebaParser();
	free(puertoMemoria);
	exit_gracefully(EXIT_SUCCESS);
}

void iniciar(){
	verificarArchivoConfigKernel();
	mostrarDatosArchivoConfigKernel();
	puertoMemoria = int_to_string(configKernel.puerto_memoria);
}

void conectarAMemoria(){
	char * buffer[1024];
	int socketMemoria = conectarAlServidor(configKernel.ip_memoria,puertoMemoria);
	send(socketMemoria,"Hola Memoria",strlen("Hola Memoria"),0);
	int bytes = recv(socketMemoria,buffer,1024,0);
	buffer[bytes] = '\0';
	printf("%s%",buffer);
}
