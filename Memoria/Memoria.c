#include "Memoria.h"

int main(void) {
	char * buffer;
	puts("conectandome");
	int socketKernel = conectarAlServidor("127.0.0.1","8080");
	puts("enviando");
	send(socketKernel,"Hola Kernel",strlen("Hola Kernel"),0);
	int bytes = recv(socketKernel,buffer,1024,0);
	buffer[bytes] = '\0';
	printf("%s%",buffer);
	exit_gracefully(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
