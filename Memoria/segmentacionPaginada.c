#include "segmentacionPaginada.h"

void destructor2(t_pagina * pagina){
	free(pagina);
}
void destructor(t_segmento* segmento){
	list_destroy_and_destroy_elements(segmento->tablaPaginas, (void*)destructor2);
	free(segmento);
}
void borrarElementos(){
		list_destroy_and_destroy_elements(tablaDeSegmentos, (void*)destructor);

}
void mostrarElementosMemoriaPrincipal(t_registro *memoriaPrincipal){
	for(int i = 0; i<cantMaxPags; i ++){
		printf("En la posicion %d de la MEMORIA PRINCIPAL, la KEY es %d, el VALUE es %s\n", i, memoriaPrincipal[i].key,memoriaPrincipal[i].value);
	}
}

void mostrarElementosTablaSegmentos(){
	int tamanioTS= tamanioLista(tablaDeSegmentos);
	if(tamanioTS==0){
		puts("INFO: No hay SEGMENTOS en la TABLA DE SEGMENTOS");
	}
	for(int i = 0; i < tamanioTS; i++){
		void* elemento = list_get(tablaDeSegmentos, i);
		t_segmento* segmento = (t_segmento*) elemento;
		printf("NOMBRE DE TABLA: '%s'\n",segmento->nombreTabla);
		mostrarElementosTablaPaginas(segmento->tablaPaginas);
	}
}

void mostrarElementosTablaPaginas(t_list * lista){
	int tamanioTP= tamanioLista(lista);
	if(tamanioTP==0){
		puts("INFO: No hay PAGINAS en la TABLA DE PAGINAS");
	}
	for(int i = 0 ; i<tamanioTP; i++){
		void* elemento = list_get(lista, i);
		t_pagina* pagina = (t_pagina*) elemento;
		printf("NUMERO DE PAG %d, SU KEY ES %d y su INDICE EN MEMORIA ES %d\n", pagina->numeroPag, pagina->key, pagina->indiceMemoria);
	}
}
void mostrarElementosListaJournal(){
	int tamanioTP= tamanioLista(listaJournal);
	printf("CANT ELEMENTOS JOURNAL %d\n", tamanioTP);
	for(int i = 0 ; i<tamanioTP; i++){
		void* elemento = list_get(listaJournal, i);
		t_JOURNAL* journal = (t_JOURNAL*) elemento;
		printf("NOMBRE TABLA es '%s', su KEY es %d, su TIMESTAMP %d y su VALUE es %s\n", journal->nombreTabla, journal->registro->key, journal->registro->timestamp, journal->registro->value);
	}
}
char* convertirAStringListaJournal(){

	int tamanioJOURNAL= tamanioLista(listaJournal);
	char * elementoEnviar = string_new();
	for (int i = 0; i<tamanioJOURNAL; i ++){
		void* elemento = list_get(listaJournal, i);
		t_JOURNAL* journal = (t_JOURNAL*) elemento;

		string_append(&elementoEnviar, journal->nombreTabla);
		char* numeroKey= malloc (1500);
		char* numeroTimestamp= malloc (1500);
		string_append(&elementoEnviar, " ");
		sprintf(numeroKey, "%d", journal->registro->key);
		string_append(&elementoEnviar, numeroKey);
		string_append(&elementoEnviar, " ");
		string_append(&elementoEnviar, journal->registro->value);
		string_append(&elementoEnviar, " ");
		sprintf(numeroTimestamp, "%d", journal->registro->timestamp);
		string_append(&elementoEnviar, numeroTimestamp);
		string_append(&elementoEnviar, "/");

	}
	printf("[INFO] LA LISTA ES----> %s \n", elementoEnviar);
	return elementoEnviar;
}

void definirTamanioMemoriaPrincipal( int tamanioValueDadoXLFS){
	int tamanioMaxMemoria = 14; //de archivo de config
	int tamanioUnRegistro = tamanioValueDadoXLFS + sizeof(int) + sizeof(uint16_t); //6+ tamanioValueDado
	cantMaxPags = tamanioMaxMemoria/ tamanioUnRegistro;
	memoriaPrincipal = (t_registro*)calloc ( cantMaxPags, sizeof(t_registro)); //aca van a estar todas los registros
	//mp= (t_registro*)calloc ( cantMaxPags, sizeof(t_registro));
	tamanioMemoriaPrincipal= tamanioArray(memoriaPrincipal);
}

//retorna 0 si encuentra el elemento en tablaSegmentos
int buscarTablaSegmentos(char* nombreTabla){
	int cantSegmentos =  tamanioLista(tablaDeSegmentos);
	for(int i = 0; i<cantSegmentos ; i++){
		void * elemento = list_get(tablaDeSegmentos, i);
		t_segmento *segmento =(t_segmento*)elemento;
		if(string_equals_ignore_case(nombreTabla, segmento->nombreTabla)){
			return i;
		}
	}
	return cantSegmentos+1;
}

char* buscarTablaPaginas(t_list* tabla, uint16_t key, t_registro* memoriaPrincipal){

	int cantPaginas = tamanioLista(tabla);
	for(int i = 0; i<cantPaginas; i++){
		void * elemento = list_get(tabla, i);
		t_pagina *pagina =(t_pagina*)elemento;
		if(pagina->key ==key){
		char* encontroEnMP = buscarEnMemoriaPrincipal(pagina->indiceMemoria, memoriaPrincipal);
		pagina->contadorVecesSolicitado++;
		printf("Cantidad veces solicitado %d\n", pagina->contadorVecesSolicitado);
		return encontroEnMP;
		}
	}
	return NULL; // SI retorna null significa que no esta en tabla de paginas y hay que consultarle a FS y agregarlo
}

int buscarEnTablaPaginasINSERT(t_list* tabla, uint16_t key,int timeStamp, t_registro* memoriaPrincipal , char* value){

	int cantPaginas = tamanioLista(tabla);
	for(int i = 0; i<cantPaginas; i++){
		void * elemento = list_get(tabla, i);
		t_pagina *pagina =(t_pagina*)elemento;
		if(pagina->key ==key){
			int TS = buscarEnMemoriaPrincipalTimeStamp(pagina->indiceMemoria, memoriaPrincipal);
			if(timeStamp>=TS){ // asi lo modifica si se llega a dar el caso q se hace en el mismo momento
				actualizarMemoriaPrincipal(pagina->indiceMemoria, memoriaPrincipal, timeStamp, value);
				pagina->flagModificado=1;
				puts("INFO: Se actualizo la MEMORIA PRINCIPAL");
			}
			return timeStamp;
		}
	}
	return 0;
}


char* buscarEnMemoriaPrincipal( int indice,t_registro* memoriaPrincipal){
		 return memoriaPrincipal[indice].value;
}

int buscarEnMemoriaPrincipalTimeStamp( int indice,t_registro* memoriaPrincipal){
	return memoriaPrincipal[indice].timestamp;
}

uint16_t buscarEnMemoriaPrincipalKey(int indice, t_registro* memoriaPrincipal){
	return memoriaPrincipal[indice].key;
}
void actualizarMemoriaPrincipal( int indice,t_registro* memoriaPrincipal, int timeStamp, char* value){
	memoriaPrincipal[indice].timestamp = timeStamp;
	memoriaPrincipal[indice].value = strdup(value);
}

int buscarEspacioEnMP(t_registro* memoriaPrincipal){
	for(int i = 0; i< cantMaxPags; i ++){
		if(memoriaPrincipal[i].key==NULL){
			return i;
		}
	}
	return cantMaxPags;
}

t_segmento* guardarEnTablaDeSegmentos(char* nombreTabla){
	int cantPaginasGuardadas;
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->nombreTabla= nombreTabla;
	segmento->tablaPaginas= list_create();
	list_add(tablaDeSegmentos, (void*)segmento);
	return segmento;
}

void guardarEnTablaDePaginas(t_segmento * segmento, int indice,uint16_t key, int flagModificado ){
	int tamanioTablaPaginas = tamanioLista(segmento->tablaPaginas);
	t_pagina * pagina = malloc(sizeof(t_pagina));
	pagina->contadorVecesSolicitado=0;
	pagina->flagModificado= flagModificado;
	pagina->key = key;
	pagina->indiceMemoria= indice;
	pagina->numeroPag= tamanioTablaPaginas;
	list_add(segmento->tablaPaginas,(void*)pagina);
}


t_LRU * LRU (){
	int cantVecesSolicitadaMinimo = 0;
	int paginaMenosCantVecesSolicitada = cantMaxPags;
	int esElPrimerElemento = 0;
	t_LRU * lru = malloc (sizeof(t_LRU));
	lru->numeroPag= cantMaxPags;

	int tamanioTablaSegmentos = tamanioLista(tablaDeSegmentos);
	for(int i = 0; i< tamanioTablaSegmentos; i++){
		void * elemento = list_get(tablaDeSegmentos, i);
		t_segmento *segmento =(t_segmento*)elemento;
		int tamanioTablaPaginas = tamanioLista(segmento->tablaPaginas);
		for(int j = 0; j<tamanioTablaPaginas; j++){
			void * elemento = list_get(segmento->tablaPaginas, j);
			t_pagina *pagina =(t_pagina*)elemento;
			if(esElPrimerElemento == 0 && pagina->flagModificado == 0){
				cantVecesSolicitadaMinimo = pagina->contadorVecesSolicitado;
				paginaMenosCantVecesSolicitada = pagina->numeroPag;
				esElPrimerElemento = 1;
				lru->numeroPag= pagina->numeroPag;
				lru->tablaPaginas= segmento->tablaPaginas;
				lru->nombreTabla = segmento->nombreTabla;
			} else if(pagina->flagModificado==0 && (pagina->contadorVecesSolicitado)<cantVecesSolicitadaMinimo){
				cantVecesSolicitadaMinimo = pagina->contadorVecesSolicitado;
				paginaMenosCantVecesSolicitada = pagina->numeroPag;
				lru->numeroPag= pagina->numeroPag;
				lru->tablaPaginas= segmento->tablaPaginas;
				lru->nombreTabla= segmento->nombreTabla;
			}
		}
	}
	return lru;
}

int guardarEnMemoria(char* nombreTabla, uint16_t key, char* value, t_registro* memoriaPrincipal){
	int espacio = buscarEspacioEnMP(memoriaPrincipal);
	if(espacio !=cantMaxPags){
		memoriaPrincipal[espacio].key = key;
		memoriaPrincipal[espacio].timestamp= obtenerTimeStamp();
		memoriaPrincipal[espacio].value= strdup(value);
		return espacio; //tengo que guardar en tabla de pagina
	}
	else{
		t_LRU * lru = LRU();
		if(lru->numeroPag != cantMaxPags){
			void * elemento = list_remove(lru->tablaPaginas, lru->numeroPag);
			t_pagina *pagina =(t_pagina*)elemento;
			printf("INFO: La PAGINA que voy a reemplazar es la nro %d del SEGMENTO '%s' \n", lru->numeroPag, lru->nombreTabla);
			memoriaPrincipal[pagina->indiceMemoria].key = key;
			memoriaPrincipal[pagina->indiceMemoria].timestamp= obtenerTimeStamp();
			memoriaPrincipal[pagina->indiceMemoria].value= strdup(value);
			free(lru);
			return pagina->indiceMemoria;
		}
		else {
			puts("INFO: Tengo que realizar JOURNAL\n");
			iniciarJournal(memoriaPrincipal);// se inicia journal, osea que queda vacia la memoria entonces tiene si o si espacio
			//luego de realizar journal, se terminaria guardando el dato :D
			int espacio = buscarEspacioEnMP(memoriaPrincipal);
			memoriaPrincipal[espacio].key = key;
			memoriaPrincipal[espacio].timestamp= obtenerTimeStamp();
			memoriaPrincipal[espacio].value= strdup(value);
			free(lru);
			return espacio;
		}

	}
}
void borrarTodo(t_registro* memoriaPrincipal){
	for(int i = 0; i< cantMaxPags; i++ ){
		memoriaPrincipal[i].key=NULL;
		memoriaPrincipal[i].timestamp= NULL;
		memoriaPrincipal[i].value= NULL;
	}
	borrarElementos();
}


void iniciarJournal(t_registro* memoriaPrincipal){
	//generar lista
	listaJournal= list_create();
	int tamanioTablaSegmentos = tamanioLista(tablaDeSegmentos);
	printf("Tamanio tabla de segmentos %d\n", tamanioTablaSegmentos);
	for(int i = 0 ; i < tamanioTablaSegmentos; i ++){
		void * elemento = list_get(tablaDeSegmentos, i);
		t_segmento *segmento =(t_segmento*)elemento;
		int tamanioTablaPaginas = tamanioLista(segmento->tablaPaginas);
		printf("Tamanio tabla de paginas %d para el segmento nro %d\n", tamanioTablaPaginas, i);
		for(int j = 0; j<tamanioTablaPaginas; j++){
			void * elemento = list_get(segmento->tablaPaginas, j);
			t_pagina * pagina = (t_pagina *) elemento;
			if(pagina->flagModificado ==1){
			t_JOURNAL* journal = malloc(sizeof(t_JOURNAL));
			t_registro * registro = malloc(sizeof(t_registro));
			registro->key =buscarEnMemoriaPrincipalKey(pagina->indiceMemoria, memoriaPrincipal);
			registro->timestamp = buscarEnMemoriaPrincipalTimeStamp( pagina->indiceMemoria, memoriaPrincipal);
			registro->value =  buscarEnMemoriaPrincipal( pagina->indiceMemoria, memoriaPrincipal);
			journal->nombreTabla= segmento->nombreTabla;
			journal-> registro= registro;

			list_add(listaJournal,(void*)journal);
			}
		}

	}
	puts("-------------------------------------------------------------------");
	printf("[INFO]: Elementos en la lista de JOURNAL...\n");
	mostrarElementosListaJournal();
	puts("-------------------------------------------------------------------");
	char* msjEnviado = convertirAStringListaJournal();
	borrarTodo(memoriaPrincipal);//borrar todo lo guardado
	empaquetarEnviarMensaje(socketLFS,22,strlen(msjEnviado),msjEnviado);

}

void enviarElemento(){

}



int tamanioArray(void** array){
	int i = 0;
	while(array[i]!=NULL){
		i++;
	}
	return i;
}

int obtenerTimeStamp(){
	return time(NULL)* 0.001;
}

char* recibirRespuestaSELECTMemoriaLfs(){
	return "Hola Soy Lissandra";
}


void quitarEspaciosGuardadosEnMemoria(t_list* lista , t_registro* memoriaPrincipal){
	int tamanio = tamanioLista(lista);
	for(int i =0 ; i< tamanio; i++){
		void * elemento = list_get(lista, i);
		t_pagina * pagina = (t_pagina *) elemento;
		borrarDeMemoria(pagina->indiceMemoria, memoriaPrincipal);
	}
	printf("INFO: Se borraron espacios guardados en Memoria Principal\n");
}

void borrarDeMemoria (int indice, t_registro* memoriaPrincipal){
	memoriaPrincipal[indice].key=NULL;
	memoriaPrincipal[indice].timestamp= NULL;
	memoriaPrincipal[indice].value= NULL;
}

void element_destroyer(void* elemento){
	t_pagina* pagina = (t_pagina*)elemento;
	pagina->contadorVecesSolicitado=NULL;
	pagina->flagModificado=NULL;
	pagina->indiceMemoria= NULL;
	pagina->key= NULL;
	pagina->numeroPag= NULL;
	pagina = NULL;
}

void borrarTablaDePaginas(t_list* lista){
	list_destroy_and_destroy_elements(lista, element_destroyer);
}

char* SELECTMemoria(char * nombreTabla, uint16_t key, t_registro* memoriaPrincipal, int flagModificado){
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
	int cantSegmentos = tamanioLista(tablaDeSegmentos);
	if(ubicacionSegmento!=(cantSegmentos+1)){ //esta en tabla de segmentos
		puts("INFO: Esta en la tabla de SEGMENTOS");
		void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
		t_segmento *segmento =(t_segmento*)elemento;
		char* value = buscarTablaPaginas(segmento->tablaPaginas, key, memoriaPrincipal);// aca tenemos que buscar en la tabla de paginas especifica de este segmento y meternos 1 x 1 en sus paginas para ver si en la memoria Principal esta el key
		if(value!= NULL){ //lo encontro en tabla de paginas, lo busca en memoria principal y devuelve lo que vale
			puts("INFO: Esta en la tabla de PAGINAS");
			return value;
		}
		else{ //no lo encontro en tabla de paginas
			//tengo que consultarle a LFS PERO solo guardo en tabla de paginas
			//consultaSELECTMemoriaLfs();// esto va a mandarle SELECT nombreTabla key con SOCKETS
			puts("INFO: No esta en la tabla de PAGINAS");
			char* value = recibirRespuestaSELECTMemoriaLfs(); //con SOCKETS
			int indice = guardarEnMemoria(nombreTabla, key, value,memoriaPrincipal);
			if(indice==cantMaxPags){
				puts("INFO: No se guardo en la tabla de PAGINAS porque tengo que hacer el JOURNAL");
			}
			else {
				guardarEnTablaDePaginas(segmento, indice, key, flagModificado);
				puts("INFO: Se guardo en la tabla de PAGINAS y en la MEMORIA");
			}
			return value;
		}
	}
	else{// no esta en tabla de segmentos
		//pedirle a lfs y guardar datos en tabla segmentos y tabla paginas
		//consultaSELECTMemoriaLfs();
		puts("INFO: No se encontro en la tabla de SEGMENTOS");
		char* value = recibirRespuestaSELECTMemoriaLfs();
		t_segmento* segmento = guardarEnTablaDeSegmentos(nombreTabla);
		segmento->tablaPaginas= list_create();
		int indice = guardarEnMemoria(nombreTabla, key, value, memoriaPrincipal);
		if(indice== cantMaxPags){
			puts("INFO: No se guardo en la tabla de PAGINAS porque tengo que hacer el JOURNAL");
		}
		else {
			guardarEnTablaDePaginas(segmento, indice, key, flagModificado);
			puts("INFO: Se guardo en tabla de PAGINAS y en tabla de SEGMENTOS");
		}
		return value;
	}

}
void INSERTMemoria(char * nombreTabla, uint16_t key, char* value, int timeStamp, t_registro* memoriaPrincipal){
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
	int cantSegmentos = tamanioLista(tablaDeSegmentos);
	if(ubicacionSegmento!=(cantSegmentos+1)){ //esta en tabla de SEGMENTOS
		puts("INFO: Esta en la tabla de SEGMENTOS");
		void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
		t_segmento *segmento =(t_segmento*)elemento;
		int valor =  buscarEnTablaPaginasINSERT(segmento->tablaPaginas, key, timeStamp, memoriaPrincipal, value );// aca tenemos que buscar en la tabla de paginas especifica de este segmento y meternos 1 x 1 en sus paginas para ver si en la memoria Principal esta el key
		if(valor!= 0){ //lo encontro en tabla de paginas
			//tengo que verificar los timestamps entre ambos a ver cual se queda en memoria principal
			puts("INFO: Esta en la tabla de PAGINAS");
		}
		else{ //no lo encontro en tabla de paginas
			puts("INFO: No esta en la tabla de PAGINAS");
			int indice = guardarEnMemoria(nombreTabla, key, value,memoriaPrincipal);
			if(indice==cantMaxPags){
				puts("INFO: No se guardo en la tabla de PAGINAS porque tengo que hacer el JOURNAL");
			}
			else {
				guardarEnTablaDePaginas(segmento, indice, key, 1);
				puts("INFO: Se guardo en la tabla de PAGINAS y en la MEMORIA");
			}
		}
	}
	else{ // no esta en tabla de SEGMENTOS
		puts("INFO: No se encontro en la tabla de SEGMENTOS");
		int indice = guardarEnMemoria(nombreTabla, key, value, memoriaPrincipal);
		if(indice== cantMaxPags){
			puts("INFO: No se guardo en la tabla de PAGINAS porque tengo que hacer el JOURNAL");
		}
		else {
			t_segmento* segmento = guardarEnTablaDeSegmentos(nombreTabla);
			segmento->tablaPaginas= list_create();
			guardarEnTablaDePaginas(segmento, indice, key, 1);
			puts("INFO: Se guardo en tabla de PAGINAS y en tabla de SEGMENTOS");
		}
	}
}

void DROPMemoria(char* nombreTabla, t_registro* memoriaPrincipal){
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
		int cantSegmentos = tamanioLista(tablaDeSegmentos);
		if(ubicacionSegmento!=(cantSegmentos+1)){
			puts("INFO: Esta en la tabla de SEGMENTOS");
			void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
			t_segmento *segmento =(t_segmento*)elemento;
			quitarEspaciosGuardadosEnMemoria(segmento->tablaPaginas, memoriaPrincipal);
			borrarTablaDePaginas(segmento->tablaPaginas);
			list_remove(tablaDeSegmentos, ubicacionSegmento);
			//aca tengo que avisarle al FS que se borro esta tabla
		}
		else{
			puts("INFO: Dicha tabla no se encuentra en la tabla de SEGMENTOS");
		}
}

int tamanioLista(t_list * lista){
	return list_size(lista);
}

