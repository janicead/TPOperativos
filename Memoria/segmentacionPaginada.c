#include "segmentacionPaginada.h"

//----------------------------------------GENERALES-------------------------------------------//

void definirTamanioMemoriaPrincipal( int tamanioValueDadoXLFS){
	int tamanioMaxMemoria = 52; //de archivo de config
	tamanioDadoPorLFS= tamanioValueDadoXLFS;
	tamanioUnRegistro = tamanioValueDadoXLFS + sizeof(unsigned long int) + sizeof(uint16_t); //6+ tamanioValueDado
	obtenerValue = sizeof(unsigned long int) + sizeof(uint16_t);
	memoriaPrincipal = malloc(tamanioMaxMemoria);
	printf("el tamanio de un resgistro es de %d\n", tamanioUnRegistro);
	cantMaxMarcos = tamanioMaxMemoria/ tamanioUnRegistro;
	cantMarcosIngresados= 0;
	marcosOcupados=(int*)calloc(cantMaxMarcos,sizeof(int));
	tablaDeSegmentos= list_create();

}

int tamanioLista(t_list * lista){
	return list_size(lista);
}

char* recibirRespuestaSELECTMemoriaLfs(){
	return "Hola Soy Lissandra";
}

int obtenerTimeStamp(){
	return time(NULL);
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
		list_destroy_and_destroy_elements(tablaDeSegmentos, (void*)destructor);
}

void borrarTodo(){
	for(int i = 0; i< cantMaxMarcos; i ++){
		marcosOcupados[i]=0;
		}
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
	return cantSegmentos+1;
}

t_segmento* guardarEnTablaDeSegmentos(char* nombreTabla){
	int cantPaginasGuardadas;
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->nombreTabla =strdup( nombreTabla);
	segmento->tablaPaginas= list_create();
	list_add(tablaDeSegmentos, (void*)segmento);
	return segmento;
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
//----------------------------------------PAGINAS----------------------------------------------------------//

char* buscarTablaPaginas(t_list* tabla, uint16_t key){

	int cantPaginas = tamanioLista(tabla);
	for(int i = 0; i<cantPaginas; i++){
		void * elemento = list_get(tabla, i);
		t_pagina *pagina =(t_pagina*)elemento;
		if(pagina->key ==key){
		t_registro* registro = buscarEnMemoriaPrincipal(pagina->numeroMarco);
		pagina->contadorVecesSolicitado++;
		printf("Cantidad veces solicitado %d\n", pagina->contadorVecesSolicitado);
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
		puts("INFO: No hay PAGINAS en la TABLA DE PAGINAS");
	}
	for(int i = 0 ; i<tamanioTP; i++){
		void* elemento = list_get(lista, i);
		t_pagina* pagina = (t_pagina*) elemento;
		printf("NUMERO DE PAG %d, SU KEY ES %d y su INDICE EN MEMORIA ES %d\n", pagina->numeroPag, pagina->key, pagina->numeroMarco);
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
				puts("INFO: Se actualizo la MEMORIA PRINCIPAL");
			}
			return timeStamp;
		}
	}
	return 0;
}

void destructorPaginas(void* elemento){
	t_pagina* pagina = (t_pagina*)elemento;
	pagina->contadorVecesSolicitado=NULL;
	pagina->flagModificado=NULL;
	pagina->numeroMarco= NULL;
	pagina->key= NULL;
	pagina->numeroPag= NULL;
	pagina = NULL;
}

void borrarTablaDePaginas(t_list* lista){
	list_destroy_and_destroy_elements(lista, destructorPaginas);
}

//------------------------------------------MEMORIA------------------------------------------//

t_registro* buscarEnMemoriaPrincipal( int nroMarco){
	t_registro * registro = malloc(sizeof(t_registro));
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
	for(int i = 0; i< cantMaxMarcos; i ++){
		if(marcosOcupados[i]==0){
			return i;
		}
	}
	return cantMaxMarcos;
}

void settearMarcoEnMP(int nroMarco, int nroDeseado){
	marcosOcupados[nroMarco]=nroDeseado;
}

void actualizarMemoriaPrincipal(int nroMarco, unsigned long int timeStamp, char* value){
	t_registro * registro = malloc(sizeof(t_registro));
	registro->value = value;
	registro->timestamp= timeStamp;
	memcpy(memoriaPrincipal+ nroMarco*tamanioUnRegistro+sizeof(uint16_t), &registro->timestamp, sizeof(unsigned long int));
	memcpy(memoriaPrincipal+nroMarco*tamanioUnRegistro+sizeof(uint16_t)+ sizeof(unsigned long int), registro->value, tamanioDadoPorLFS);
	free(registro);
}

void guardarEnMPLugarEspecifico(uint16_t key, char* value, int nroMarco){
	t_registro * registro = malloc(sizeof(t_registro));
	registro->key = key;
	registro->value= value;
	registro->timestamp= obtenerTimeStamp();
	memcpy(memoriaPrincipal+ nroMarco*tamanioUnRegistro, &registro->key, sizeof(uint16_t));
	memcpy(memoriaPrincipal+ nroMarco*tamanioUnRegistro+sizeof(uint16_t), &registro->timestamp, sizeof(unsigned long int));
	memcpy(memoriaPrincipal+nroMarco*tamanioUnRegistro+sizeof(uint16_t)+ sizeof(unsigned long int), registro->value, tamanioDadoPorLFS);
	cantMarcosIngresados++;
	settearMarcoEnMP(nroMarco, 1);
	free(registro);
}

int guardarEnMemoria(char* nombreTabla, uint16_t key, char* value){
	int nroMarco = buscarEspacioLibreEnMP();
	if(nroMarco!=cantMaxMarcos){
		guardarEnMPLugarEspecifico(key, value, nroMarco);
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
			printf("INFO: La PAGINA que voy a reemplazar es la nro %d del SEGMENTO '%s' \n", lru->numeroPag, lru->nombreTabla);
			settearMarcoEnMP(pagina->numeroMarco, 0);
			guardarEnMPLugarEspecifico(key, value, pagina->numeroMarco);
			free(lru);
			int nroMarco = pagina->numeroMarco;
			free(pagina);

			return nroMarco;
		}
		else {
			puts("INFO: Tengo que realizar JOURNAL\n");
			iniciarJournal();// se inicia journal, osea que queda vacia la memoria entonces tiene si o si espacio
			//luego de realizar journal, se terminaria guardando el dato :D
			nroMarco = buscarEspacioLibreEnMP();
			guardarEnMPLugarEspecifico(key, value, nroMarco);
			free(lru);
			return 0;
		}
	}
}

void mostrarElementosMemoriaPrincipal(){
	 uint16_t *key = 0;
	 unsigned long int* eltimestamp =0;
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
		 printf("LA KEY ES %d\n",key);
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
	printf("INFO: Se borraron espacios guardados en Memoria Principal\n");
}
void borrarTodaMemoria(){
	for(int i = 0; i<cantMaxMarcos; i++){
		settearMarcoEnMP(i, 0);
	}
}
//---------------------------------------LRU-------------------------------------------------------//

t_LRU * LRU (){
	int cantVecesSolicitadaMinimo = 0;
	int paginaMenosCantVecesSolicitada = cantMaxMarcos;
	int esElPrimerElemento = 0;
	t_LRU * lru = malloc (sizeof(t_LRU));
	lru->numeroPag= cantMaxMarcos;
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

//-------------------------------------JOURNAL----------------------------------------------------//

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
		sprintf(numeroTimestamp, "%lu", journal->registro->timestamp);
		string_append(&elementoEnviar, numeroTimestamp);
		string_append(&elementoEnviar, "/");

		//printf("La key es %d", journal->registro->key);
		//printf("El timestamp es %lu", journal->registro->key);
		//printf("La key es %d", journal->registro->key);

		free(journal);
	}
	printf("[INFO] LA LISTA ES----> %s \n", elementoEnviar);
	return elementoEnviar;
}


// INSERT TABLA1 3 "CHAU" 4
// INSERT TABLA2 4 "HOLA" 9
// INSERT TABLA3 6 "SI" 10

void iniciarJournal(){
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
			t_registro* registro =buscarEnMemoriaPrincipal( pagina->numeroMarco);
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
	//char* msjEnviado = convertirAStringListaJournal();
	//printf("El mensaje que se va a enviar es: %s\n", msjEnviado);
	borrarTodaMemoria();
	printf("ACA BORRARIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA \n");
	/*int c = tamanioLista(tablaDeSegmentos);
	for(int i = 0; i <c; i++){
		void* elemento = list_get(tablaDeSegmentos, i);
		t_segmento * segmento = (t_segmento*) elemento;
		DROPMemoria(segmento->nombreTabla);
	}*/

	//borrarTodo();//borrar todo lo guardado
	//empaquetarEnviarMensaje(socketLFS,22,strlen(msjEnviado),msjEnviado);
	//free(msjEnviado);
}

//----------------------------------------------------REQUESTS-------------------------------------------------//


char* SELECTMemoria(char * nombreTabla, uint16_t key, int flagModificado){
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
	int cantSegmentos = tamanioLista(tablaDeSegmentos);
	if(ubicacionSegmento!=(cantSegmentos+1)){ //esta en tabla de segmentos
		puts("INFO: Esta en la tabla de SEGMENTOS");
		void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
		t_segmento *segmento =(t_segmento*)elemento;
		char* value = buscarTablaPaginas(segmento->tablaPaginas, key);// aca tenemos que buscar en la tabla de paginas especifica de este segmento y meternos 1 x 1 en sus paginas para ver si en la memoria Principal esta el key
		if(value!= NULL){ //lo encontro en tabla de paginas, lo busca en memoria principal y devuelve lo que vale
			puts("INFO: Esta en la tabla de PAGINAS");
			return value;
		}
		else{ //no lo encontro en tabla de paginas
			//tengo que consultarle a LFS PERO solo guardo en tabla de paginas
			//consultaSELECTMemoriaLfs();// esto va a mandarle SELECT nombreTabla key con SOCKETS
			puts("INFO: No esta en la tabla de PAGINAS");
			int nroMarco = guardarEnMemoria(nombreTabla, key, value);
			char* value = recibirRespuestaSELECTMemoriaLfs(); //con SOCKETS
			guardarEnTablaDePaginas(segmento, nroMarco, key, flagModificado);
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
		int nroMarco = guardarEnMemoria(nombreTabla, key, value);
		guardarEnTablaDePaginas(segmento, nroMarco, key, flagModificado);
		puts("INFO: Se guardo en MP, en tabla de PAGINAS y en tabla de SEGMENTOS");
		return value;
	}

}


void INSERTMemoria(char * nombreTabla, uint16_t key, char* value, int timeStamp){
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
	int cantSegmentos = tamanioLista(tablaDeSegmentos);
	if(ubicacionSegmento!=(cantSegmentos+1)){ //esta en tabla de SEGMENTOS
		puts("INFO: Esta en la tabla de SEGMENTOS");
		void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
		t_segmento *segmento =(t_segmento*)elemento;
		int valor =  buscarEnTablaPaginasINSERT(segmento->tablaPaginas, key, timeStamp, value );// aca tenemos que buscar en la tabla de paginas especifica de este segmento y meternos 1 x 1 en sus paginas para ver si en la memoria Principal esta el key
		if(valor!= 0){ //lo encontro en tabla de paginas
			//tengo que verificar los timestamps entre ambos a ver cual se queda en memoria principal
			puts("INFO: Esta en la tabla de PAGINAS");
		}
		else{ //no lo encontro en tabla de paginas
			puts("INFO: No esta en la tabla de PAGINAS");
			int indice = guardarEnMemoria(nombreTabla, key, value);
			guardarEnTablaDePaginas(segmento, indice, key, 1);
			puts("INFO: Se guardo en la tabla de PAGINAS y en la MEMORIA");
		}
	}
	else{ // no esta en tabla de SEGMENTOS
		puts("INFO: No se encontro en la tabla de SEGMENTOS");
		int indice = guardarEnMemoria(nombreTabla, key, value);
		t_segmento* segmento = guardarEnTablaDeSegmentos(nombreTabla);
		segmento->tablaPaginas= list_create();
		guardarEnTablaDePaginas(segmento, indice, key, 1);
		puts("INFO: Se guardo en MP, en tabla de PAGINAS y en tabla de SEGMENTOS");
	}

}

void DROPMemoria(char* nombreTabla){
	int ubicacionSegmento = buscarTablaSegmentos(nombreTabla);  // Busco la tabla en mi tabla de Segmentos
		int cantSegmentos = tamanioLista(tablaDeSegmentos);
		if(ubicacionSegmento!=(cantSegmentos+1)){
			puts("INFO: Esta en la tabla de SEGMENTOS");
			void * elemento = list_get(tablaDeSegmentos, ubicacionSegmento);
			t_segmento *segmento =(t_segmento*)elemento;
			quitarEspaciosGuardadosEnMemoria(segmento->tablaPaginas);
			borrarTablaDePaginas(segmento->tablaPaginas);
			list_remove(tablaDeSegmentos, ubicacionSegmento);
			//aca tengo que avisarle al FS que se borro esta tabla
		}
		else{
			puts("INFO: Dicha tabla no se encuentra en la tabla de SEGMENTOS");
			//aca tengo que avisarle al FS que borre la tabla
		}
}


