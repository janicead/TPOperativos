#include "segmentacionPaginada.h"

//----------------------------------------GENERALES-------------------------------------------//

void definirTamanioMemoriaPrincipal( int tamanioValueDadoXLFS){
	tamanioMaxMemoria = 52; //de archivo de config //antes 52
	tamanioDadoPorLFS= tamanioValueDadoXLFS;
	tamanioUnRegistro = tamanioValueDadoXLFS + sizeof(unsigned long int) + sizeof(uint16_t); //6+ tamanioValueDado
	obtenerValue = sizeof(unsigned long int) + sizeof(uint16_t);
	memoriaPrincipal = malloc(tamanioMaxMemoria);
	log_info(loggerMemoria,"El tamanio de un resgistro es de %d\n", tamanioUnRegistro);
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
	pthread_mutex_init(&semMemoriaPrincipal,NULL);
}
int tamanioLista(t_list * lista){
	return list_size(lista);
}

char* recibirRespuestaSELECTMemoriaLfs(){
	return "Hola Soy Lissandra";
}

void mostrarDatosMarcos(){
	for(int i = 0; i<cantMaxMarcos; i ++){
		printf("Marco %d : %d\n" ,i, marcosOcupados[i]);
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
	pthread_mutex_lock(&semTablaSegmentos);
	int cantSegmentos =  tamanioLista(tablaDeSegmentos);
	for(int i = 0; i<cantSegmentos ; i++){
		void * elemento = list_get(tablaDeSegmentos, i);
		t_segmento *segmento =(t_segmento*)elemento;
		if(string_equals_ignore_case(nombreTabla, segmento->nombreTabla)){
			pthread_mutex_unlock(&semTablaSegmentos);
			return i;
		}
	}
	pthread_mutex_unlock(&semTablaSegmentos);
	return cantSegmentos+1;
}

t_segmento* guardarEnTablaDeSegmentos(char* nombreTabla){
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->nombreTabla = strdup(nombreTabla);
	segmento->tablaPaginas= list_create();
	pthread_mutex_lock(&semTablaSegmentos);
	list_add(tablaDeSegmentos, (void*)segmento);
	pthread_mutex_unlock(&semTablaSegmentos);
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
		return registro->value;
		}
	}
	return NULL; // SI retorna null significa que no esta en tabla de paginas y hay que consultarle a FS y agregarlo
}

void guardarEnTablaDePaginas(t_segmento * segmento, int nroMarco,uint16_t key, int flagModificado ){
	int tamanioTablaPaginas = tamanioLista(segmento->tablaPaginas);
	t_pagina * pagina = malloc(sizeof(t_pagina));
	pagina->contadorVecesSolicitado=0;
	pagina->flagModificado= flagModificado;
	pagina->key = key;
	pagina->numeroMarco= nroMarco;
	pagina->numeroPag= tamanioTablaPaginas;
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
		log_info(loggerMemoria,"NUMERO DE PAG %d, SU KEY ES %d y su INDICE EN MEMORIA ES %d\n", pagina->numeroPag, pagina->key, pagina->numeroMarco);
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
	t_registro * registro = malloc(sizeof(t_registro));
	int copiarDesde = 0;
	registro->value = malloc(tamanioDadoPorLFS);
	pthread_mutex_lock(&semMemoriaPrincipal);
	memcpy(&registro->key, memoriaPrincipal + tamanioUnRegistro * nroMarco + copiarDesde,sizeof(uint16_t));
	copiarDesde += sizeof(uint16_t);
	memcpy(&registro->timestamp, memoriaPrincipal+tamanioUnRegistro * nroMarco+ copiarDesde, sizeof(unsigned long int));
	copiarDesde += sizeof(unsigned long int);
	memcpy(registro->value, memoriaPrincipal + tamanioUnRegistro * nroMarco+ copiarDesde,tamanioDadoPorLFS);
	pthread_mutex_unlock(&semMemoriaPrincipal);
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
	t_registro * registro = malloc(sizeof(t_registro));
	registro->value = value;
	registro->timestamp= timeStamp;
	pthread_mutex_lock(&semMemoriaPrincipal);
	memcpy(memoriaPrincipal+ nroMarco*tamanioUnRegistro+sizeof(uint16_t), &registro->timestamp, sizeof(unsigned long int));
	memcpy(memoriaPrincipal+nroMarco*tamanioUnRegistro+sizeof(uint16_t)+ sizeof(unsigned long int), registro->value, tamanioDadoPorLFS);
	pthread_mutex_unlock(&semMemoriaPrincipal);
	free(registro);
}

void guardarEnMPLugarEspecifico(uint16_t key, char* value, int nroMarco, unsigned long int timestamp){
	t_registro * registro = malloc(sizeof(t_registro));
	registro->key = key;
	registro->value= value;
	registro->timestamp= timestamp;
	pthread_mutex_lock(&semMemoriaPrincipal);
	memcpy(memoriaPrincipal+ nroMarco*tamanioUnRegistro, &registro->key, sizeof(uint16_t));
	memcpy(memoriaPrincipal+ nroMarco*tamanioUnRegistro+sizeof(uint16_t), &registro->timestamp, sizeof(unsigned long int));
	memcpy(memoriaPrincipal+nroMarco*tamanioUnRegistro+sizeof(uint16_t)+ sizeof(unsigned long int), registro->value, tamanioDadoPorLFS);
	pthread_mutex_unlock(&semMemoriaPrincipal);
	pthread_mutex_lock(&semCantMarcosIngresados);
	cantMarcosIngresados++;
	pthread_mutex_unlock(&semCantMarcosIngresados);
	settearMarcoEnMP(nroMarco, 1);
	free(registro);
}

int guardarEnMemoria(char* nombreTabla, uint16_t key, char* value, unsigned long int timestamp){
	int nroMarco = buscarEspacioLibreEnMP();
	puts("aca entre 1\n");
	pthread_mutex_lock(&semCantMaxMarcos);
	puts("aca entre 1\n");
	if(nroMarco!=cantMaxMarcos){
		puts("aca entre 1\n");
		guardarEnMPLugarEspecifico(key, value, nroMarco, timestamp);
		puts("aca entre 2 \n");
		pthread_mutex_unlock(&semCantMaxMarcos);
			return nroMarco;
	}
	else{
		t_LRU * lru = LRU();
		if(lru->numeroPag != cantMaxMarcos){

			void * elemento = list_remove(lru->tablaPaginas, lru->numeroPag);
			t_pagina *pagina =(t_pagina*)elemento;
			int t = tamanioLista(lru->tablaPaginas);
			if(t==0){
				DROPMemoria(lru->nombreTabla);
			}
			log_info(loggerMemoria,"La PAGINA que voy a reemplazar es la nro %d del SEGMENTO '%s' \n", lru->numeroPag, lru->nombreTabla);
			settearMarcoEnMP(pagina->numeroMarco, 0);
			guardarEnMPLugarEspecifico(key, value, pagina->numeroMarco, timestamp);
			free(lru);
			int nroMarco = pagina->numeroMarco;
			free(pagina);
			pthread_mutex_unlock(&semCantMaxMarcos);
			return nroMarco;
		}
		else {
			log_info(loggerMemoria,"Tengo que realizar JOURNAL\n");
			iniciarJournal();// se inicia journal, osea que queda vacia la memoria entonces tiene si o si espacio
			//luego de realizar journal, se terminaria guardando el dato :D
			nroMarco = buscarEspacioLibreEnMP();
			guardarEnMPLugarEspecifico(key, value, nroMarco, timestamp);
			free(lru);
			pthread_mutex_unlock(&semCantMaxMarcos);
			return 0;
		}
	}
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
	pthread_mutex_lock(&semMemoriaPrincipal);
	memset(memoriaPrincipal, 0, tamanioMaxMemoria);
	pthread_mutex_unlock(&semMemoriaPrincipal);
}
//---------------------------------------LRU-------------------------------------------------------//

t_LRU * LRU (){
	int cantVecesSolicitadaMinimo = 0;
	//int paginaMenosCantVecesSolicitada = cantMaxMarcos;
	int esElPrimerElemento = 0;
	t_LRU * lru = malloc (sizeof(t_LRU));
	lru->numeroPag= cantMaxMarcos;
	pthread_mutex_unlock(&semCantMaxMarcos);
	int tamanioTablaPaginas = 0;
	pthread_mutex_lock(&semTablaSegmentos);
	int tamanioTablaSegmentos = tamanioLista(tablaDeSegmentos);
	for(int i = 0; i< tamanioTablaSegmentos; i++){

		void * elemento = list_get(tablaDeSegmentos, i);
		t_segmento *segmento =(t_segmento*)elemento;
		tamanioTablaPaginas = tamanioLista(segmento->tablaPaginas);
		lru->posicionSegmento= i;
		for(int j = 0; j<tamanioTablaPaginas; j++){

			void * elemento = list_get(segmento->tablaPaginas, j);
			t_pagina *pagina =(t_pagina*)elemento;

			if(esElPrimerElemento == 0 && pagina->flagModificado == 0){

				cantVecesSolicitadaMinimo = pagina->contadorVecesSolicitado;
				//paginaMenosCantVecesSolicitada = pagina->numeroPag;
				esElPrimerElemento = 1;
				lru->numeroPag= pagina->numeroPag;
				lru->tablaPaginas= segmento->tablaPaginas;
				lru->nombreTabla = segmento->nombreTabla;

			} else if(pagina->flagModificado==0 && (pagina->contadorVecesSolicitado)<cantVecesSolicitadaMinimo){

				cantVecesSolicitadaMinimo = pagina->contadorVecesSolicitado;
				//paginaMenosCantVecesSolicitada = pagina->numeroPag;
				lru->numeroPag= pagina->numeroPag;
				lru->tablaPaginas= segmento->tablaPaginas;
				lru->nombreTabla= segmento->nombreTabla;

			}
		}
	}
	pthread_mutex_unlock(&semTablaSegmentos);
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
	pthread_mutex_lock(&semTablaSegmentos);
	int tamanioTablaSegmentos = tamanioLista(tablaDeSegmentos);
	char* elementoEnviar = string_new();
	for(int i = 0 ; i < tamanioTablaSegmentos; i ++){
		void * elemento = list_get(tablaDeSegmentos, i);
		t_segmento *segmento =(t_segmento*)elemento;
		int tamanioTablaPaginas = tamanioLista(segmento->tablaPaginas);
		for(int j = 0; j<tamanioTablaPaginas; j++){
			void * elemento = list_get(segmento->tablaPaginas, j);
			t_pagina * pagina = (t_pagina *) elemento;
			if(pagina->flagModificado ==1){
			t_registro* registro = buscarEnMemoriaPrincipal(pagina->numeroMarco);
			char* unRegistro= string_from_format("%s %d %s %lu/",segmento->nombreTabla,registro->key,registro->value,registro->timestamp);
			string_append(&elementoEnviar, unRegistro);
			free(unRegistro);
			free(registro->value);
			free(registro);
			}
		}
	}
	puts("-------------------------------------------------------------------");
	log_info(loggerMemoria, "JOURNAL: lista a enviar es---> %s\n", elementoEnviar);
	puts("-------------------------------------------------------------------");

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
	pthread_mutex_unlock(&semTablaSegmentos);
	borrarTodaMemoria();
	pthread_mutex_lock(&semCantMarcosIngresados);
	cantMarcosIngresados=0;
	pthread_mutex_unlock(&semCantMarcosIngresados);
	//empaquetarEnviarMensaje(socketLFS,22,strlen(msjEnviado),msjEnviado);
	free(elementoEnviar);
}

//----------------------------------------------------REQUESTS-------------------------------------------------//


char* SELECTMemoria(char * nombreTabla, uint16_t key, int flagModificado){
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
	pthread_mutex_lock(&semTablaSegmentos);
	int cantSegmentos = tamanioLista(tablaDeSegmentos);
	pthread_mutex_unlock(&semTablaSegmentos);
	if(ubicacionSegmento!=(cantSegmentos+1)){ //esta en tabla de segmentos
		log_info(loggerMemoria,"Esta en la tabla de SEGMENTOS");
		pthread_mutex_lock(&semTablaSegmentos);
		void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
		t_segmento *segmento =(t_segmento*)elemento;
		char* value = buscarTablaPaginas(segmento->tablaPaginas, key);// aca tenemos que buscar en la tabla de paginas especifica de este segmento y meternos 1 x 1 en sus paginas para ver si en la memoria Principal esta el key
		pthread_mutex_unlock(&semTablaSegmentos);
		if(value!= NULL){ //lo encontro en tabla de paginas, lo busca en memoria principal y devuelve lo que vale
			log_info(loggerMemoria,"Esta en la tabla de PAGINAS");
			mostrarDatosMarcos();
			mostrarElementosTablaSegmentos();
			mostrarElementosMemoriaPrincipal();
			return value;
		}
		else{ //no lo encontro en tabla de paginas
			//tengo que consultarle a LFS PERO solo guardo en tabla de paginas
			//consultaSELECTMemoriaLfs();// esto va a mandarle SELECT nombreTabla key con SOCKETS
			log_info(loggerMemoria,"No esta en la tabla de PAGINAS");
			char* value = recibirRespuestaSELECTMemoriaLfs(); //con SOCKETS
			unsigned long int t = obtenerTimeStamp();
			int nroMarco = guardarEnMemoria(nombreTabla, key, value, t);
			pthread_mutex_lock(&semTablaSegmentos);
			guardarEnTablaDePaginas(segmento, nroMarco, key, flagModificado);
			pthread_mutex_unlock(&semTablaSegmentos);
			mostrarDatosMarcos();
			mostrarElementosTablaSegmentos();
			mostrarElementosMemoriaPrincipal();
			return value;
		}
	}
	else{// no esta en tabla de segmentos
		//pedirle a lfs y guardar datos en tabla segmentos y tabla paginas
		//consultaSELECTMemoriaLfs();
		log_info(loggerMemoria,"No se encontro en la tabla de SEGMENTOS");
		char* value = recibirRespuestaSELECTMemoriaLfs();
		t_segmento* segmento = guardarEnTablaDeSegmentos(nombreTabla);
		segmento->tablaPaginas= list_create();
		unsigned long int t = obtenerTimeStamp();
		int nroMarco = guardarEnMemoria(nombreTabla, key, value, t);
		pthread_mutex_lock(&semTablaSegmentos);
		guardarEnTablaDePaginas(segmento, nroMarco, key, flagModificado);
		pthread_mutex_unlock(&semTablaSegmentos);
		log_info(loggerMemoria,"Se guardo en MP, en tabla de PAGINAS y en tabla de SEGMENTOS");
		mostrarDatosMarcos();
		mostrarElementosTablaSegmentos();
		mostrarElementosMemoriaPrincipal();
		return value;
	}
}

char* INSERTMemoria(char * nombreTabla, uint16_t key, char* value, unsigned long int timeStamp){
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
	pthread_mutex_lock(&semTablaSegmentos);
	int cantSegmentos = tamanioLista(tablaDeSegmentos);
	pthread_mutex_unlock(&semTablaSegmentos);
	if(ubicacionSegmento!=(cantSegmentos+1)){ //esta en tabla de SEGMENTOS
		log_info(loggerMemoria,"Esta en la tabla de SEGMENTOS");
		pthread_mutex_lock(&semTablaSegmentos);
		void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
		t_segmento *segmento =(t_segmento*)elemento;
		int valor =  buscarEnTablaPaginasINSERT(segmento->tablaPaginas, key, timeStamp, value );// aca tenemos que buscar en la tabla de paginas especifica de este segmento y meternos 1 x 1 en sus paginas para ver si en la memoria Principal esta el key
		pthread_mutex_unlock(&semTablaSegmentos);
		if(valor!= 0){ //lo encontro en tabla de paginas
			//tengo que verificar los timestamps entre ambos a ver cual se queda en memoria principal
			log_info(loggerMemoria,"Esta en la tabla de PAGINAS");
			return "INFO: Se ha actualizado el value de la tabla";
		}
		else{ //no lo encontro en tabla de paginas
			log_info(loggerMemoria,"No esta en la tabla de PAGINAS");
			int indice = guardarEnMemoria(nombreTabla, key, value, timeStamp);
			pthread_mutex_lock(&semTablaSegmentos);
			int t = tamanioLista(tablaDeSegmentos);
			pthread_mutex_unlock(&semTablaSegmentos);
			if(t==0){
				segmento = guardarEnTablaDeSegmentos(nombreTabla);
			}
			pthread_mutex_lock(&semTablaSegmentos);
			guardarEnTablaDePaginas(segmento, indice, key, 1);
			pthread_mutex_unlock(&semTablaSegmentos);
			log_info(loggerMemoria,"Se guardo en la tabla de PAGINAS y en la MEMORIA");
			return "INFO: Se guardo correctamente";
		}
	}
	else{ // no esta en tabla de SEGMENTOS
		log_info(loggerMemoria,"No se encontro en la tabla de SEGMENTOS");
		int indice = guardarEnMemoria(nombreTabla, key, value, timeStamp);
		t_segmento* segmento = guardarEnTablaDeSegmentos(nombreTabla);
		segmento->tablaPaginas= list_create();
		pthread_mutex_lock(&semTablaSegmentos);
		guardarEnTablaDePaginas(segmento, indice, key, 1);
		pthread_mutex_unlock(&semTablaSegmentos);
		log_info(loggerMemoria, "Se guardo en MP, en tabla de PAGINAS y en tabla de SEGMENTOS");
		return "INFO: Se guardo correctamente";
	}

}

char* DROPMemoria(char* nombreTabla){
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
	pthread_mutex_lock(&semTablaSegmentos);
	int cantSegmentos = tamanioLista(tablaDeSegmentos);
	pthread_mutex_unlock(&semTablaSegmentos);
		if(ubicacionSegmento!=(cantSegmentos+1)){
			log_info(loggerMemoria,"Esta en la tabla de SEGMENTOS");
			pthread_mutex_lock(&semTablaSegmentos);
			void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
			t_segmento *segmento =(t_segmento*)elemento;
			quitarEspaciosGuardadosEnMemoria(segmento->tablaPaginas);
			borrarTablaDePaginas(segmento->tablaPaginas);
			pthread_mutex_unlock(&semTablaSegmentos);
			list_remove(tablaDeSegmentos, ubicacionSegmento);
			free(segmento->nombreTabla);
			free(segmento);
		}
		else{
			log_info(loggerMemoria,"Dicha tabla no se encuentra en la tabla de SEGMENTOS");
		}
		int existe = 1; // esto es para q quede lindo despues se modifica de acuerdo a la rsta del lfs
		//aca tengo que avisarle al FS que borre la tabla
		if(existe == 1){
			return  "INFO: Se borro correctamente la tabla";
		}
		else{ // si la encuentra, la borra y sino tiro mensaje de error de que no existe
			return "ERROR: Dicha tabla no se encuentra en la tabla de SEGMENTOS";
		}

}
void JOURNALMemoria(){
	iniciarJournal();
}
char* DESCRIBETodasLasTablasMemoria(){
	//aca le mando a lfs para q me de metadata de todas las tablas
	return "DESCRIBE TOTAL OK";
}
char* DESCRIBEMemoria( char* nombreTabla){
	 //aca tengo q mandarle mensaje a LFS para q me de la metada de esa tabla
	return "DESCRIBE OK";
}

char* CREATEMemoria(char* nombreTabla, char* tipoConsistencia, int nroParticiones, int compactionTime){
	 return "CREATE OK";
}

