#include "kernel.h"

int main(void) {
	escuchar();
	multiplexacion(socketKernel);
	return EXIT_SUCCESS;
}

void escuchar()
{
	struct sockaddr_in dirServidor;

	int skEnUso;

	dirServidor.sin_family = AF_INET;
	dirServidor.sin_port = htons(8080); //serverPort
	dirServidor.sin_addr.s_addr = htonl(INADDR_ANY);

	//printf("IP server: %d\n",dirServidor.sin_addr.s_addr);

	socketKernel = socket(AF_INET, SOCK_STREAM, 0);

	skEnUso = 1;
	setsockopt(socketKernel,SOL_SOCKET,SO_REUSEADDR,&skEnUso,sizeof(skEnUso));

	if(bind(socketKernel,(void*) &dirServidor,sizeof(dirServidor)) != 0)
	{
		perror("Fallo del bind");
		exit(0);
	}

	if(listen(socketKernel,10)==0)
	{
		puts("Esperando conexiones");
	}
	else
	{
		puts("fallo al establecer escucha");
		exit(0);
	}

	printf("\n");

}

void multiplexacion(int socketServidor)
{
	fd_set readSet;
	fd_set tempReadSet;
	int maximoFD;
	int resultado;

	int fd;
	int nuevaConexion;
	char* mensaje[1024];

	FD_ZERO(&readSet);
	FD_ZERO(&tempReadSet);
	FD_SET(socketServidor,&readSet);
	maximoFD = socketServidor;
	int i = 0;
	while(i < 8)
	{
		FD_ZERO(&tempReadSet);
		tempReadSet = readSet;

		resultado = select(maximoFD + 1, &tempReadSet, NULL, NULL, NULL);

		if(resultado == -1)
		{
			printf("Fallo en select().\n");
		}
		//VERIFICO SI HAY NUEVA CONEXION
		if (FD_ISSET(socketServidor, &tempReadSet))
		{
			nuevaConexion = aceptarConexiones(socketServidor);
			FD_SET(nuevaConexion,&readSet);
			maximoFD = (nuevaConexion > maximoFD)?nuevaConexion:maximoFD;
			FD_CLR(socketServidor,&tempReadSet);
		}
		//BUSCO EN EL CONJUNTO, si hay datos para leer
		for(fd = 0; fd <= maximoFD; fd++)
		{
			if (FD_ISSET(fd, &tempReadSet)) //Si llega un mensaje
			{
				int bytes=recv(fd,mensaje,1024,0);
				mensaje[bytes]='\0';
				puts(mensaje);
				send(fd,"hola memoria",strlen("hola memoria"),0);
				//realizarProtocoloDelPackage(package, fd);
			}
			FD_CLR(fd,&tempReadSet);
		}
		i++;
	}
}

int aceptarConexiones(int servidorKernel)
{
	struct sockaddr_in dirCliente;
	unsigned int tamanoDir;

	int socket;

	tamanoDir= sizeof(struct sockaddr_in);
	socket = accept(servidorKernel,(void*) &dirCliente,&tamanoDir);

	if (socket == -1)
	{
		puts("error");
		exit(0);
	}

	return socket;
}
