#include "sockets.h"

int conectarAlServidor(char * ip, char * port) {
  struct addrinfo hints;
  struct addrinfo *server_info;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
  hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP

  getaddrinfo(ip, port, &hints, &server_info);  // Carga en server_info los datos de la conexion

  // 2. Creemos el socket con el nombre "server_socket" usando la "server_info" que creamos anteriormente
  int server_socket = socket( server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol );

  int retorno = connect(server_socket,server_info->ai_addr,server_info->ai_addrlen);

  freeaddrinfo(server_info);  // No lo necesitamos mas

  if (retorno==-1)
  {
	  puts("Error al conectar");
	  exit(0);
  }
  return server_socket;
}

