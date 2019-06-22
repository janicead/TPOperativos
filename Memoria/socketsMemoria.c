#include "socketsMemoria.h"

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
	    	   pthread_create(&clienteMemoria, NULL, (void*)hacermeClienteDeMisServers, NULL);

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


void iniciarEscuchaMemoria(){
	int yes=1;
    if ((servidorEscuchaMemoria = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    if (setsockopt(servidorEscuchaMemoria, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
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
    if (listen(servidorEscuchaMemoria, 10) == -1) {
        perror("listen");
        exit(1);
    }
    free(ipDeEscucha);
}
void serCliente(char* ip , int puerto){
	int cliente;
	char* ipServidor = quitarComillas(ip);
	struct sockaddr_in dirServidorMemoria;
	//struct sockaddr_in dirCliente;
	//unsigned int tamanioDireccion;
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
	close(cliente);
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


void realizarMultiplexacion(int socketEscuchando){
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
		                    if(valor!=0 && valor!=1313){
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
		                    	/////////////////////////////////////////////////////////
		                    	//aca se supone que le respondo al SELECT
		                    	/*t_PaqueteDeDatos* paquete = recibirPaquete(newfd);
		                    	printf("HOLA HOLA");
		                    			t_SELECT* SELECT=deserializarT_SELECT(paquete->Datos);
		                    			char* nombreTabla= malloc(sizeof(SELECT->nombreTabla));
		                    			strcpy(nombreTabla,SELECT->nombreTabla);
		                    			uint16_t k = (uint16_t) SELECT->KEY;
		                    			char* verificado = SELECTMemoria(nombreTabla,k,0);
		                    			empaquetarEnviarMensaje(newfd,13,sizeof(verificado),verificado);


		                    		free(paquete->Datos);
		                    		free(paquete);*/
		                    	////////////////////////////////////////////////////////

		                    	free(soyMemoria);
		                    	free(memoriasTablaGossip);
		                    }
		                    else {
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
		            // gestionar datos de un cliente
		    		if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
		                // error o conexión cerrada por el cliente
		    			if (nbytes == 0) {
		                    // conexión cerrada
		    				printf("El socket %d se desconecto\n", i);
		                } else {
		                    perror("recv");
		                }
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

}

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





