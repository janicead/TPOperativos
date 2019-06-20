#include "sockets.h"

/*int conectarAlServidorEric(char * ip, char * port) {
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
} */
int crearServidor(){
	int servidor,aceptar = 1;
	if((servidor= socket(AF_INET, SOCK_STREAM, 0))==-1){
			perror("Error al asignar socket");
			exit(1);
		}
	//si socket ya esta siendo utilizado tira error
	if (setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &aceptar, sizeof(aceptar))){
		perror("Socket utilizado");
		exit(EXIT_FAILURE);
	}
	return servidor;
}
void escuchar(int servidor,struct sockaddr_in dirSocket){
	if(bind(servidor,(struct sockaddr *)&dirSocket,sizeof(dirSocket))<0){
		perror("Error al esperar conexion");
		exit(1);
	}
	if(listen(servidor,100)==-1){
		perror("Error al escuchar");
		exit(1);
	}
}
int conectarAlServidor( char* ip, int puerto, t_log* logger){
	char* ipSinComillas=string_substring(ip,1,(string_length(ip)-2));
	struct sockaddr_in dirSocket;
	int cliente;
	cliente= socket(AF_INET, SOCK_STREAM, 0);
	if(cliente==-1){
		log_error(logger,"\nError al crear socket\n");
		return -1;
	}
	memset(&dirSocket,'0',sizeof(dirSocket));
	dirSocket.sin_family = AF_INET;
	dirSocket.sin_addr.s_addr = inet_addr(ipSinComillas);
	dirSocket.sin_port = htons(puerto);

	if(inet_pton(AF_INET,ipSinComillas , &dirSocket.sin_addr)<=0)
	    {
		log_error(logger,"\nDireccion invalida/ Direccion no soportada\n" );
	    return -1;
	    }

	if(connect(cliente,(struct sockaddr*)&dirSocket,sizeof(dirSocket))<0){
		log_error(logger,"No se pudo realizar la conexion\n" );
		return 1;
	}
	return cliente;
}
void emitirMensaje(int cliente) {
    char mensaje[1024] = {};
    while (1) {
        while (fgets(mensaje, 1024, stdin) != NULL) {
        	corteStringAntesDeEnter(mensaje, 1024);
            if (strlen(mensaje) == 0) {
            } else {
                break;
            }
        }
        send(cliente, mensaje, 1024, 0);
        if (strcmp(mensaje, "exit") == 0) {
            break;
        }
    }
}
void corteStringAntesDeEnter (char* mensaje, int largo) {
    int i;
    for (i = 0; i < largo; i++) {
        if (mensaje[i] == '\n') {
            mensaje[i] = '\0';
            break;
        }
    }
}
void receptarMensaje(int cliente) {
    char mensajeRecibido[1024] = {};
    while (1) {
        int recibido = recv(cliente, mensajeRecibido, 1024, 0);
        if (recibido > 0) {
            printf("\r%s\n", mensajeRecibido);
        } else if (recibido == 0) {
            break;
        }
    }
}
/////////////////////////////////////////HANDSHAKEEEEEEEEEEEEEEE/////////////////////////////////////
t_handShake crearHandShake(t_identidad identidad, char* mensaje){
	t_handShake handShake;
	handShake.identidad = identidad;
	handShake.mensaje = mensaje;
	handShake.longMensaje = strlen(mensaje);
	return handShake;
}
void realizarHandShake(int servidor, t_identidad identidad, char*mensaje){
	t_handShake handShake;
	t_handShake* punteroHandShake;
	char* handShakeSerializado;
	handShake = crearHandShake(identidad,mensaje);
	punteroHandShake= crearPunteroHandShake(handShake);
	int sizeHandshakeAEnviar = sizeof(t_identidad) + sizeof(uint32_t) + punteroHandShake->longMensaje;
	handShakeSerializado=serializarHandShake(punteroHandShake);
	empaquetarEnviarMensaje(servidor,punteroHandShake->identidad,sizeHandshakeAEnviar,handShakeSerializado);
	freeHandShake(punteroHandShake);
	return;
}


t_handShake* crearPunteroHandShake(t_handShake handShake){
	t_handShake *nuevoHandShake = (t_handShake*) malloc(sizeof(t_handShake));
	nuevoHandShake->identidad=handShake.identidad;
	nuevoHandShake->mensaje = malloc((strlen(handShake.mensaje)+1) * sizeof(char));
	strcpy(nuevoHandShake->mensaje,handShake.mensaje);
	nuevoHandShake->longMensaje = handShake.longMensaje;
	return nuevoHandShake;
}

char* serializarHandShake(t_handShake *handShake){
	int sizeIdentidad= sizeof(handShake->identidad);
	int sizeMensaje=handShake->longMensaje;
	char* handShakeSerializado = malloc(sizeMensaje + sizeIdentidad + sizeof(uint32_t));
	int copiarDesde=0;

	memcpy(handShakeSerializado + copiarDesde, &(handShake->identidad),sizeIdentidad);
	copiarDesde += sizeIdentidad;

	memcpy(handShakeSerializado + copiarDesde, &(handShake->longMensaje),sizeof(handShake->longMensaje));
	copiarDesde += sizeof(handShake->longMensaje);

	memcpy(handShakeSerializado + copiarDesde, (handShake->mensaje),handShake->longMensaje);
	return handShakeSerializado;
}

void freePackage(t_PaqueteDeDatos *unPackage)
{
	free(unPackage->Datos);
	free(unPackage);
}

void empaquetarEnviarMensaje(int socketReceptor, int unID, int longitudDatos, char *datos){
	t_PaqueteDeDatos *package;
	char * paqueteListo;
	int offset;

	package = (t_PaqueteDeDatos *) malloc(sizeof(t_PaqueteDeDatos));
	definirHeader(package, unID,longitudDatos);
	package->Datos = datos;

	paqueteListo = malloc((sizeof(uint32_t)*2) + (package->longDatos));
	strcpy(paqueteListo,"");

	offset = 0;
	memcpy(paqueteListo + offset, &(package->ID), sizeof(package->ID));

	offset += sizeof(package->ID);
	memcpy(paqueteListo + offset,  &(package->longDatos), sizeof(package->longDatos));

	offset += sizeof(package->longDatos);
	memcpy(paqueteListo + offset, package->Datos, package->longDatos);

	int bytesEnviados;

	bytesEnviados = send(socketReceptor,paqueteListo,(sizeof(uint32_t)*2) + (package->longDatos),0);
	if( bytesEnviados== -1)
	{
		perror("fallo en send(), al enviar el paqueteListo");
		exit(0);
	}

	free(paqueteListo);

}
void definirHeader(t_PaqueteDeDatos *unPackage,int unID, int unaLongitudData)
{
	unPackage->ID = unID;
	unPackage->longDatos = unaLongitudData;
}

void freeHandShake(t_handShake* handShake){
	free(handShake->mensaje);
	free(handShake);
	return;
}
void enviarMemoriasTablaGossip(int servidor, t_identidad identidad, char*mensaje){
	t_handShake handShake;
	t_handShake* punteroHandShake;
	char* handShakeSerializado;
	handShake = crearHandShake(identidad,mensaje);
	punteroHandShake= crearPunteroHandShake(handShake);
	int sizeHandshakeAEnviar = sizeof(t_identidad) + sizeof(uint32_t) + punteroHandShake->longMensaje;
	handShakeSerializado=serializarHandShake(punteroHandShake);
	empaquetarEnviarMensaje(servidor,punteroHandShake->identidad,sizeHandshakeAEnviar,handShakeSerializado);
	freeHandShake(punteroHandShake);
	return;
}

int recibirHandShakeMemoria(int emisor,t_identidad identidad, t_log* logger){
	t_PaqueteDeDatos* paquete=recibirPaquete(emisor);
	if(paquete->ID==identidad){
		t_handShake* punteroHandShake;
		punteroHandShake=deserializarHandShake(paquete->Datos);
		char* verificado= verificarMensajeDeMemoria(punteroHandShake->mensaje,identidad);
		if(identidad==MEMORIA && verificado!=NULL){

			char* soyMemoria = string_new();
			string_append(&soyMemoria, "Handshake con MEMORIA ");
			char* numeroMemoria[1000];
			string_append(&soyMemoria, verificado);
        	string_append(&soyMemoria, " realizado con exito");
        	int valor = atoi(verificado);
			log_info(logger,soyMemoria);
			free(soyMemoria);
			free(punteroHandShake->mensaje);
			free(punteroHandShake);
			free(paquete->Datos);
			free(paquete);
			free(verificado);
			return valor;
		}

		else if(identidad==KERNELOMEMORIA){
			if(string_equals_ignore_case(verificado,"SOY KERNEL")){
				log_info(logger,"Handshake con KERNEL realizado con exito");
				free(punteroHandShake->mensaje);
				free(punteroHandShake);
				free(paquete->Datos);
				free(paquete);
				return 0;
			}
			else{
				char* soyMemoria = string_new();
				string_append(&soyMemoria, "Handshake con MEMORIA ");
				char* numeroMemoria[1000];
				string_append(&soyMemoria, verificado);
	        	string_append(&soyMemoria, " realizado con exito");
	        	int valor = atoi(verificado);
				log_info(logger,soyMemoria);
				free(soyMemoria);
				free(punteroHandShake->mensaje);
				free(punteroHandShake);
				free(paquete->Datos);
				free(paquete);
				free(verificado);
				return valor;
			}
		}
		else{
			log_error(logger,"Mensaje incorrecto");
			free(punteroHandShake->mensaje);
			free(punteroHandShake);
			free(paquete->Datos);
			free(paquete);
			free(verificado);
			return NULL;
		}
	}
	else{
		log_error(logger,"ID incorrecto");
		free(paquete->Datos);
		free(paquete);
		return NULL;
	}
}

char* verificarMensajeDeMemoria(char* mensaje,t_identidad identidad){
	//char** s= string_n_split(mensaje, 3, " ");
	return chequear(cortaPalabra(mensaje));

}
char* chequear(char** mensaje){
	if(string_equals_ignore_case("SOY",mensaje[0]) && string_equals_ignore_case("MEMORIA",mensaje[1])){
		char* valor = string_duplicate(mensaje[2]);
		hacerFreeArray(mensaje);
		free(mensaje);
		return valor;
	}
	else if(string_equals_ignore_case("SOY",mensaje[0]) && string_equals_ignore_case("KERNEL",mensaje[1])){
		hacerFreeArray(mensaje);
		free(mensaje);
		return "SOY KERNEL";
	}
	else{
		hacerFreeArray(mensaje);
		free(mensaje);
		return NULL;
	}
}
char** cortaPalabra(char* mensaje){
	return string_n_split(mensaje, 3, " ");
}

int verificarMensajeDeKernel(char* mensaje,t_identidad identidad){
	return string_equals_ignore_case(mensaje,"SOY KERNEL");
}

t_PaqueteDeDatos *recibirPaquete(int socketEmisor)  //v1.5
{

	int bytesRecibidos;

	t_PaqueteDeDatos *package;

	package = (t_PaqueteDeDatos*) malloc(sizeof(t_PaqueteDeDatos));

	bytesRecibidos = recv(socketEmisor, &package->ID,sizeof(uint32_t),MSG_WAITALL);

	if(bytesRecibidos <= 0)
	{
		package->ID = bytesRecibidos;
		//printf("\nID: %d   longDatos: %d\n",package->ID,package->longDatos);
		return package;
	}


	bytesRecibidos = recv(socketEmisor, &package->longDatos,sizeof(uint32_t),MSG_WAITALL);
	//if(package->ID == 0)

	//printf("\nID: %d   longDatos: %d\n",package->ID,package->longDatos);


	package->Datos = (char*) malloc(package->longDatos +1); //+1 VALGRIND

	bytesRecibidos = recv(socketEmisor, package->Datos, package->longDatos, MSG_WAITALL);

	bytesRecibidos = 0;

	return package;
}
t_handShake* deserializarHandShake(char *handShakeSerializado){
	t_handShake *handShake = (t_handShake*) malloc(sizeof(t_handShake));
	int sizeIdentidad= sizeof(handShake->identidad);
	int copiarDesde=0;

	memcpy(&handShake->identidad, handShakeSerializado + copiarDesde,sizeIdentidad);
	copiarDesde += sizeIdentidad;

	memcpy(&handShake->longMensaje,handShakeSerializado + copiarDesde,sizeof(uint32_t));
	handShake->mensaje = (char *) malloc(handShake->longMensaje + 1);
	copiarDesde += sizeof(handShake->longMensaje);

	memcpy(handShake->mensaje, handShakeSerializado + copiarDesde,handShake->longMensaje);

	handShake->mensaje[handShake->longMensaje]='\0';
	return handShake;
	// RECORDAR QUE AL USAR ESTA FUNCION HAY Q HACER UN FREE(handshake->mensaje);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////CONEXION KERNEL Y MEMORIA///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void mostrarmeMemoriasTablaGossip(){
	int puerto;
	int memoria;
	char* puertoChar [150];
	char* memoriaChar [150];
	char* completo  = string_new ();
	int cantidadMemoriasConectadas = cantMemoriasTablaDeGossip();

	for(int i = 0 ; i<cantidadMemoriasConectadas;i ++){

		void * elemento = list_get(tablaDeGossip, i);
		t_memoriaTablaDeGossip *memoriaConectada =(t_memoriaTablaDeGossip*)elemento;
		puerto = memoriaConectada->puerto;
		memoria = memoriaConectada->numeroDeMemoria;
		char * ip = malloc((strlen(memoriaConectada->ip)+1)*sizeof(memoriaConectada->ip));
		strcpy(ip,memoriaConectada->ip);
		string_append(&completo, ip);
		sprintf(puertoChar, " %d",puerto);
		string_append(&completo, puertoChar);
		sprintf(memoriaChar, " %d ",memoria);
		string_append(&completo, memoriaChar);
		puts("-------------------------------------------");
		printf("ACA: /%s/", completo);
		puts("-------------------------------------------");
		free(ip);
	}

	free(completo);

}

int verificarMensajeMemoriasTablaGossip(char* mensaje, t_log* logger){
	char** memorias= string_split(mensaje, " ");
	int cantElementos = tamanioArray(memorias);
	int cantMemorias = cantElementos /3;

	int i = 0;
	int j = 0;
	if(cantMemorias == 0){
		free(memorias);
		return 0;
	} else{
		while(j<cantMemorias){
			agregarATablaDeGossip(atoi(memorias[i+1]), memorias[i],atoi(memorias[i+2]));
			i= i+3;
			j++;

		}
	hacerFreeArray(memorias);
	free(memorias);
	return cantMemorias;

	}

}

void recibirMemoriasTablaDeGossip(int emisor,t_identidad identidad, t_log* logger){
	t_PaqueteDeDatos* paquete=recibirPaquete(emisor);
	if(paquete->ID==identidad){
		t_handShake* punteroHandShake;
		punteroHandShake=deserializarHandShake(paquete->Datos);
		int verificado = verificarMensajeMemoriasTablaGossip(punteroHandShake->mensaje,logger);
		if(identidad==KERNELOMEMORIA && verificado!=0){
			char* soyMemoria = string_new();
			string_append(&soyMemoria, "Recibi las memorias conectadas correctamente");
			log_info(logger,soyMemoria);
			free(soyMemoria);
			free(punteroHandShake->mensaje);
			free(punteroHandShake);
		}
		else if(identidad == KERNELOMEMORIA && verificado ==NULL){
			log_info(logger,"Esta memoria no posee seeds");
			free(punteroHandShake->mensaje);
			free(punteroHandShake);

		}
	}
	else{
			log_error(logger,"ID incorrecto");
	}

	free(paquete->Datos);
	free(paquete);
}


char* memoriasTablaDeGossip(){

	int puerto;
	int memoria;
	char* puertoChar [150];
	char* memoriaChar [150];
	char* completo  = string_new ();
	int cantidadMemoriasConectadas = cantMemoriasTablaDeGossip();

	for(int i = 0 ; i<cantidadMemoriasConectadas;i ++){

		void * elemento = list_get(tablaDeGossip, i);
		t_memoriaTablaDeGossip *memoriaTablaGossip =(t_memoriaTablaDeGossip*)elemento;
		puerto = memoriaTablaGossip->puerto;
		memoria = memoriaTablaGossip->numeroDeMemoria;
		char * ip = malloc((strlen(memoriaTablaGossip->ip)+1));
		strcpy(ip,memoriaTablaGossip->ip);
		string_append(&completo, ip);
		sprintf(puertoChar, " %d",puerto);
		string_append(&completo, puertoChar);
		sprintf(memoriaChar, " %d ",memoria);
		string_append(&completo, memoriaChar);
		free(ip);
	}

	return completo;

}

int cantMemoriasTablaDeGossip(){
	return  list_size(tablaDeGossip);
}

void agregarATablaDeGossip(int puerto, char* ipServidor, int memoria){
	t_memoriaTablaDeGossip * memoriaConectada = malloc (sizeof(t_memoriaTablaDeGossip));
	memoriaConectada->puerto = puerto;
	memoriaConectada->ip = (char*) malloc((strlen(ipServidor)+1)*sizeof(char));
	memoriaConectada->numeroDeMemoria = memoria;
	strcpy(memoriaConectada->ip,ipServidor);
	if(revisarQueNoEsteEnLaLista(memoria)==1){
		list_add(tablaDeGossip , (void *)memoriaConectada);
	}
	else{
		free(memoriaConectada->ip);
		free(memoriaConectada);
	}

}

int revisarQueNoEsteEnLaLista(int nroMemoria){
	int cantidadMemoriasTablaGossip = cantMemoriasTablaDeGossip();
	for(int i = 0 ; i<cantidadMemoriasTablaGossip;i ++){

		void * elemento = list_get(tablaDeGossip, i);
		t_memoriaTablaDeGossip *memoriaTablaGossip =(t_memoriaTablaDeGossip*)elemento;
		if(nroMemoria ==memoriaTablaGossip->numeroDeMemoria ){
			return 0;
		}
	}
	return 1;
}


char * quitarComillas(char* ip){
	return string_substring(ip,1,(string_length(ip)-2));
}
