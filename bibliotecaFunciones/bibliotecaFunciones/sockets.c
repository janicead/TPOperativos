#include "sockets.h"

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
		free(ipSinComillas);
	    return -1;
	    }

	if(connect(cliente,(struct sockaddr*)&dirSocket,sizeof(dirSocket))<0){
		log_error(logger,"No se pudo realizar la conexion\n" );
		free(ipSinComillas);
		return -2;
	}
	free(ipSinComillas);
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
	free(handShakeSerializado);

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

	int bytesEnviados = 0;

	bytesEnviados = send(socketReceptor,paqueteListo,(sizeof(uint32_t)*2) + (package->longDatos),0);
	if( bytesEnviados== -1)
	{
		perror("fallo en send(), al enviar el paqueteListo");
		exit(0);
	}
	free(paqueteListo);
	free(package);

}


int empaquetarEnviarMensaje2(int socketReceptor, int unID, int longitudDatos, char *datos, t_log* logger){
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
		log_error(logger, "No se pudo pedir la TABLA DE GOSSIP ya que esta desconectada dicha memoria");
		return 0;
	}
	free(paqueteListo);
	free(package);
	return 1;

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
	t_handShake handShake = crearHandShake(identidad,mensaje);
	t_handShake* punteroHandShake=crearPunteroHandShake(handShake);
	int sizeHandshakeAEnviar = sizeof(t_identidad) + sizeof(uint32_t) + punteroHandShake->longMensaje;
	char* handShakeSerializado=serializarHandShake(punteroHandShake);
	empaquetarEnviarMensaje(servidor,punteroHandShake->identidad,sizeHandshakeAEnviar,handShakeSerializado);
	freeHandShake(punteroHandShake);
	free(handShakeSerializado);
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
			return -1;
		}
	}
	else{
		log_error(logger,"ID incorrecto");
		free(paquete->Datos);
		free(paquete);
		return -1;
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
	//printf("los bytes recibidos son %d\n", bytesRecibidos);
	if(bytesRecibidos <= 0)
	{
		package->ID = bytesRecibidos;
		//printf("\nID: %d   longDatos: %d\n",package->ID,package->longDatos);
		return package;
	}


	bytesRecibidos = recv(socketEmisor, &package->longDatos,sizeof(uint32_t),MSG_WAITALL);
	//if(package->ID == 0)

	//printf("\nID: %d   longDatos: %d\n",package->ID,package->longDatos);


	//package->Datos = (char*) malloc(package->longDatos +1); //+1 VALGRIND

	package->Datos = calloc(1, package->longDatos+1);

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

void mostrarmeMemoriasTablaGossip(t_list* tablaDeGossip){
	int cantidadMemoriasConectadas = cantMemoriasTablaDeGossip(tablaDeGossip);
	for(int i = 0; i <cantidadMemoriasConectadas; i ++){
		void * elemento = list_get(tablaDeGossip, i);
		t_memoriaTablaDeGossip *memoriaConectada =(t_memoriaTablaDeGossip*)elemento;
		//printf("POSICION '%d'\n", i);
		//printf("IP: '%s'\n",memoriaConectada->ip);
		//printf("PUERTO: '%d'\n",memoriaConectada->puerto);
		//printf("NRO MEMORIA: '%d'\n",memoriaConectada->numeroDeMemoria);
		//char* booleano = pasarBoolAString(memoriaConectada->conectado);
		//printf("CONECTADO: '%s'\n", booleano);
		//puts("---------------------------------------------------");
	}
	//puts("---------------------------------------------------");
}

bool modificadoHacePocoEnGossip(bool modificadoHacePoco){
	return modificadoHacePoco;
}


int verificarMensajeMemoriasTablaGossip(char* mensaje, t_log* logger, t_list* tablaDeGossip){
	char** memorias= string_split(mensaje, " ");
	int cantElementos = tamanioArray(memorias);
	int cantMemorias = cantElementos /4;

	int i = 0;
	int j = 0;

	int tamanioTablaGossip = list_size(tablaDeGossip);
	int valor = 0;
	int miNroDeMemoria =0;
	if(tamanioTablaGossip==1){

		void* elemento =list_get(tablaDeGossip, 0);
		t_memoriaTablaDeGossip * soyYoMismo = (t_memoriaTablaDeGossip*)elemento;
		miNroDeMemoria = soyYoMismo->numeroDeMemoria;
		valor = 1;
	}
	if(cantMemorias == 0){

		free(memorias);
		return 0;
	} else if(cantMemorias ==1 ){
		agregarATablaDeGossipUnicoElemento(atoi(memorias[i+1]), memorias[i],atoi(memorias[i+2]), tablaDeGossip);
		if(valor == 1){
			actualizarMemoriaEspecifica(miNroDeMemoria, tablaDeGossip, true);
		}

		hacerFreeArray(memorias);
		free(memorias);
		return cantMemorias;
	}else{
		while(j<cantMemorias){
			bool booleano = pasarStringABool(memorias[i+3]);
			agregarATablaDeGossip(atoi(memorias[i+1]), memorias[i],atoi(memorias[i+2]),booleano, tablaDeGossip);
			i= i+4;
			j++;

		}
		if(valor == 1){
			actualizarMemoriaEspecifica(miNroDeMemoria, tablaDeGossip, true);
		}
	hacerFreeArray(memorias);
	free(memorias);
	return cantMemorias;

	}

}

void recibirMemoriasTablaDeGossip(int emisor,t_identidad identidad, t_log* logger, t_list* tablaDeGossip){
	t_PaqueteDeDatos* paquete=recibirPaquete(emisor);
	if(paquete->ID==identidad){
		t_handShake* punteroHandShake;
		punteroHandShake=deserializarHandShake(paquete->Datos);
		int verificado = verificarMensajeMemoriasTablaGossip(punteroHandShake->mensaje,logger, tablaDeGossip);
		if(identidad==KERNELOMEMORIA && verificado!=0){
			char* soyMemoria = string_new();
			string_append(&soyMemoria, "Recibi la TABLA DE GOSSIP correctamente");
			log_info(logger,soyMemoria);
			free(soyMemoria);
			free(punteroHandShake->mensaje);
			free(punteroHandShake);
		}
		else if(identidad == KERNELOMEMORIA && verificado ==NULL){
			//log_info(logger,"Esta memoria no posee seeds");
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

char* pasarBoolAString(bool booleano){
	if(booleano){
		return "true";
	}
	return "false";

}

bool pasarStringABool(char* booleano){
	if(string_equals_ignore_case(booleano, "true")){
		return true;
	}
	return false;

}

char* memoriasTablaDeGossip(t_list* tablaDeGossip){

	int puerto;
	int memoria;
	char* puertoChar [150];
	char* memoriaChar [150];
	char* completo  = string_new ();
	int cantidadMemoriasConectadas = cantMemoriasTablaDeGossip(tablaDeGossip);

	for(int i = 0 ; i<cantidadMemoriasConectadas;i ++){

		void * elemento = list_get(tablaDeGossip, i);
		t_memoriaTablaDeGossip *memoriaTablaGossip =(t_memoriaTablaDeGossip*)elemento;
		puerto = memoriaTablaGossip->puerto;
		memoria = memoriaTablaGossip->numeroDeMemoria;
		char* booleano = pasarBoolAString(memoriaTablaGossip->conectado);

		//FORMATO : IP PUERTO NROMEMORIA BOOLCONECTADO
		char * ip = malloc((strlen(memoriaTablaGossip->ip)+1));
		strcpy(ip,memoriaTablaGossip->ip);
		string_append(&completo, ip);
		sprintf(puertoChar, " %d",puerto);
		string_append(&completo, puertoChar);
		sprintf(memoriaChar, " %d ",memoria);
		string_append(&completo, memoriaChar);
		string_append(&completo, booleano);
		string_append(&completo, " ");
		free(ip);
	}

	return completo;

}

int cantMemoriasTablaDeGossip(t_list* tablaDeGossip){
	return  list_size(tablaDeGossip);
}

void agregarATablaDeGossip(int puerto, char* ipServidor, int nroMemoria,bool estado, t_list* tablaDeGossip){
	t_memoriaTablaDeGossip * memoriaConectada = malloc (sizeof(t_memoriaTablaDeGossip));
	memoriaConectada->puerto = puerto;
	memoriaConectada->ip = (char*) malloc((strlen(ipServidor)+1)*sizeof(char));
	strcpy(memoriaConectada->ip,ipServidor);
	memoriaConectada->numeroDeMemoria = nroMemoria;
	memoriaConectada->conectado =estado;


	if(revisarQueNoEsteEnLaLista(nroMemoria, tablaDeGossip)==1 && estado ==true){
		list_add(tablaDeGossip , (void *)memoriaConectada);
	}else if(revisarQueNoEsteEnLaLista(nroMemoria,tablaDeGossip) ==0){
		actualizarMemoriaEspecifica(nroMemoria, tablaDeGossip, estado);
		free(memoriaConectada->ip);
		free(memoriaConectada);
	}
	else{
		free(memoriaConectada->ip);
		free(memoriaConectada);
	}

}

void agregarATablaDeGossipUnicoElemento(int puerto, char* ipServidor, int nroMemoria, t_list* tablaDeGossip){
	t_memoriaTablaDeGossip * memoriaConectada = malloc (sizeof(t_memoriaTablaDeGossip));
	memoriaConectada->puerto = puerto;
	memoriaConectada->ip = (char*) malloc((strlen(ipServidor)+1)*sizeof(char));
	strcpy(memoriaConectada->ip,ipServidor);
	memoriaConectada->numeroDeMemoria = nroMemoria;
	memoriaConectada->conectado =true;
	if(revisarQueNoEsteEnLaLista(nroMemoria, tablaDeGossip)==1){
		list_add(tablaDeGossip , (void *)memoriaConectada);
	}
	else{
		actualizarMemoriaEspecifica(nroMemoria, tablaDeGossip, true);
		free(memoriaConectada->ip);
		free(memoriaConectada);
	}
}

void actualizarMemoriaEspecifica(int nroMemoria, t_list* tablaDeGossip, bool nuevoBool){
	int tamanio = list_size(tablaDeGossip);
	for(int i = 0; i <tamanio ; i ++){
		void* elemento = list_get(tablaDeGossip, i);
		t_memoriaTablaDeGossip * memoriaConectada = (t_memoriaTablaDeGossip*)elemento;
		if(memoriaConectada->numeroDeMemoria==nroMemoria){
			memoriaConectada->conectado=nuevoBool;
		}
	}
}

int revisarQueNoEsteEnLaLista(int nroMemoria, t_list* tablaDeGossip){
	int cantidadMemoriasTablaGossip = list_size(tablaDeGossip);
	for(int i = 0 ; i<cantidadMemoriasTablaGossip;i ++){

		void * elemento = list_get(tablaDeGossip, i);
		t_memoriaTablaDeGossip *memoriaTablaGossip =(t_memoriaTablaDeGossip*)elemento;
		if(nroMemoria ==memoriaTablaGossip->numeroDeMemoria ){
			return 0;
		}
	}
	return 1;
}

