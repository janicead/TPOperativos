#include "Kernel.h"

int main(void) {
	/*char * buffer[1024];
	puts("conectandome");
	int socketMemoria = conectarAlServidor("127.0.0.1","8080");
	send(socketMemoria,"Hola Memoria",strlen("Hola Memoria"),0);
	int bytes = recv(socketMemoria,buffer,1024,0);
	buffer[bytes] = '\0';
	printf("%s%",buffer);*/
	verificarArchivoConfigKernel();
	mostrarDatosArchivoConfigKernel();
	exit_gracefully(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}


