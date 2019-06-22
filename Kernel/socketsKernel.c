#include "socketsKernel.h"

void definirValorMP(){
	memoria= 0;
}

void conectarmeAMP(){
	char* ipServidor = quitarComillas(configKernel.ip_memoria);
	struct sockaddr_in dirServidorMemoria;
	dirServidorMemoria.sin_family = AF_INET;
	dirServidorMemoria.sin_addr.s_addr = inet_addr(ipServidor);
	dirServidorMemoria.sin_port = htons(configKernel.puerto_memoria); //puerto al que va a escuchar
	socketKernel = socket(AF_INET,SOCK_STREAM,0);

	if (connect (socketKernel, (void*)&dirServidorMemoria, sizeof(dirServidorMemoria))!=0){
		log_info(loggerKernel,"La memoria con puerto %d e ip %s no se encuentra activa",configKernel.puerto_memoria,ipServidor);
	}
	else {
		memoria = socketKernel;
		log_info(loggerKernel,"Me conecte a la memoria con puerto %d e ip %s",configKernel.puerto_memoria,ipServidor);
		realizarHandShake(socketKernel,KERNELOMEMORIA,"SOY KERNEL");
		int nroMemoria = recibirHandShakeMemoria(socketKernel,KERNELOMEMORIA,loggerKernel);
		recibirMemoriasTablaDeGossip(socketKernel,KERNELOMEMORIA,loggerKernel);
		agregar_memoria(configKernel.puerto_memoria,ipServidor,nroMemoria);
	}
	free(ipServidor);
}

void recibirMemorias(){
	while(1){
		if(memoria!=0){
			recibirMemoriasTablaDeGossipKernel(memoria,KERNELOMEMORIA,loggerKernel);
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
		else if(identidad == KERNELOMEMORIA && verificado == 0){
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

int verificarMensajeMemoriasTablaGossipKernel(char* mensaje, t_log* logger){
	char** memorias= string_split(mensaje, " ");
	int cantElementos = tamanioArray((void**)memorias);
	int cantMemorias = cantElementos /3;

	int i = 0;
	int j = 0;
	if(cantMemorias == 0){
		free(memorias);
		return 0;
	}
	else{
		while(j<cantMemorias){
			agregar_memoria(atoi(memorias[i+1]), memorias[i],atoi(memorias[i+2]));
			i= i+3;
			j++;
		}
		hacerFreeArray((void**)memorias);
		free(memorias);
		return cantMemorias;
	}
}

void conectarmeAMemorias(){
	int cantMemorias= list_size(memorias);
	if(cantMemorias==0){
		return;
	}
	for(int i = 0; i<cantMemorias; i++){
		t_memoria* mem = list_get(tablaDeGossip, i);
		if(!mem->conectada){
			conectarmeAMemoriaEspecifica(mem->puerto,mem->ip);
			mem->conectada = true;
		}
	}
}

void conectarmeAMemoriaEspecifica(int puerto,char* ipServidor){
	char* ip = quitarComillas(ipServidor);
	struct sockaddr_in dirServidorMemoria;
	dirServidorMemoria.sin_family = AF_INET;
	dirServidorMemoria.sin_addr.s_addr = inet_addr(ip);
	dirServidorMemoria.sin_port = htons(puerto); //puerto al que va a escuchar
	if (connect (socketKernel, (void*)&dirServidorMemoria, sizeof(dirServidorMemoria))!=0){
		log_info(loggerKernel,"La memoria con puerto %d e ip %s no se encuentra activa",puerto,ip);
	}
	else {
		realizarHandShake(socketKernel,KERNELOMEMORIA,"SOY KERNEL");
		recibirHandShakeMemoria(socketKernel,KERNELOMEMORIA,loggerKernel);
	}
	free(ip);
}

