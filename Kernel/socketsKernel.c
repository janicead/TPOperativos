#include "socketsKernel.h"

void conectarAMemoria(char* ip, int puerto){
	int socketServer = conectarAlServidor(ip,puerto,loggerKernel);
	realizarHandShake(socketServer,KERNELOMEMORIA,"SOY KERNEL");
	int nroMemoria = recibirHandShakeMemoria(socketServer,KERNELOMEMORIA,loggerKernel);
	agregar_memoria(puerto, ip, nroMemoria);
	agregar_socket_mem(nroMemoria,socketServer);

	char* memoriasDondeEstoyConectado = memoriasTablaDeGossip(tablaDeGossipKernel);
	printf("Memorias donde estoy conectado %s\n", memoriasDondeEstoyConectado);
	enviarMemoriasTablaGossip(socketServer,KERNELOMEMORIA,memoriasDondeEstoyConectado);
	recibirMemoriasTablaDeGossip(socketServer,KERNELOMEMORIA,loggerKernel, tablaDeGossipKernel);
	mostrarmeMemoriasTablaGossip(tablaDeGossipKernel);
	agregarAMemoriasConectadasAKernel(puerto, ip, nroMemoria);
	conectarmeAMemorias();
	mostrarmeMemoriasTablaGossip(memoriasALasQueMeConecte);
	free(memoriasDondeEstoyConectado);

}

void definirValorMP(){
	memoria= 0;
}

void conectarmeAMP(){
	int cliente;
	char* ipServidor = quitarComillas(configKernel.ip_memoria);
	struct sockaddr_in dirServidorMemoria;
	//struct sockaddr_in dirCliente;
	//unsigned int tamanioDireccion;
	dirServidorMemoria.sin_family = AF_INET;
	dirServidorMemoria.sin_addr.s_addr = inet_addr(ipServidor);
	dirServidorMemoria.sin_port = htons(configKernel.puerto_memoria); //puerto al que va a escuchar
	cliente = socket(AF_INET,SOCK_STREAM,0);

	if (connect (cliente, (void*)&dirServidorMemoria, sizeof(dirServidorMemoria))!=0){

		log_info(loggerKernel,"La memoria con puerto %d e ip %s no se encuentra activa",configKernel.puerto_memoria,ipServidor);
	}
	else {
		memoria = cliente;
		log_info(loggerKernel,"Me conecte a la memoria con puerto %d e ip %s",configKernel.puerto_memoria,ipServidor);
		realizarHandShake(cliente,KERNELOMEMORIA,"SOY KERNEL");
		int nroMemoria = recibirHandShakeMemoria(cliente,KERNELOMEMORIA,loggerKernel);
		recibirMemoriasTablaDeGossip(cliente,KERNELOMEMORIA,loggerKernel,tablaDeGossipKernel);
		agregarATablaDeGossipKernel(configKernel.puerto_memoria,ipServidor,nroMemoria);
		agregarAMemoriasConectadasAKernel(configKernel.puerto_memoria,ipServidor,nroMemoria);
		mostrarmeMemoriasTablaGossip(tablaDeGossipKernel);
	}
	free(ipServidor);
}

void pedirTablaGossip(int socketReceptor, int protocoloID, char *respuesta){
	char* stringSerializado;
	t_UnString *unString = definirT_UnString(respuesta);

	stringSerializado = serializarT_UnString(unString);
	int tamanioStructSerializado = sizeof(uint32_t) + unString->longString;
	empaquetarEnviarMensaje(socketReceptor,protocoloID,tamanioStructSerializado,stringSerializado);

	free(stringSerializado);
	freeT_UnString(unString);
}

void gossipDeKernel(){
	while(1){
		sleep(10);// aca deberia ir configKernel.tiempoGossiping
		int cantMemorias = list_size(memorias);
		if(cantMemorias!=0){
			t_memoria * memoria = random_memory(memorias);
			pedirTablaGossip(memoria->socket_mem, 50, "Dame tabla gossip");
			recibirMemoriasTablaDeGossipKernel(memoria->socket_mem,KERNELOMEMORIA,loggerKernel);
			mostrarmeMemoriasTablaGossip(tablaDeGossipKernel);
			conectarmeAMemorias();
		}
	}
}

void recibirMemoriasTablaDeGossipKernel(int emisor,t_identidad identidad, t_log* logger){
	t_PaqueteDeDatos* paquete=recibirPaquete(emisor);
	if(paquete->ID==identidad){
		t_handShake* punteroHandShake;
		punteroHandShake=deserializarHandShake(paquete->Datos);
		int verificado = verificarMensajeMemoriasTablaGossipKernel(punteroHandShake->mensaje,logger);
		if(identidad==KERNELOMEMORIA && verificado!=0){
			char* soyMemoria = string_new();
			string_append(&soyMemoria, "Recibi las memorias conectadas correctamente");
			log_info(logger,soyMemoria);
			free(soyMemoria);
			free(punteroHandShake->mensaje);
			free(punteroHandShake);
		}
		else if(identidad == KERNELOMEMORIA && verificado ==0){
			log_info(logger,"Esta memoria no posee seeds");
			free(punteroHandShake->mensaje);
			free(punteroHandShake);

		}
	}
	else{
		log_error(logger,"ID incorrecto");
		free(paquete);
		return;
	}

	free(paquete->Datos);
	free(paquete);
}

int verificarMensajeMemoriasTablaGossipKernel(char* mensaje, t_log* logger){
	char** memorias= string_split(mensaje, " ");
	int cantElementos = tamanioArray((void**)memorias);
	int cantMemorias = cantElementos /3;

	int i = 0;
	int j = 0;
	if(cantMemorias == 0){
		free(memorias);
		return 0;
	} else{
		while(j<cantMemorias){
			agregarATablaDeGossipKernel(atoi(memorias[i+1]), memorias[i],atoi(memorias[i+2]));
			i= i+3;
			j++;
		}
	hacerFreeArray((void**)memorias);
	free(memorias);
	return cantMemorias;
	}
}

void agregarATablaDeGossipKernel(int puerto, char* ipServidor, int memoria){
	t_memoriaTablaDeGossip * memoriaConectada = malloc (sizeof(t_memoriaTablaDeGossip));
	memoriaConectada->puerto = puerto;
	memoriaConectada->ip = (char*) malloc((strlen(ipServidor)+1)*sizeof(char));
	memoriaConectada->numeroDeMemoria = memoria;
	strcpy(memoriaConectada->ip,ipServidor);
	if(revisarQueNoEsteEnLaLista(memoria, tablaDeGossipKernel)==1){
		list_add(tablaDeGossipKernel, (void *)memoriaConectada);
	}
	else{
		free(memoriaConectada->ip);
		free(memoriaConectada);
	}
}

void conectarmeAMemorias(){
	int cantMemoriasTablaGossip= cantMemoriasTablaDeGossip(tablaDeGossipKernel);
	//int cantidadMemoriasConectadas = cantMemoriasConectadas();
	if(cantMemoriasTablaGossip==0){
		return;
	}
	for(int i = 0; i<cantMemoriasTablaGossip; i++){
		void * elemento = list_get(tablaDeGossipKernel, i);
		t_memoriaTablaDeGossip *memoriaTablaGossip =(t_memoriaTablaDeGossip*)elemento;
		int nroMemoriaTablaGossip = memoriaTablaGossip->numeroDeMemoria;
		if(revisarQueNoEsteEnListaMemoriasConectadas(nroMemoriaTablaGossip)==1){
			conectarAMemoria(memoriaTablaGossip->ip, memoriaTablaGossip->puerto);
			agregarAMemoriasConectadasAKernel(memoriaTablaGossip->puerto,memoriaTablaGossip->ip, memoriaTablaGossip->numeroDeMemoria);
		}
	}
}

void conectarmeAMemoriaEspecifica(int puerto,char* ipServidor, t_log* logger){

	int cliente;
	char* ip = quitarComillas(ipServidor);
	struct sockaddr_in dirServidorMemoria;
	dirServidorMemoria.sin_family = AF_INET;
	dirServidorMemoria.sin_addr.s_addr = inet_addr(ip);
	dirServidorMemoria.sin_port = htons(puerto); //puerto al que va a escuchar
	cliente = socket(AF_INET,SOCK_STREAM,0);
	if (connect (cliente, (void*)&dirServidorMemoria, sizeof(dirServidorMemoria))!=0){
		log_info(logger,"La memoria con puerto %d e ip %s no se encuentra activa",puerto,ip);
	}
	else {
		realizarHandShake(cliente,KERNELOMEMORIA,"SOY KERNEL");
		recibirHandShakeMemoria(cliente,KERNELOMEMORIA,loggerKernel);
	}
	free(ip);
}

void agregarAMemoriasConectadasAKernel(int puerto, char* ipServidor, int memoria){
	t_memoriaTablaDeGossip * memoriaConectada = malloc (sizeof(t_memoriaTablaDeGossip));
	memoriaConectada->puerto = puerto;
	memoriaConectada->ip = (char*) malloc((strlen(ipServidor)+1)*sizeof(char));
	memoriaConectada->numeroDeMemoria = memoria;
	strcpy(memoriaConectada->ip,ipServidor);
	if(revisarQueNoEsteEnListaMemoriasConectadas(memoria)!=0){
		list_add(memoriasALasQueMeConecte , (void *)memoriaConectada);
	}
	else{
		free(memoriaConectada->ip);
		free(memoriaConectada);
	}
}

int cantMemoriasConectadas(){
	return  list_size(memoriasALasQueMeConecte);
}

int revisarQueNoEsteEnListaMemoriasConectadas(int nroMemoria){
	int cantMemoriasALasQueMeConecte = cantMemoriasConectadas();
	if(cantMemoriasALasQueMeConecte==0){
		return 2;
	}
	for(int i = 0 ; i<cantMemoriasALasQueMeConecte;i ++){

		void * elemento = list_get(memoriasALasQueMeConecte, i);
		t_memoriaTablaDeGossip *memoriaTablaGossip =(t_memoriaTablaDeGossip*)elemento;
		if(nroMemoria ==memoriaTablaGossip->numeroDeMemoria ){
			return 0;
		}
	}
	return 1;
}
