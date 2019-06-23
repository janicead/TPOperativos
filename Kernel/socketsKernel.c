#include "socketsKernel.h"

void definirValorMP(){
	memoria= 0;
}
void conectarmeAMP(){

	int cliente;
	char* ipServidor = quitarComillas(configKernel.ip_memoria);
	struct sockaddr_in dirServidorMemoria;
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
		recibirMemoriasTablaDeGossip(cliente,KERNELOMEMORIA,loggerKernel);
		agregarATablaDeGossipKernel(configKernel.puerto_memoria,ipServidor,nroMemoria);
		agregar_memoria(configKernel.puerto_memoria,ipServidor,nroMemoria);
		pthread_mutex_lock(&memorias_sem);
		t_memoria* mem = obtener_memoria_por_id(nroMemoria);
		mem->socket_mem = cliente;
		mem->conectada = true;
		pthread_mutex_unlock(&memorias_sem);
		mostrarmeMemoriasTablaGossip();
	}
	free(ipServidor);

}
void recibirMemorias(){
	while(1){
		if(memoria!=0){
			recibirMemoriasTablaDeGossipKernel(memoria,KERNELOMEMORIA);
			mostrarmeMemoriasTablaGossip();
			conectarmeAMemorias();
		}
	}

}
void recibirMemoriasTablaDeGossipKernel(int emisor,t_identidad identidad){
	t_PaqueteDeDatos* paquete=recibirPaquete(emisor);
	if(paquete->ID==identidad){
		t_handShake* punteroHandShake;
		punteroHandShake=deserializarHandShake(paquete->Datos);
		int verificado = verificarMensajeMemoriasTablaGossipKernel(punteroHandShake->mensaje);
		if(identidad==KERNELOMEMORIA && verificado!=0){
			char* soyMemoria = string_new();
			string_append(&soyMemoria, "Recibi las memorias conectadas correctamente");
			log_info(loggerKernel,soyMemoria);
			free(soyMemoria);
			free(punteroHandShake->mensaje);
			free(punteroHandShake);
		}
		else if(identidad == KERNELOMEMORIA && verificado ==NULL){
			log_info(loggerKernel,"Esta memoria no posee seeds");
			free(punteroHandShake->mensaje);
			free(punteroHandShake);
		}
	}
	else{
		log_error(loggerKernel,"ID incorrecto");
	}
	free(paquete->Datos);
	free(paquete);
}

int verificarMensajeMemoriasTablaGossipKernel(char* mensaje){
	char** memorias= string_split(mensaje, " ");
	int cantElementos = tamanioArray((void*)memorias);
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
	hacerFreeArray((void*)memorias);
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
	if(revisarQueNoEsteEnLaLista(memoria)==1){
		list_add(tablaDeGossip, (void *)memoriaConectada);
	}
	else{
		free(memoriaConectada->ip);
		free(memoriaConectada);
	}
}

void conectarmeAMemorias(){
	pthread_mutex_lock(&memorias_sem);
	int cantMemorias= list_size(memorias);
	pthread_mutex_unlock(&memorias_sem);
	if(cantMemorias==0){
		return;
	}
	for(int i = 0; i<cantMemorias; i++){
		pthread_mutex_lock(&memorias_sem);
		t_memoria* mem = list_get(memorias, i);
		if(!mem->conectada){
			int socket = conectarmeAMemoriaEspecifica(mem->puerto,mem->ip);
			mem->conectada = true;
			agregar_socket_mem(mem->id_mem,socket);
		}
		pthread_mutex_unlock(&memorias_sem);
	}
}

int conectarmeAMemoriaEspecifica(int puerto,char* ipServidor){
	int cliente;
	char* ip = quitarComillas(ipServidor);
	struct sockaddr_in dirServidorMemoria;
	dirServidorMemoria.sin_family = AF_INET;
	dirServidorMemoria.sin_addr.s_addr = inet_addr(ip);
	dirServidorMemoria.sin_port = htons(puerto); //puerto al que va a escuchar
	cliente = socket(AF_INET,SOCK_STREAM,0);
	if (connect (cliente, (void*)&dirServidorMemoria, sizeof(dirServidorMemoria))!=0){
		log_info(loggerKernel,"La memoria con puerto %d e ip %s no se encuentra activa",puerto,ip);
	}
	else {
		realizarHandShake(cliente,KERNELOMEMORIA,"SOY KERNEL");
		recibirHandShakeMemoria(cliente,KERNELOMEMORIA,loggerKernel);
	}
	free(ip);
	return cliente;
}
