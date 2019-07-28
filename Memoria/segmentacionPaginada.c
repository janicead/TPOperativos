#include "segmentacionPaginada.h"

//----------------------------------------GENERALES-------------------------------------------//

void definirTamanioMemoriaPrincipal(int tamanioValue){
	tamanioMaxMemoria = configMemoria.tamanioMemoria;
	tamanioUnRegistro = tamanioDadoPorLFS + sizeof(unsigned long int) + sizeof(uint16_t); //6+ tamanioValueDado
	obtenerValue = sizeof(unsigned long int) + sizeof(uint16_t);
	memoriaPrincipal = malloc(tamanioMaxMemoria);
	log_info(loggerMemoria,"El tamanio de un registro es de %d\n", tamanioUnRegistro);
	cantMaxMarcos = tamanioMaxMemoria/ tamanioUnRegistro;
	cantMarcosIngresados= 0;
	log_info(loggerMemoria,"Cant max de marcos posibles %d\n", cantMaxMarcos);
	marcosOcupados=(int*)calloc(cantMaxMarcos,sizeof(int));
	tablaDeSegmentos= list_create();
	iniciarSemaforos();
}

void iniciarSemaforos(){
	pthread_mutex_init(&semTablaSegmentos,NULL);
	pthread_mutex_init(&semCantMaxMarcos,NULL);
	pthread_mutex_init(&semCantMarcosIngresados,NULL);
	pthread_mutex_init(&semMarcosOcupados,NULL);
	pthread_mutex_init(&semMP, NULL);
	pthread_mutex_init(&semMemoriaPrincipal,NULL);
	pthread_mutex_init(&semLfs, NULL);
	pthread_mutex_init(&tamPorLFS,NULL);
}

int tamanioLista(t_list * lista){
	return list_size(lista);
}

void mostrarDatosMarcos(){
	for(int i = 0; i<cantMaxMarcos; i ++){
		printf("Marco %d : %d\n" ,i, marcosOcupados[i]);
	}
}

void reacomodarNumerosDePaginas(){
	int tamanioPag = 0;
	int t = tamanioLista(tablaDeSegmentos);
	if(t==0){
		return;
	}
	for(int i = 0 ; i <t ; i ++){
		void * elemento = list_get(tablaDeSegmentos, i);
		t_segmento * segmento = (t_segmento*) elemento;
		tamanioPag = tamanioLista(segmento->tablaPaginas);
		for(int j = 0; j < tamanioPag; j++){
			void* elemento2 = list_get(segmento->tablaPaginas, j);
			t_pagina * pagina = (t_pagina*)elemento2;
			pagina->numeroPag=j;
		}
	}
}

//-------------------------------------BORRADO---------------------------------------------------//

void destructor2(t_pagina * pagina){
	free(pagina);
}
void destructor(t_segmento* segmento){
	list_destroy_and_destroy_elements(segmento->tablaPaginas, (void*)destructor2);
	free(segmento);
}
void borrarElementos(){
		pthread_mutex_lock(&semTablaSegmentos);
		list_destroy_and_destroy_elements(tablaDeSegmentos, (void*)destructor);
		pthread_mutex_unlock(&semTablaSegmentos);
}

void borrarTodo(){

	pthread_mutex_lock(&semMarcosOcupados);
	pthread_mutex_lock(&semCantMaxMarcos);
	for(int i = 0; i< cantMaxMarcos; i ++){
		marcosOcupados[i]=0;
		}
	pthread_mutex_unlock(&semCantMaxMarcos);
	pthread_mutex_unlock(&semMarcosOcupados);
	borrarElementos();

}

//------------------------------------------SEGMENTOS--------------------------------------------------------//

int buscarTablaSegmentos(char* nombreTabla){
	int cantSegmentos =  tamanioLista(tablaDeSegmentos);
	for(int i = 0; i<cantSegmentos ; i++){
		void * elemento = list_get(tablaDeSegmentos, i);
		t_segmento *segmento =(t_segmento*)elemento;
		if(string_equals_ignore_case(nombreTabla, segmento->nombreTabla)){
			return i;
		}
	}
	return -1;
}

t_segmento* guardarEnTablaDeSegmentos(char* nombreTabla){
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->nombreTabla = malloc(strlen(nombreTabla)+1);
	strcpy(segmento->nombreTabla, nombreTabla);
	segmento->tablaPaginas= list_create();
	segmento->id =0;
	//pthread_mutex_lock(&semTablaSegmentos);
	list_add(tablaDeSegmentos, (void*)segmento);
	//pthread_mutex_unlock(&semTablaSegmentos);
	return segmento;
}

void mostrarElementosTablaSegmentos(){
	int tamanioTS= tamanioLista(tablaDeSegmentos);
	if(tamanioTS==0){
		log_info(loggerMemoria,"No hay SEGMENTOS en la TABLA DE SEGMENTOS");
	}
	for(int i = 0; i < tamanioTS; i++){
		void* elemento = list_get(tablaDeSegmentos, i);
		t_segmento* segmento = (t_segmento*) elemento;
		log_info(loggerMemoria, "NOMBRE DE TABLA: '%s'\n",segmento->nombreTabla);
		mostrarElementosTablaPaginas(segmento->tablaPaginas);
	}
}
//----------------------------------------PAGINAS----------------------------------------------------------//

char* buscarTablaPaginas(t_list* tabla, uint16_t key){

	int cantPaginas = tamanioLista(tabla);
	for(int i = 0; i<cantPaginas; i++){
		void * elemento = list_get(tabla, i);
		t_pagina *pagina =(t_pagina*)elemento;
		if(pagina->key ==key){
		t_registro* registro = buscarEnMemoriaPrincipal(pagina->numeroMarco);
		pagina->contadorVecesSolicitado++;
		log_info(loggerMemoria, "Cantidad veces solicitado %d\n", pagina->contadorVecesSolicitado);
		char* valor = malloc(tamanioDadoPorLFS);
		strcpy(valor, registro->value);
		free(registro->value);
		free(registro);
		return valor;
		}
	}
	return NULL; // SI retorna null significa que no esta en tabla de paginas y hay que consultarle a FS y agregarlo
}

void guardarEnTablaDePaginas(t_segmento * segmento, int nroMarco,uint16_t key, int flagModificado ){
	t_pagina * pagina = malloc(sizeof(t_pagina));
	pagina->contadorVecesSolicitado=0;
	pagina->flagModificado= flagModificado;
	pagina->key = key;
	pagina->numeroMarco= nroMarco;
	pagina->numeroPag= segmento->id;
	segmento->id = pagina->numeroPag +1;
	list_add(segmento->tablaPaginas,(void*)pagina);
}

void mostrarElementosTablaPaginas(t_list * lista){
	int tamanioTP= tamanioLista(lista);
	if(tamanioTP==0){
		log_info(loggerMemoria,"No hay PAGINAS en la TABLA DE PAGINAS");
	}
	for(int i = 0 ; i<tamanioTP; i++){
		void* elemento = list_get(lista, i);
		t_pagina* pagina = (t_pagina*) elemento;
		log_info(loggerMemoria,"NUMERO DE PAG %d, SU KEY ES %d y su INDICE EN MEMORIA ES %d\n, SU FLAG ES %d y SU CANTIDAD SOLICITADO ES %d", pagina->numeroPag, pagina->key, pagina->numeroMarco, pagina->flagModificado, pagina->contadorVecesSolicitado);
	}
}

int buscarEnTablaPaginasINSERT(t_list* tabla, uint16_t key,int timeStamp , char* value){

	int cantPaginas = tamanioLista(tabla);
	for(int i = 0; i<cantPaginas; i++){
		void * elemento = list_get(tabla, i);
		t_pagina *pagina =(t_pagina*)elemento;
		if(pagina->key ==key){
			t_registro * registro = buscarEnMemoriaPrincipal(pagina->numeroMarco);
			if(timeStamp>=registro->timestamp){ // asi lo modifica si se llega a dar el caso q se hace en el mismo momento
				actualizarMemoriaPrincipal(pagina->numeroMarco, timeStamp, value);
				pagina->flagModificado=1;
				log_info(loggerMemoria,"Se actualizo la MEMORIA PRINCIPAL");
			}
			free(registro->value);
			free(registro);
			return timeStamp;
		}
	}
	return 0;
}

void destructorPaginas(void* elemento){
	t_pagina* pagina = (t_pagina*)elemento;
	free(pagina);
}

void borrarTablaDePaginas(t_list* lista){
	list_destroy_and_destroy_elements(lista, destructorPaginas);
}

//------------------------------------------MEMORIA------------------------------------------//

t_registro* buscarEnMemoriaPrincipal( int nroMarco){
	t_registro * registro = malloc(tamanioUnRegistro);
	int copiarDesde = 0;
	registro->value = malloc(tamanioDadoPorLFS);
	memcpy(&registro->key, memoriaPrincipal + tamanioUnRegistro * nroMarco + copiarDesde,sizeof(uint16_t));
	copiarDesde += sizeof(uint16_t);
	memcpy(&registro->timestamp, memoriaPrincipal+tamanioUnRegistro * nroMarco+ copiarDesde, sizeof(unsigned long int));
	copiarDesde += sizeof(unsigned long int);
	memcpy(registro->value, memoriaPrincipal + tamanioUnRegistro * nroMarco+ copiarDesde,tamanioDadoPorLFS);
	return registro;
}

int buscarEspacioLibreEnMP(){
	pthread_mutex_lock(&semCantMaxMarcos);
	for(int i = 0; i< cantMaxMarcos; i ++){
		if(marcosOcupados[i]==0){
			pthread_mutex_unlock(&semCantMaxMarcos);
			return i;
		}
	}
	pthread_mutex_unlock(&semCantMaxMarcos);
	return cantMaxMarcos;
}

void settearMarcoEnMP(int nroMarco, int nroDeseado){
	pthread_mutex_lock(&semMarcosOcupados);
	marcosOcupados[nroMarco]=nroDeseado;
	pthread_mutex_unlock(&semMarcosOcupados);
}

void actualizarMemoriaPrincipal(int nroMarco, unsigned long int timeStamp, char* value){
	t_registro * registro = malloc(tamanioUnRegistro);
	registro->value= malloc(tamanioDadoPorLFS);
	strcpy(registro->value, value);
	registro->timestamp= timeStamp;
	pthread_mutex_lock(&semMP);
	memcpy(memoriaPrincipal+ nroMarco*tamanioUnRegistro+sizeof(uint16_t), &registro->timestamp, sizeof(unsigned long int));
	memcpy(memoriaPrincipal+nroMarco*tamanioUnRegistro+sizeof(uint16_t)+ sizeof(unsigned long int), registro->value, tamanioDadoPorLFS);
	pthread_mutex_unlock(&semMP);
	free(registro->value);
	free(registro);
}

void guardarEnMPLugarEspecifico(uint16_t key, char* value, int nroMarco, unsigned long int timestamp){
	t_registro * registro = malloc(tamanioUnRegistro);
	registro->key = key;
	registro->value= malloc(tamanioDadoPorLFS);
	strcpy(registro->value, value);
	registro->timestamp= timestamp;
	pthread_mutex_lock(&semMP);
	memcpy(memoriaPrincipal+ nroMarco*tamanioUnRegistro, &registro->key, sizeof(uint16_t));
	memcpy(memoriaPrincipal+ nroMarco*tamanioUnRegistro+sizeof(uint16_t), &registro->timestamp, sizeof(unsigned long int));
	memcpy(memoriaPrincipal+nroMarco*tamanioUnRegistro+sizeof(uint16_t)+ sizeof(unsigned long int), registro->value, tamanioDadoPorLFS);
	pthread_mutex_unlock(&semMP);
	pthread_mutex_lock(&semCantMarcosIngresados);
	cantMarcosIngresados++;
	pthread_mutex_unlock(&semCantMarcosIngresados);
	settearMarcoEnMP(nroMarco, 1);
	free(registro->value);
	free(registro);
}

int guardarEnMemoria(char* nombreTabla, uint16_t key, char* value, unsigned long int timestamp){
	int nroMarco = buscarEspacioLibreEnMP();
	pthread_mutex_lock(&semCantMaxMarcos);
	if(nroMarco!=cantMaxMarcos){
		guardarEnMPLugarEspecifico(key, value, nroMarco, timestamp);
		pthread_mutex_unlock(&semCantMaxMarcos);
			return nroMarco;
	}
	else{
		t_LRU * lru = LRU();
		if(lru->numeroPag != -1){
			pthread_mutex_unlock(&semCantMaxMarcos);
			t_pagina*  pagina = eliminarElementoEspecifico(lru->tablaPaginas, lru->numeroPag);
			int t = tamanioLista(lru->tablaPaginas);
			if(t==0){
				pthread_mutex_unlock(&semTablaSegmentos);
				DROPMemoriaExclusivoLRU(lru->nombreTabla);
				pthread_mutex_lock(&semTablaSegmentos);
			}
			t_registro* registro = buscarEnMemoriaPrincipal(pagina->numeroMarco);
			log_info(loggerMemoria,"La PAGINA que voy a reemplazar es la nro %d del SEGMENTO '%s' cuyo value es '%s' ", lru->numeroPag, lru->nombreTabla, registro->value);
			settearMarcoEnMP(pagina->numeroMarco, 0);
			guardarEnMPLugarEspecifico(key, value, pagina->numeroMarco, timestamp);
			free(lru->nombreTabla);
			free(lru);
			int nroMarco = pagina->numeroMarco;
			free(pagina);
			free(registro->value);
			free(registro);
			return nroMarco;
		}
		else {
			pthread_mutex_unlock(&semCantMaxMarcos);
			log_info(loggerMemoria,"Tengo que realizar JOURNAL\n");
			free(lru);
			return -1;
		}
	}
}

t_pagina* eliminarElementoEspecifico(t_list* lista, int nroPag){
	int t = tamanioLista(lista);
	for(int i = 0; i <t; i++){
		void* elemento = list_get(lista, i);
		t_pagina* p = (t_pagina*) elemento;
		if(p->numeroPag == nroPag){
			void*elemento2 = list_remove(lista, i);
			t_pagina* p2 = (t_pagina*) elemento2;
			return p2;
		}
	}
	return NULL;
}
void mostrarElementosMemoriaPrincipal(){
	 uint16_t key = 0;
	 unsigned long int eltimestamp =0;
	 char* elvalue = malloc(tamanioDadoPorLFS);
	 puts("----------------------MOSTRANDO DATOS MEMORIA----------------------");
	 int copiarDesde = 0;
	 int valor = cantMaxMarcos;
	 if(cantMarcosIngresados<cantMaxMarcos){
		 valor= cantMarcosIngresados;
	 }
	 for(int i = 0 ; i <valor; i++){
		 printf("el i es %d\n", i);
		 memcpy(&key, memoriaPrincipal + tamanioUnRegistro * i + copiarDesde,sizeof(uint16_t));
		 copiarDesde += sizeof(uint16_t);
		 memcpy(&eltimestamp, memoriaPrincipal+tamanioUnRegistro * i+ copiarDesde, sizeof(unsigned long int));
		 copiarDesde += sizeof(unsigned long int);
		 memcpy(elvalue, memoriaPrincipal + tamanioUnRegistro * i+ copiarDesde,tamanioDadoPorLFS);
		 printf("LA KEY ES %u\n",(unsigned int)key);
		 printf("EL TIMESTAMP ES %lu\n",eltimestamp);
		 printf("EL VALUE ES '%s'\n",elvalue);
		 puts("--------------------------------------------");
		 copiarDesde = 0;
	 }
	 free(elvalue);
}

void quitarEspaciosGuardadosEnMemoria(t_list* lista){
	int tamanio = tamanioLista(lista);
	for(int i =0 ; i< tamanio; i++){
		void * elemento = list_get(lista, i);
		t_pagina * pagina = (t_pagina *) elemento;
		settearMarcoEnMP(pagina->numeroMarco,0);
	}
	log_info(loggerMemoria,"Se borraron espacios guardados en Memoria Principal\n");
}
void borrarTodaMemoria(){
	pthread_mutex_lock(&semCantMaxMarcos);
	for(int i = 0; i<cantMaxMarcos; i++){
		settearMarcoEnMP(i, 0);
	}
	pthread_mutex_unlock(&semCantMaxMarcos);
	pthread_mutex_lock(&semMP);
	memset(memoriaPrincipal, 0, tamanioMaxMemoria);
	pthread_mutex_unlock(&semMP);
}
//---------------------------------------LRU-------------------------------------------------------//

t_LRU * LRU (){
	int cantVecesSolicitadaMinimo = INT_MAX;
	t_LRU * lru = malloc (sizeof(t_LRU));
	lru->numeroPag= -1;
	int tamanioTablaPaginas = 0;
	int tamanioTablaSegmentos = tamanioLista(tablaDeSegmentos);
	for(int i = 0; i< tamanioTablaSegmentos; i++){

		void * elemento = list_get(tablaDeSegmentos, i);
		t_segmento *segmento =(t_segmento*)elemento;
		tamanioTablaPaginas = tamanioLista(segmento->tablaPaginas);
		lru->posicionSegmento= i;
		for(int j = 0; j<tamanioTablaPaginas; j++){

			void * elemento = list_get(segmento->tablaPaginas, j);
			t_pagina *pagina =(t_pagina*)elemento;

			if(pagina->flagModificado==0 && (pagina->contadorVecesSolicitado)<cantVecesSolicitadaMinimo){
				cantVecesSolicitadaMinimo = pagina->contadorVecesSolicitado;
				lru->numeroPag= pagina->numeroPag;
				lru->tablaPaginas= segmento->tablaPaginas;
				lru->nombreTabla= malloc(strlen(segmento->nombreTabla)+1);
				strcpy(lru->nombreTabla, segmento->nombreTabla);
			}
		}
	}
	return lru;
}

//-------------------------------------JOURNAL----------------------------------------------------//

void mostrarElementosListaJournal(){
	int tamanioTP= tamanioLista(listaJournal);
	printf("CANT ELEMENTOS JOURNAL %d\n", tamanioTP);
	for(int i = 0 ; i<tamanioTP; i++){
		void* elemento = list_get(listaJournal, i);
		t_JOURNAL* journal = (t_JOURNAL*) elemento;
		printf("NOMBRE TABLA es '%s', su KEY es %u, su TIMESTAMP %lu y su VALUE es %s\n", journal->nombreTabla, (unsigned int)journal->registro->key, journal->registro->timestamp, journal->registro->value);
	}
}

char* convertirAStringListaJournal(){
	int tamanioJOURNAL= tamanioLista(listaJournal);
	char* elementoEnviar = string_new();

	for (int i = 0; i<tamanioJOURNAL; i ++){
		void* elemento = list_get(listaJournal, i);
		t_JOURNAL* journal = (t_JOURNAL*) elemento;
		char *unRegistro= string_from_format("%s %d %s %lu/",journal->nombreTabla,journal->registro->key,journal->registro->value,journal->registro->timestamp);
		string_append(&elementoEnviar, unRegistro);
		free(journal);
		free(unRegistro);
	}
	log_info(loggerMemoria,"LA LISTA ES----> %s \n", elementoEnviar);

	return elementoEnviar;
}

void iniciarJournal(){
	int tamanioTablaSegmentos = tamanioLista(tablaDeSegmentos);
	for(int i = 0 ; i < tamanioTablaSegmentos; i ++){
		void * elemento = list_get(tablaDeSegmentos, i);
		t_segmento *segmento =(t_segmento*)elemento;

		int tamanioTablaPaginas = tamanioLista(segmento->tablaPaginas);
		for(int j = 0; j<tamanioTablaPaginas; j++){

			void * elemento = list_get(segmento->tablaPaginas, j);
			t_pagina * pagina = (t_pagina *) elemento;

			if(pagina->flagModificado ==1){
				t_registro* registro =	buscarEnMemoriaPrincipal(pagina->numeroMarco);
				int keyEnINT = pasarUINT16AInt(registro->key);
				if (socketLFS ==0){
					log_error(loggerMemoria, "El LFS esta DESCONECTADO, NO se podra realizar el JOURNAL");
					free(registro->value);
					free(registro);
					return;
				}
				char* value = opINSERT(socketLFS,segmento->nombreTabla, keyEnINT, registro->value, registro->timestamp);

				if(string_equals_ignore_case(value, "NO_EXISTE_TABLA")){
					log_error(loggerMemoria, "La tabla '%s' no existe", segmento->nombreTabla);
				}
				else if (string_equals_ignore_case(value, "MEMORIA_DESCONECTADA")){
					socketLFS= 0;
					log_error(loggerMemoria, "El LFS esta DESCONECTADO, NO se podra realizar el JOURNAL");
					free(registro->value);
					free(registro);
					free(value);
					return;

				}
				else{
					log_info(loggerMemoria, "Se ha guardado en la tabla '%s' con key '%d' el value '%s'", segmento->nombreTabla, keyEnINT, registro->value);
				}
				free(value);
				free(registro->value);
				free(registro);
			}

		}
}

	int t= tamanioLista(tablaDeSegmentos);
	for(int j = 0; j < t ; j++){
		void* elemento = list_get(tablaDeSegmentos, j);
		t_segmento* s = (t_segmento *) elemento;
		borrarTablaDePaginas(s->tablaPaginas);
		log_info(loggerMemoria, "Se borro la tabla de paginas %s \n", s->nombreTabla);
		free(s->nombreTabla);
		free(s);
	}
	list_clean(tablaDeSegmentos);
	borrarTodaMemoria();
	pthread_mutex_lock(&semCantMarcosIngresados);
	cantMarcosIngresados=0;
	pthread_mutex_unlock(&semCantMarcosIngresados);
}

void retardoMemoriaAplicado(){
	pthread_mutex_lock(&semMemoriaPrincipal);
	pthread_mutex_lock(&semConfig);
	int retardoMemoriaPrincipal = configMemoria.retardoAccesoMemoriaPrincipal;
	pthread_mutex_unlock(&semConfig);
	sleep(retardoMemoriaPrincipal);
	pthread_mutex_unlock(&semMemoriaPrincipal);
}

void retardoLFSAplicado(){
	pthread_mutex_lock(&semConfig);
	int retardoLFS= configMemoria.retardoAccesoFileSystem;
	pthread_mutex_unlock(&semConfig);
	sleep(retardoLFS);

}




//----------------------------------------------------REQUESTS-------------------------------------------------//

//-----------------------------------------------------SELECT-------------------------------------------------//
char* SELECTMemoria(char * nombreTabla, uint16_t key, int flagModificado){
	pthread_mutex_lock(&semTablaSegmentos);
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
	if(ubicacionSegmento!=-1){ //esta en tabla de segmentos
		log_info(loggerMemoria,"Esta en la tabla de SEGMENTOS");
		void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
		t_segmento *segmento =(t_segmento*)elemento;
		char* value = buscarTablaPaginas(segmento->tablaPaginas, key);// aca tenemos que buscar en la tabla de paginas especifica de este segmento y meternos 1 x 1 en sus paginas para ver si en la memoria Principal esta el key
		pthread_mutex_unlock(&semTablaSegmentos);
		if(value!= NULL){ //lo encontro en tabla de paginas, lo busca en memoria principal y devuelve lo que vale
			log_info(loggerMemoria,"Esta en la tabla de PAGINAS");
			log_info(loggerMemoria,value);
			return value;
		}
		else{ //no lo encontro en tabla de paginas
			//tengo que consultarle a LFS PERO solo guardo en tabla de paginas
			pthread_mutex_lock(&semLfs);
			int keyEnINT = pasarUINT16AInt(key);
			if (socketLFS ==0){
				pthread_mutex_unlock(&semLfs);
				return "LFS_CAIDO";
			}
			char* value = opSELECT(socketLFS,nombreTabla, keyEnINT);
			if(string_equals_ignore_case(value, "NO_EXISTE_TABLA")||string_equals_ignore_case(value, "NO_EXISTE_KEY")){
				retardoLFSAplicado();
				pthread_mutex_unlock(&semLfs);
				log_info(loggerMemoria,value);
				return value;
			} else if (string_equals_ignore_case(value, "MEMORIA_DESCONECTADA")){
				socketLFS= 0;
				pthread_mutex_unlock(&semLfs);
				free(value);
				return "LFS_CAIDO";
			}
			else{
			log_info(loggerMemoria,"No esta en la tabla de PAGINAS");
			unsigned long int t = obtenerTimeStamp();
			pthread_mutex_lock(&semTablaSegmentos);
			int nroMarco = guardarEnMemoria(nombreTabla, key, value, t);

			if(nroMarco == -1){
				reacomodarNumerosDePaginas();
				retardoLFSAplicado();
				pthread_mutex_unlock(&semLfs);
				pthread_mutex_unlock(&semTablaSegmentos);
				log_info(loggerMemoria,"FULL");
				free(value);
				return "FULL";
			}
			ubicacionSegmento = buscarTablaSegmentos(nombreTabla);
			if(ubicacionSegmento==-1){
				segmento = guardarEnTablaDeSegmentos(nombreTabla);
			}
			guardarEnTablaDePaginas(segmento, nroMarco, key, 0);
			reacomodarNumerosDePaginas();
			mostrarElementosTablaSegmentos();
			mostrarDatosMarcos();
			pthread_mutex_unlock(&semTablaSegmentos);

			retardoLFSAplicado();

			pthread_mutex_unlock(&semLfs);

			log_info(loggerMemoria,value);
			return value;
			}
		}
	}
	else{
		pthread_mutex_lock(&semLfs);
		log_info(loggerMemoria,"No se encontro en la tabla de SEGMENTOS");
		int keyEnINT = pasarUINT16AInt(key);
		printf("SOCKET LFS VALE %d\n", socketLFS);
		if (socketLFS ==0){
			pthread_mutex_unlock(&semLfs);
			pthread_mutex_unlock(&semTablaSegmentos);
			return "LFS_CAIDO";
		}
		char* value = opSELECT(socketLFS,nombreTabla, keyEnINT);
		if(string_equals_ignore_case(value, "NO_EXISTE_TABLA")||string_equals_ignore_case(value, "NO_EXISTE_KEY")){
			retardoLFSAplicado();
			pthread_mutex_unlock(&semLfs);
			pthread_mutex_unlock(&semTablaSegmentos);
			log_info(loggerMemoria,value);
			return value;
		} else if (string_equals_ignore_case(value, "MEMORIA_DESCONECTADA")){
			socketLFS= 0;
			pthread_mutex_unlock(&semLfs);
			pthread_mutex_unlock(&semTablaSegmentos);
			free(value);
			return "LFS_CAIDO";
		}
		else{
			unsigned long int t = obtenerTimeStamp();
			int nroMarco = guardarEnMemoria(nombreTabla, key, value, t);
			if(nroMarco == -1){
				reacomodarNumerosDePaginas();
				retardoLFSAplicado();
				pthread_mutex_unlock(&semLfs);
				pthread_mutex_unlock(&semTablaSegmentos);
				log_info(loggerMemoria,"FULL");
				free(value);
				return "FULL";
			}
			t_segmento* segmento = guardarEnTablaDeSegmentos(nombreTabla);
			guardarEnTablaDePaginas(segmento, nroMarco, key, 0);
			reacomodarNumerosDePaginas();
			pthread_mutex_unlock(&semTablaSegmentos);
			retardoLFSAplicado();
			pthread_mutex_unlock(&semLfs);
			log_info(loggerMemoria,"Se guardo en MP, en tabla de PAGINAS y en tabla de SEGMENTOS");
			return value;
		}

	}
}
//----------------------------------------------------INSERT--------------------------------------------------//

char* INSERTMemoria(char * nombreTabla, uint16_t key, char* value, unsigned long int timeStamp){
	pthread_mutex_lock(&semTablaSegmentos);
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
	if(ubicacionSegmento!=-1){ //esta en tabla de SEGMENTOS
		log_info(loggerMemoria,"Esta en la tabla de SEGMENTOS");
		void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
		t_segmento *segmento =(t_segmento*)elemento;
		int valor =  buscarEnTablaPaginasINSERT(segmento->tablaPaginas, key, timeStamp, value );// aca tenemos que buscar en la tabla de paginas especifica de este segmento y meternos 1 x 1 en sus paginas para ver si en la memoria Principal esta el key
		if(valor!= 0){ //lo encontro en tabla de paginas
			//tengo que verificar los timestamps entre ambos a ver cual se queda en memoria principal
			log_info(loggerMemoria,"Esta en la tabla de PAGINAS");
			log_info(loggerMemoria,"Se ha actualizado el value de la tabla");
			pthread_mutex_unlock(&semTablaSegmentos);
			return "INFO: Se ha actualizado el value de la tabla";
		}
		else{ //no lo encontro en tabla de paginas
			log_info(loggerMemoria,"No esta en la tabla de PAGINAS");
			int indice = guardarEnMemoria(nombreTabla, key, value, timeStamp);
			if(indice == -1){
				pthread_mutex_unlock(&semTablaSegmentos);
				log_info(loggerMemoria,"La memoria esta FULL");
				return "FULL";
			}
			int t = tamanioLista(tablaDeSegmentos);
			ubicacionSegmento = buscarTablaSegmentos(nombreTabla);
			if(ubicacionSegmento==-1 || t==0){
				segmento = guardarEnTablaDeSegmentos(nombreTabla);
			}
			guardarEnTablaDePaginas(segmento, indice, key, 1);

			reacomodarNumerosDePaginas();

			log_info(loggerMemoria,"Se guardo en la tabla de PAGINAS y en la MEMORIA");
			mostrarElementosTablaSegmentos();
			mostrarDatosMarcos();
			log_info(loggerMemoria,"Se guardo correctamente");
			pthread_mutex_unlock(&semTablaSegmentos);
			return "INFO: Se guardo correctamente";
		}
	}
	else{ // no esta en tabla de SEGMENTOS
		log_info(loggerMemoria,"No se encontro en la tabla de SEGMENTOS");
		int indice = guardarEnMemoria(nombreTabla, key, value, timeStamp);
		if(indice == -1){
			pthread_mutex_unlock(&semTablaSegmentos);
			log_info(loggerMemoria,"FULL");
			return "FULL";
		}
		t_segmento* segmento = guardarEnTablaDeSegmentos(nombreTabla);
		guardarEnTablaDePaginas(segmento, indice, key, 1);

		reacomodarNumerosDePaginas();

		pthread_mutex_unlock(&semTablaSegmentos);
		log_info(loggerMemoria, "Se guardo en MP, en tabla de PAGINAS y en tabla de SEGMENTOS");
		log_info(loggerMemoria,"Se guardo correctamente");
		return "INFO: Se guardo correctamente";
	}

}

//-----------------------------------------------------DROP---------------------------------------------//

char* DROPMemoria(char* nombreTabla){
	pthread_mutex_lock(&semTablaSegmentos);
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
		if(ubicacionSegmento!=-1){
			log_info(loggerMemoria,"Esta en la tabla de SEGMENTOS");
			void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
			t_segmento *segmento =(t_segmento*)elemento;
			quitarEspaciosGuardadosEnMemoria(segmento->tablaPaginas);
			borrarTablaDePaginas(segmento->tablaPaginas);
			list_remove(tablaDeSegmentos, ubicacionSegmento);
			pthread_mutex_unlock(&semTablaSegmentos);
			reacomodarNumerosDePaginas();
			free(segmento->nombreTabla);
			free(segmento);
		}
		else{
			pthread_mutex_unlock(&semTablaSegmentos);
			log_info(loggerMemoria,"Dicha tabla no se encuentra en la tabla de SEGMENTOS");
		}
		pthread_mutex_lock(&semLfs);
		if (socketLFS ==0){
			pthread_mutex_unlock(&semLfs);
			return "LFS_CAIDO";
		}
		char* value = opDROP(socketLFS,nombreTabla);
		if (string_equals_ignore_case(value, "MEMORIA_DESCONECTADA")){
			socketLFS= 0;
			pthread_mutex_unlock(&semLfs);
			free(value);
			return "LFS_CAIDO";
		}
		retardoLFSAplicado();
		pthread_mutex_unlock(&semLfs);
		return  value;

}


void DROPMemoriaExclusivoLRU(char* nombreTabla){
	pthread_mutex_lock(&semTablaSegmentos);
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
		if(ubicacionSegmento!=-1){
			log_info(loggerMemoria,"Esta en la tabla de SEGMENTOS");
			void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
			t_segmento *segmento =(t_segmento*)elemento;
			list_destroy(segmento->tablaPaginas);
			pthread_mutex_unlock(&semTablaSegmentos);
			list_remove(tablaDeSegmentos, ubicacionSegmento);
			reacomodarNumerosDePaginas();
			free(segmento->nombreTabla);
			free(segmento);
		}
		else{
			pthread_mutex_unlock(&semTablaSegmentos);
			log_info(loggerMemoria,"Dicha tabla no se encuentra en la tabla de SEGMENTOS");
		}
}

//------------------------------------------------JOURNAL------------------------------------------------//
void JOURNALMemoria(){
	pthread_mutex_lock(&semLfs);
	pthread_mutex_lock(&semTablaSegmentos);
	int tamanioSegmento = list_size(tablaDeSegmentos);
	if(tamanioSegmento!=0){
		iniciarJournal();
		pthread_mutex_unlock(&semTablaSegmentos);
		retardoLFSAplicado();
		pthread_mutex_unlock(&semLfs);
	}else {
		log_info(loggerMemoria, "No hay datos para poder hacer JOURNAL");
		pthread_mutex_unlock(&semTablaSegmentos);
		pthread_mutex_unlock(&semLfs);
	}


}
char* DESCRIBETodasLasTablasMemoria(){
	pthread_mutex_lock(&semLfs);
	if (socketLFS ==0){
		pthread_mutex_unlock(&semLfs);
		return "LFS_CAIDO";
	}
	char* value = opDESCRIBE(socketLFS,"ALL_TABLES");
	if (string_equals_ignore_case(value, "MEMORIA_DESCONECTADA")){
		socketLFS= 0;
		pthread_mutex_unlock(&semLfs);
		free(value);
		return "LFS_CAIDO";
	}
	retardoLFSAplicado();
	pthread_mutex_unlock(&semLfs);

	return value;
}
char* DESCRIBEMemoria( char* nombreTabla){
	pthread_mutex_lock(&semLfs);
	if (socketLFS ==0){
		pthread_mutex_unlock(&semLfs);
		return "LFS_CAIDO";
	}
	char* value = opDESCRIBE(socketLFS,nombreTabla);
	if (string_equals_ignore_case(value, "MEMORIA_DESCONECTADA")){
		socketLFS= 0;
		pthread_mutex_unlock(&semLfs);
		free(value);
		return "LFS_CAIDO";
	}
	retardoLFSAplicado();
	pthread_mutex_unlock(&semLfs);
	return value;
}

char* CREATEMemoria(char* nombreTabla, char* tipoConsistencia, int nroParticiones, int compactionTime){
	pthread_mutex_lock(&semLfs);
	if (socketLFS ==0){
		pthread_mutex_unlock(&semLfs);
		return "LFS_CAIDO";
	}
	char* value = opCREATE(socketLFS,nombreTabla, tipoConsistencia, nroParticiones, compactionTime);
	if (string_equals_ignore_case(value, "MEMORIA_DESCONECTADA")){
		socketLFS= 0;
		pthread_mutex_unlock(&semLfs);
		free(value);
		return "LFS_CAIDO";
	}
	retardoLFSAplicado();
	pthread_mutex_unlock(&semLfs);
	 return value;
}
