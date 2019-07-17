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
		free(Respuesta);
	}

	if(packageRecibido->ID == 15){ //15: INSERT
		t_INSERT *unINSERT;
		int id_respuesta_insert = 16; //16: RESPUESTA DE UN PROTOCOLO = 15
		unINSERT = deserializarT_INSERT(packageRecibido->Datos);
		log_info(loggerMemoria,"Query recibido: INSERT [%s] [%d] [%s] [%d]",unINSERT->nombreTabla,unINSERT->KEY,unINSERT->Value,unINSERT->timeStamp);

		uint16_t key = unINSERT->KEY;

		pthread_mutex_lock(&semMemoriaPrincipal);

		char* respuesta = INSERTMemoria(unINSERT->nombreTabla,key, unINSERT->Value,  (unsigned long int)unINSERT->timeStamp);
		enviarRespuesta(socketEmisor,id_respuesta_insert,respuesta);
		freeT_INSERT(unINSERT);

		pthread_mutex_lock(&semConfig);
		int retardoMemoriaPrincipal = configMemoria.retardoAccesoMemoriaPrincipal;
		pthread_mutex_unlock(&semConfig);
		sleep(retardoMemoriaPrincipal);
		pthread_mutex_unlock(&semMemoriaPrincipal);


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

		pthread_mutex_lock(&semMemoriaPrincipal);

		char* respuesta = DROPMemoria(unDROP->nombreTabla);
		enviarRespuesta(socketEmisor,id_respuesta_drop,respuesta);

		freeT_DROP(unDROP);
		pthread_mutex_lock(&semConfig);
		int retardoMemoriaPrincipal = configMemoria.retardoAccesoMemoriaPrincipal;
		pthread_mutex_unlock(&semConfig);
		sleep(retardoMemoriaPrincipal);
		pthread_mutex_unlock(&semMemoriaPrincipal);

	}
	if(packageRecibido->ID == 23){ // JOURNAL

		int id_respuesta_journal = 24;//23: RESPUESTA DE UN PROTOCOLO = 24

		log_info(loggerMemoria, "Query recibido: JOURNAL [%s]",packageRecibido->Datos);

		pthread_mutex_lock(&semMemoriaPrincipal);

		JOURNALMemoria();
		enviarRespuesta(socketEmisor,id_respuesta_journal,"Todo ok");

		pthread_mutex_lock(&semConfig);
		int retardoMemoriaPrincipal = configMemoria.retardoAccesoMemoriaPrincipal;
		pthread_mutex_unlock(&semConfig);
		sleep(retardoMemoriaPrincipal);
		pthread_mutex_unlock(&semMemoriaPrincipal);

	}
	if(packageRecibido->ID ==50){ //PIDO TABLA GOSSIP
		char* memoriasDondeEstoyConectado = memoriasTablaDeGossip(tablaDeGossipMemoria);
		log_info(loggerMemoria, "Ya envie la TABLA DE GOSSIP a KERNEL");
		enviarMemoriasTablaGossip(socketEmisor,KERNELOMEMORIA,memoriasDondeEstoyConectado);
		free(memoriasDondeEstoyConectado);

	}

}

void realizarGossip(){

	clock_t start, diff;
	int elapsedsec;
	int sec = configMemoria.tiempoGossiping; //aca debe ir configMemoria.tiempoGossiping
	int iterations = 0;

	while (iterations < 10000000) {
	   start = clock();

	   while (1) {
		   diff = clock() - start;
	       elapsedsec = diff / CLOCKS_PER_SEC;

	       if (elapsedsec >= sec) {
	    	   pthread_create(&clienteM, NULL,(void*) hacermeClienteDeMisServers, NULL);

	           iterations++;
	           break;
	       }
	   }
	}
}

void enviarAKernel(){
	char* memoriasEnTablaDeGossip = memoriasTablaDeGossip(tablaDeGossipMemoria);
	enviarMemoriasTablaGossip(kernel,KERNELOMEMORIA,memoriasEnTablaDeGossip);
	free(memoriasEnTablaDeGossip);
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
		log_error(loggerMemoria,"Fallo al establecer escucha, listen()");
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
	char* ipServidor = quitarComillas(ip);
	struct sockaddr_in dirServidorMemoria;

	dirServidorMemoria.sin_family = AF_INET;
	dirServidorMemoria.sin_addr.s_addr = inet_addr(ipServidor);
	dirServidorMemoria.sin_port = htons(puerto); //puerto al que va a escuchar
	int cliente = socket(AF_INET,SOCK_STREAM,0);
	if (connect (cliente, (void*)&dirServidorMemoria, sizeof(dirServidorMemoria))!=0){
		log_info(loggerMemoria,"La memoria con puerto %d e ip %s no se encuentra activa",puerto,ipServidor);
		borrarMemoriaSiEstaEnTablaGossip(ip, puerto);
		mostrarmeMemoriasTablaGossip(tablaDeGossipMemoria);
	}else{
		int numMemoria = configMemoria.numeroDeMemoria;
		char* soyMemoria = string_new();
		string_append(&soyMemoria, "SOY MEMORIA ");
		char* numeroMemoria= malloc (150);
		sprintf(numeroMemoria, "%d", numMemoria);
		string_append(&soyMemoria, numeroMemoria);

		realizarHandShake(cliente,KERNELOMEMORIA,soyMemoria);
		int nroMemoria = recibirHandShakeMemoria(cliente,KERNELOMEMORIA,loggerMemoria);

		if(nroMemoria!= -1){
			char* memoriasDondeEstoyConectado = memoriasTablaDeGossip(tablaDeGossipMemoria);
			printf("Memorias donde estoy conectado %s\n", memoriasDondeEstoyConectado);
			enviarMemoriasTablaGossip(cliente,KERNELOMEMORIA,memoriasDondeEstoyConectado);
			agregarATablaDeGossip(puerto, ip, nroMemoria,true, tablaDeGossipMemoria);
			mostrarmeMemoriasTablaGossip(tablaDeGossipMemoria);
			recibirMemoriasTablaDeGossip(cliente,KERNELOMEMORIA,loggerMemoria, tablaDeGossipMemoria);
			free(memoriasDondeEstoyConectado);
		}
	free(soyMemoria);
	free(numeroMemoria);
	}
	free(ipServidor);
	close(cliente);
}

void borrarMemoriaSiEstaEnTablaGossip(char* ip, int puerto){
	int tamanioTablaGossip = tamanioLista(tablaDeGossipMemoria);
	for (int i = 0 ; i< tamanioTablaGossip; i++){
		void* elemento = list_get(tablaDeGossipMemoria, i);
		t_memoriaTablaDeGossip *memoriaConectada =(t_memoriaTablaDeGossip*)elemento;
		if(memoriaConectada->puerto == puerto && string_equals_ignore_case(memoriaConectada->ip, ip)==1){
			memoriaConectada->conectado=false;

		}
	}
}


void conectarmeAEsaMemoria(int puerto,char* ip, t_log* logger){

	int cliente;
	struct sockaddr_in dirServidorMemoria;
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
		recibirHandShakeMemoria(cliente,KERNELOMEMORIA,loggerMemoria);
	}

}

void realizarMultiplexacion(int socketEscuchando){
	int fdmax;        // número máximo de descriptores de fichero
	int newfd;        // descriptor de socket de nueva conexión aceptada
	t_PaqueteDeDatos *package;
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
		for(int i = 0; i <= fdmax; i++) {
		    if (FD_ISSET(i, &copy)) { // ¡¡tenemos datos!!
		    	if (i == servidorEscuchaMemoria) {
					newfd = aceptarConexiones(socketEscuchando, loggerMemoria);
					int numMemoria = configMemoria.numeroDeMemoria;
					char* soyMemoria = string_new();
					string_append(&soyMemoria, "SOY MEMORIA ");
					char* numeroMemoria = malloc(1000);
					sprintf(numeroMemoria, "%d", numMemoria);
					string_append(&soyMemoria, numeroMemoria);
					realizarHandShake(newfd,KERNELOMEMORIA,soyMemoria);
					recibirHandShakeMemoria(newfd,KERNELOMEMORIA,loggerMemoria);

					char* memoriasQueTengo = memoriasTablaDeGossip(tablaDeGossipMemoria);
					recibirMemoriasTablaDeGossip(newfd, KERNELOMEMORIA, loggerMemoria, tablaDeGossipMemoria);
					mostrarmeMemoriasTablaGossip(tablaDeGossipMemoria);
					enviarMemoriasTablaGossip(newfd, KERNELOMEMORIA, memoriasQueTengo);
					free(soyMemoria);
					free(numeroMemoria);
					free(memoriasQueTengo);
					FD_SET(newfd,&master);
					fdmax = (newfd > fdmax)?newfd:fdmax;
					FD_CLR(socketEscuchando,&copy);
		    	} else {

		    		package = recibirPaquete(i);

		    		if(package->ID==0){
	                    // conexión cerrada
		    			free(package);
	    				printf("El socket %d se desconecto\n", i);
	                    close(i); // bye!
	                    FD_CLR(i, &master); // eliminar del conjunto maestro
		    		} else if(package->ID<0){
		    			free(package);
	                    perror("recv");
	                    close(i); // bye!
	                    FD_CLR(i, &master); // eliminar del conjunto maestro
		    		}
		    		else{

						printf("\npackage->ID: %d\n",package->ID);
						gestionarPaquetes(package, i);
						freePackage(package);
		    		}

		                    }
		                }
		            }
		        }

}

void hacermeClienteDeMisServers(){
	int cantidadSeeds = tamanioArray((void**)configMemoria.puertosDeSeeds);
	if(cantidadSeeds ==0){
	 log_info(loggerMemoria, "No tengo que conectarme a ninguna memoria");
	}else{
		for(int i = 0; i< cantidadSeeds; i++){
			serCliente(configMemoria.ipDeSeeds[i], (configMemoria.puertosDeSeeds[i]));
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


int conectarmeAlLFS() {
	char* ipServidor = quitarComillas(configMemoria.ipDelFileSystem);
	struct sockaddr_in dirServidorMemoria;
	int tamanioValue;
	dirServidorMemoria.sin_family = AF_INET;
	dirServidorMemoria.sin_addr.s_addr = inet_addr(ipServidor);
	dirServidorMemoria.sin_port = htons(configMemoria.puertoDelFileSystem); //puerto al que va a escuchar
	int cliente = socket(AF_INET,SOCK_STREAM,0);
	if (connect (cliente, (void*)&dirServidorMemoria, sizeof(dirServidorMemoria))!=0){
		log_info(loggerMemoria,"No me he podido conectar con el LFS");
	}else{
		char *msjEnviado = string_from_format("Memoria %d",configMemoria.numeroDeMemoria); //ACA EN VEZ DEL 1, IRIA EL NrO Q TIENE LA MEMORIA
		tamanioValue = realizarHandshakeAlLFS(loggerMemoria,cliente,msjEnviado);
		free(msjEnviado);
		}
	socketLFS= cliente;
	free(ipServidor);
	return tamanioValue;
}
