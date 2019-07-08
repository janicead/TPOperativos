#include "socketsMemoria.h"

void enviarRespuesta(int socketReceptor, int protocoloID, char *respuesta){
	char* stringSerializado;
	t_UnString *unString = definirT_UnString(respuesta);

	stringSerializado = serializarT_UnString(unString);
	int tamanioStructSerializado = sizeof(uint32_t) + unString->longString;
	empaquetarEnviarMensaje(socketReceptor,protocoloID,tamanioStructSerializado,stringSerializado);

	free(stringSerializado);
	freeT_UnString(unString);
}

void realizarMultiplexacion(int socketEscuchando){
	fd_set readSet;
	fd_set tempReadSet;
	int maximoFD;
	int resultado;

	int fd;
	int nuevaConexion;
	t_PaqueteDeDatos *package;

	FD_ZERO(&readSet);
	FD_ZERO(&tempReadSet);

	FD_SET(socketEscuchando,&readSet);
	maximoFD = socketEscuchando;
	int i = 0;
	while(1){
		FD_ZERO(&tempReadSet);
		tempReadSet = readSet;
		resultado = select(maximoFD + 1, &tempReadSet, NULL, NULL, NULL);
		printf("%d",resultado);

		if(resultado == -1){
			printf("Fallo en select().\n");
			exitGracefully(EXIT_FAILURE,loggerMemoria,socketEscuchando);
		}
		//VERIFICO SI HAY NUEVA CONEXION
		if (FD_ISSET(socketEscuchando, &tempReadSet)){
			nuevaConexion = aceptarConexiones(socketEscuchando, loggerMemoria);
			int numMemoria = configMemoria.numeroDeMemoria;
			char* soyMemoria = string_new();
			string_append(&soyMemoria, "SOY MEMORIA ");
			char* numeroMemoria[1000];
			kernel = nuevaConexion;
			sprintf(numeroMemoria, "%d", numMemoria);
			string_append(&soyMemoria, numeroMemoria);
			realizarHandShake(nuevaConexion,KERNELOMEMORIA,soyMemoria);
			FD_SET(nuevaConexion,&readSet);
			maximoFD = (nuevaConexion > maximoFD)?nuevaConexion:maximoFD;
			FD_CLR(socketEscuchando,&tempReadSet);
		}
		//BUSCO EN EL CONJUNTO, si hay datos para leer
		for(fd = 0; fd <= maximoFD; fd++){
			if (FD_ISSET(fd, &tempReadSet)){ //HAY UN PAQUETE PARA RECIBIR
				package = recibirPaquete(fd);
				printf("\npackage->ID: %d\n",package->ID);
				gestionarPaquetes(package, fd);
			}
			FD_CLR(fd,&tempReadSet);
		}
		i++;
	}
}

void gestionarPaquetes(t_PaqueteDeDatos *packageRecibido, int socketEmisor){
	if(packageRecibido->ID == 13){ //13: SELECT
		t_SELECT *unSELECT;
		int id_respuesta_select = 14;//14: RESPUESTA DE UN PROTOCOLO = 13
		unSELECT = deserializarT_SELECT(packageRecibido->Datos);
		log_info(loggerMemoria,"Query recibido: SELECT [%s] [%d]",unSELECT->nombreTabla,unSELECT->KEY);
		uint16_t key = (uint16_t) unSELECT->KEY;
		char* Respuesta = SELECTMemoria(unSELECT->nombreTabla,key,0);
		enviarRespuesta(socketEmisor,id_respuesta_select,Respuesta);

		freeT_SELECT(unSELECT);
	}

	if(packageRecibido->ID == 15){ //15: INSERT
		t_INSERT *unINSERT;
		int id_respuesta_insert = 16; //16: RESPUESTA DE UN PROTOCOLO = 15
		unINSERT = deserializarT_INSERT(packageRecibido->Datos);
		log_info(loggerMemoria,"Query recibido: INSERT [%s] [%d] [%s] [%d]",unINSERT->nombreTabla,unINSERT->KEY,unINSERT->Value,unINSERT->timeStamp);

		uint16_t key = (uint16_t) unINSERT->KEY;

		char* respuesta = INSERTMemoria(unINSERT->nombreTabla,key, unINSERT->Value, (unsigned long int)unINSERT->timeStamp);
		enviarRespuesta(socketEmisor,id_respuesta_insert,respuesta);

		freeT_INSERT(unINSERT);
	}

	if(packageRecibido->ID == 17){ //17: CREATE
		t_CREATE *unCREATE;
		int id_respuesta_create = 18;//18: RESPUESTA DE UN PROTOCOLO = 17

		unCREATE = deserializarT_CREATE(packageRecibido->Datos);
		log_info(loggerMemoria,"Query recibido: CREATE [%s] [%s] [%d] [%d]",unCREATE->nombreTabla,unCREATE->tipoConsistencia,unCREATE->nParticiones,unCREATE->tiempoCompactacion);

		char* Respuesta = string_from_format("CREATE OK");
		enviarRespuesta(socketEmisor,id_respuesta_create,Respuesta);

		free(Respuesta);
		freeT_CREATE(unCREATE);
	}

	if(packageRecibido->ID == 19){ //19: DESCRIBE
		t_DESCRIBE *unDESCRIBE;
		char* Respuesta;
		int id_respuesta_describe = 20;//20: RESPUESTA DE UN PROTOCOLO = 19

		unDESCRIBE = deserializarT_DESCRIBE(packageRecibido->Datos);
		log_info(loggerMemoria,"Query recibido: DESCRIBE [%s]",unDESCRIBE->nombreTabla);

		if(string_is_empty(unDESCRIBE->nombreTabla)){
			char* respuesta = DESCRIBETodasLasTablasMemoria();
			enviarRespuesta(socketEmisor,id_respuesta_describe, respuesta);
		}
		else{
			char* respuesta = DESCRIBEMemoria(unDESCRIBE->nombreTabla);
			enviarRespuesta(socketEmisor,id_respuesta_describe,respuesta);
		}
		freeT_DESCRIBE(unDESCRIBE);
	}

	if(packageRecibido->ID == 21){ //11:  DROP
		t_DROP *unDROP;
		int id_respuesta_drop = 22;//22: RESPUESTA DE UN PROTOCOLO = 21

		unDROP = deserializarT_DROP(packageRecibido->Datos);
		log_info(loggerMemoria, "Query recibido: DESCRIBE [%s]",unDROP->nombreTabla);

		char* respuesta = DROPMemoria(unDROP->nombreTabla);
		enviarRespuesta(socketEmisor,id_respuesta_drop,respuesta);

		freeT_DROP(unDROP);
	}
	if(packageRecibido->ID == 23){ // JOURNAL

		int id_respuesta_journal = 24;//23: RESPUESTA DE UN PROTOCOLO = 24

		log_info(loggerMemoria, "Query recibido: JOURNAL [%s]",packageRecibido->Datos);

		//char* respuesta = DROPMemoria(unDROP->nombreTabla);
		enviarRespuesta(socketEmisor,id_respuesta_journal,"Todo ok");

	}
	freePackage(packageRecibido);
}

void definirValorKernel(){
	kernel= 0;
}

void realizarGossip(){

	clock_t start, diff;
	int elapsedsec;
	int sec = 10; //aca debe ir tiempoGossiping
	int iterations = 0;

	while (iterations < 1000) {
	   start = clock();

	   while (1) {
		   diff = clock() - start;
	       elapsedsec = diff / CLOCKS_PER_SEC;

	       if (elapsedsec >= sec) {
	    	   pthread_create(&clienteMemoria, NULL, hacermeClienteDeMisServers, NULL);

	    	   if(kernel!=0){
	    	   char* memoriasEnTablaDeGossip = memoriasTablaDeGossip();
	    	   enviarMemoriasTablaGossip(kernel,KERNELOMEMORIA,memoriasEnTablaDeGossip);
	    	   free(memoriasEnTablaDeGossip);
	    	   }
	           iterations++;
	           break;
	       }
	   }
	}
}

void iniciarEscucha(){
	struct sockaddr_in dirServidor;

	int skEnUso;

	dirServidor.sin_family = AF_INET;
	dirServidor.sin_port = htons(configMemoria.puertoDeEscucha); //serverPort
	dirServidor.sin_addr.s_addr = htonl(INADDR_ANY);

	//printf("IP server: %d\n",dirServidor.sin_addr.s_addr);

	log_info(loggerMemoria, "Asignado un socket al proceso LFS");
	servidorEscuchaMemoria = socket(AF_INET, SOCK_STREAM, 0);

	skEnUso = 1;
	setsockopt(servidorEscuchaMemoria,SOL_SOCKET,SO_REUSEADDR,&skEnUso,sizeof(skEnUso));

	if(bind(servidorEscuchaMemoria,(void*) &dirServidor,sizeof(dirServidor)) != 0)
	{
		log_error(loggerMemoria, "Fallo del bind()");
		exit(0);
	}

	if(listen(servidorEscuchaMemoria,30)==0)
	{
		log_info(loggerMemoria, "Esperando conexiones, listen()....");
	}
	else
	{
		log_error(loggerMemoria,"fallo al establecer escucha, listen()");
		exit(0);
	}
}

void iniciarEscuchaMemoria(){
	int yes=1;
    if ((servidorEscuchaMemoria = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    if (setsockopt(servidorEscuchaMemoria, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    // enlazar
    char* ipDeEscucha = quitarComillas(configMemoria.ipDeEscucha);
    servidorMemoria.sin_family = AF_INET;
    servidorMemoria.sin_addr.s_addr = inet_addr(ipDeEscucha);
    servidorMemoria.sin_port = htons(configMemoria.puertoDeEscucha); //configMemoria.puertoDeEscucha
    memset(&(servidorMemoria.sin_zero), '\0', 8);

    if (bind(servidorEscuchaMemoria, (struct sockaddr *)&servidorMemoria, sizeof(servidorMemoria)) == -1) {
        perror("bind");
        exit(1);
    }
    // escuchar
    if (listen(servidorEscuchaMemoria, 30) == -1) {
        perror("listen");
        exit(1);
    }
    free(ipDeEscucha);
}

void serCliente(char* ip , int puerto){
	int cliente;
	char* ipServidor = quitarComillas(ip);
	struct sockaddr_in dirServidorMemoria;
	struct sockaddr_in dirCliente;
	unsigned int tamanioDireccion;
	dirServidorMemoria.sin_family = AF_INET;
	dirServidorMemoria.sin_addr.s_addr = inet_addr(ipServidor);
	dirServidorMemoria.sin_port = htons(puerto); //puerto al que va a escuchar
	cliente = socket(AF_INET,SOCK_STREAM,0);
	if (connect (cliente, (void*)&dirServidorMemoria, sizeof(dirServidorMemoria))!=0){
		log_info(loggerMemoria,"La memoria con puerto %d e ip %s no se encuentra activa",puerto,ipServidor);

	}else{
		int numMemoria = configMemoria.numeroDeMemoria;
		char* soyMemoria = string_new();
		string_append(&soyMemoria, "SOY MEMORIA ");
		char* numeroMemoria= malloc (150);
		sprintf(numeroMemoria, "%d", numMemoria);
		string_append(&soyMemoria, numeroMemoria);

		realizarHandShake(cliente,KERNELOMEMORIA,soyMemoria);
		int nroMemoria = recibirHandShakeMemoria(cliente,KERNELOMEMORIA,loggerMemoria);

		if(nroMemoria!= NULL){
			char* memoriasDondeEstoyConectado = memoriasTablaDeGossip();
			enviarMemoriasTablaGossip(cliente,KERNELOMEMORIA,memoriasDondeEstoyConectado);
			agregarATablaDeGossip(puerto, ip, nroMemoria);
			mostrarmeMemoriasTablaGossip();
			recibirMemoriasTablaDeGossip(cliente,KERNELOMEMORIA,loggerMemoria);
			free(memoriasDondeEstoyConectado);
		}
	free(soyMemoria);
	free(numeroMemoria);
	}
	free(ipServidor);
}

void conectarmeAEsaMemoria(int puerto,char* ip, t_log* logger){

	int cliente;
	struct sockaddr_in dirServidorMemoria;
	struct sockaddr_in dirCliente;
	unsigned int tamanioDireccion;
	dirServidorMemoria.sin_family = AF_INET;
	dirServidorMemoria.sin_addr.s_addr = inet_addr(ip);
	dirServidorMemoria.sin_port = htons(puerto); //puerto al que va a escuchar
	cliente = socket(AF_INET,SOCK_STREAM,0);
	if (connect (cliente, (void*)&dirServidorMemoria, sizeof(dirServidorMemoria))!=0){
		log_info(logger,"La memoria con puerto %d e ip %s no se encuentra activa",puerto,ip);
	}
	else {
		int numMemoria = configMemoria.numeroDeMemoria;
		char* soyMemoria = string_new();
		string_append(&soyMemoria, "SOY MEMORIA ");
		char* numeroMemoria= malloc (150);
		sprintf(numeroMemoria, "%d", numMemoria);
		string_append(&soyMemoria, numeroMemoria);

		realizarHandShake(cliente,KERNELOMEMORIA,soyMemoria);
		int nroMemoria = recibirHandShakeMemoria(cliente,KERNELOMEMORIA,loggerMemoria);
	}

}

/*void realizarMultiplexacion(int socketEscuchando){
	int fdmax;        // número máximo de descriptores de fichero
	int newfd;        // descriptor de socket de nueva conexión aceptada
	char buf[256];    // buffer para datos del cliente
	int nbytes;
	int addrlen;
	int i, j;
	FD_ZERO(&master);    // borra los conjuntos maestro y temporal
	FD_ZERO(&copy);
	// añadir listener al conjunto maestro
	FD_SET(servidorEscuchaMemoria, &master);
	// seguir la pista del descriptor de fichero mayor
	fdmax = servidorEscuchaMemoria; // por ahora es éste
	// bucle principal
	for(;;) {
		copy = master;
		if (select(fdmax+1, &copy, NULL, NULL, NULL) == -1) {
		    perror("select");
		    exit(1);
		}
		 // explorar conexiones existentes en busca de datos que leer
		for(i = 0; i <= fdmax; i++) {
		    if (FD_ISSET(i, &copy)) { // ¡¡tenemos datos!!
		    	if (i == servidorEscuchaMemoria) {
		                        // gestionar nuevas conexiones
		    		addrlen = sizeof(clienteMemoria);
		            	if ((newfd = accept(servidorEscuchaMemoria, (struct sockaddr *)&clienteMemoria,&addrlen)) == -1) {
		            		perror("accept");
		                } else {
		                    FD_SET(newfd, &master); // añadir al conjunto maestro
		                    if (newfd > fdmax) {    // actualizar el máximo
		                    	fdmax = newfd;
		                    }
		                    int valor = recibirHandShakeMemoria(newfd,KERNELOMEMORIA, loggerMemoria);
		                    if(valor!=0){
		                    	int numMemoria = configMemoria.numeroDeMemoria;
		                    	char* soyMemoria = string_new();
		                    	string_append(&soyMemoria, "SOY MEMORIA ");
		                    	char* numeroMemoria[1000];
		                    	sprintf(numeroMemoria, "%d", numMemoria);
		                    	string_append(&soyMemoria, numeroMemoria);
		                    	realizarHandShake(newfd,KERNELOMEMORIA,soyMemoria);
		                    	char* memoriasTablaGossip = memoriasTablaDeGossip();
		                    	recibirMemoriasTablaDeGossip(newfd,KERNELOMEMORIA,loggerMemoria);
		                    	mostrarmeMemoriasTablaGossip();
		                    	enviarMemoriasTablaGossip(newfd,KERNELOMEMORIA,memoriasTablaGossip);
		                    	free(soyMemoria);
		                    	free(memoriasTablaGossip);
		                    }
		                    else{
		                    	int numMemoria = configMemoria.numeroDeMemoria;
		                    	char* soyMemoria = string_new();
		                    	string_append(&soyMemoria, "SOY MEMORIA ");
		                    	char* numeroMemoria[1000];
		                    	kernel = newfd;
		                    	sprintf(numeroMemoria, "%d", numMemoria);
		                    	string_append(&soyMemoria, numeroMemoria);
		                    	realizarHandShake(newfd,KERNELOMEMORIA,soyMemoria);
		                    	char* memoriasTablaGossip = memoriasTablaDeGossip();
		                    	enviarMemoriasTablaGossip(newfd,KERNELOMEMORIA,memoriasTablaGossip);
		                    	free(soyMemoria);
		                    	free(memoriasTablaGossip);
		                    }


		                }
		    	} else {
		    		t_PaqueteDeDatos *package;
		    		package = (t_PaqueteDeDatos*) malloc(sizeof(t_PaqueteDeDatos));
		            // gestionar datos de un cliente
		    		if ((nbytes = recv(i, &package->ID,sizeof(uint32_t),MSG_WAITALL)) > 0) {
		    			nbytes = recv(i, &package->longDatos,sizeof(uint32_t),MSG_WAITALL);
		    			package->Datos = (char*) malloc(package->longDatos +1); //+1 VALGRIND
		    			nbytes = recv(i, package->Datos, package->longDatos, MSG_WAITALL);
		    			t_SELECT* select = deserializarT_SELECT(package->Datos);
		    			printf("Los datos que recibi del socket %d, son %s\n", i, select->nombreTabla);
		    			char* palabra="hola";
		    			t_UnString* s = definirT_UnString(palabra);
		    			char* serializados= serializarT_UnString(s);
		    			int tamanioSerializado = s->longString+ sizeof (uint32_t);

		    			empaquetarEnviarMensaje(i,14, tamanioSerializado,serializados);
	                   // close(i); // bye!
	                   // FD_CLR(i, &master); // eliminar del conjunto maestro
		    		}
		    		else if (nbytes == 0) {
		                    // conexión cerrada
		    				printf("El socket %d se desconecto\n", i);
		                    close(i); // bye!
		                    FD_CLR(i, &master); // eliminar del conjunto maestro
		                }else if(nbytes<0) {
		                    perror("recv");
		                    close(i); // bye!
		                    FD_CLR(i, &master); // eliminar del conjunto maestro
		                } else {
		                            // tenemos datos de algún cliente
		                    for(j = 0; j <= fdmax; j++) {
		                                // ¡enviar a todo el mundo!
		                    	if (FD_ISSET(j, &master)) {
		                                    // excepto al listener y a nosotros mismos
		                    		if (j != servidorEscuchaMemoria && j != i) {
		                                        if (send(j, buf, nbytes, 0) == -1) {
		                                            perror("send");
		                                        }
		                                    }
		                                }
		                            }
		                        }
		                    }
		                }
		            }
		        }

}*/

void hacermeClienteDeMisServers(){
	char** ipDeSeeds = configMemoria.ipDeSeeds;
	int* puertosDeSeeds = configMemoria.puertosDeSeeds;
	int cantidadSeeds = tamanioArray(ipDeSeeds);
	if(cantidadSeeds ==0){
	 log_info(loggerMemoria, "No tengo que conectarme a ninguna memoria");
	}else{
		for(int i = 0; i< cantidadSeeds; i++){
			serCliente(ipDeSeeds[i], puertosDeSeeds[i]);
		}
	}
}

int aceptarConexiones(int socket, t_log* logger){
	struct sockaddr_in dirCliente;
	unsigned int tamanoDir;

	int skUnaConexion;

	tamanoDir= sizeof(struct sockaddr_in);
	skUnaConexion = accept(socket,(void*) &dirCliente,&tamanoDir);

	if (skUnaConexion == -1)
	{
		log_error(logger, "Fallo en la conexion, accept()");
		exitGracefully(0, logger,socket);
	}

	log_info(logger, "Acepte una conexion en socket: %d",skUnaConexion);
	return skUnaConexion;
}

void exitGracefully(int return_nr, t_log* logger, int servidorEscucha)
{
	config_destroy(archivoConfigMemoria);
	log_destroy(logger);

	close(servidorEscucha);
	exit(return_nr);
}


