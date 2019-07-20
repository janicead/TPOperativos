#include "lfs.h"
//#include "fs.h"

int socketLFS; //SOCKET ESCUCHANDO, SERVIDOR
#define printLOGGER 0  //SI=1,  NO=0

int main(void) {

	leerConfigLFS();
	mostrarValoresDeConfig();

	crearListasGenerales();

	pthread_mutex_init(&laMEMTABLE,NULL);
	pthread_mutex_init(&elFS,NULL);
	pthread_mutex_init(&LISSANDRA,NULL);

	iniciarFileSystemLISSANDRA();

	cargarTablasPersistidasEnMEMTABLE();

	iniciarEscucha();
	pthread_t hiloIDAsignador;
	pthread_create( &hiloIDAsignador , NULL ,  asignadorLISSANDRA , NULL);

	pthread_t hiloIdDUMP;
	if( pthread_create( &hiloIdDUMP , NULL ,  hiloDUMP , NULL) < 0)
	{
		perror("No se pudo crear el hilo DUMP");
		exitLFS(1);
	}

	printf("\nCONSOLA...\n");
	consolaAPI();

	destruirListasGenerales();

	exitLFS(0);

	return 0; //T_SUCCESS;
}

void leerConfigLFS()
{
	archivoConfig = config_create("../../lfs.conf");
	//archivoConfig = config_create("/home/utnso/workspace/backUpTP1C2019/tp-2019-1c-BEFGN/lfs.conf");
	logger = log_create("lfs.log", "lfs", printLOGGER,LOG_LEVEL_INFO);

	log_info(logger,"\n\nINICIANDO PROCESO LFS...");
	log_info(logger, "Leyendo archivo de configuracion...");


	if (config_has_property(archivoConfig, "PUERTO_ESCUCHA"))
		configLFS.puertoEscucha = config_get_int_value(archivoConfig,"PUERTO_ESCUCHA");
	else
	{
		log_error(logger,"No se encontro la key PUERTO_ESCUCHA en el archivo de configuracion");
		exitLFS(EXIT_FAILURE);
	}

	if (config_has_property(archivoConfig, "PUNTO_MONTAJE"))
		configLFS.puntoMontaje = config_get_string_value(archivoConfig,"PUNTO_MONTAJE");
	else
	{
		log_error(logger,"No se encontro la key PUNTO_MONTAJE en el archivo de configuracion");
		exitLFS(EXIT_FAILURE);
	}

	if (config_has_property(archivoConfig, "RETARDO"))
		configLFS.retardo = config_get_int_value(archivoConfig,"RETARDO");
	else
	{
		log_error(logger,"No se encontro la key RETARDO en el archivo de configuracion");
		exitLFS(EXIT_FAILURE);
	}

	if (config_has_property(archivoConfig, "TAMAÑO_VALUE"))
		configLFS.tamanioValue = config_get_int_value(archivoConfig,"TAMAÑO_VALUE");
	else
	{
		log_error(logger,"No se encontro la key TAMAÑO_VALUE en el archivo de configuracion");
		exitLFS(EXIT_FAILURE);
	}

	if (config_has_property(archivoConfig, "TIEMPO_DUMP"))
		configLFS.tiempoDump = config_get_int_value(archivoConfig,"TIEMPO_DUMP");
	else
	{
		log_error(logger,"No se encontro la key TIEMPO_DUMP en el archivo de configuracion");
		exitLFS(EXIT_FAILURE);
	}
}

void mostrarValoresDeConfig(void)
{
	char *unBuffer = string_from_format("\n\nPUERTO_ESCUCHA: %d\nPUNTO_MONTAJE: %s\nRETARDO: %d\nTAMAÑO_VALUE: %d\nTIEMPO_DUMP: %d\n\n",configLFS.puertoEscucha,configLFS.puntoMontaje,configLFS.retardo,configLFS.tamanioValue,configLFS.tiempoDump);
	log_info(logger,unBuffer);
	/*
	printf("\nPUERTO_ESCUCHA: %d\n",configLFS.puertoEscucha);
	printf("PUNTO_MONTAJE: %s\n",configLFS.puntoMontaje);
	printf("RETARDO: %d\n",configLFS.retardo);
	printf("TAMAÑO_VALUE: %d\n",configLFS.tamanioValue);
	printf("TIEMPO_DUMP: %d\n\n",configLFS.tiempoDump);*/
	free(unBuffer);
}

void exitLFS(int return_nr)
{
	config_destroy(archivoConfig);
	//config_destroy(archivoMetadata);

	config_destroy(archivoMetadata);
	log_destroy(logger);

	pthread_mutex_destroy(&laMEMTABLE);
	pthread_mutex_destroy(&elFS);

	pthread_mutex_destroy(&LISSANDRA);
	//close(socketLFS);
	exit(return_nr);
}

void consolaAPI()
{
	char *linea;
	char **operacion;
	int cantArgumentos;

	while(1)
	{
	    linea = readline(">");
	    if(strlen(linea) != 0) {
	    //printf("strlen(linea): %d\n",strlen(linea));
	    if(linea)
	    	add_history(linea);

	    //operacion = string_split(linea," ");
	    operacion = splitDeOperaciones(linea);

	    free(linea); //ES NECESARIO
	    cantArgumentos = longitudArrayDePunteros(operacion) - 1; //XQ LA OPERACION CUENTA
	    string_to_upper(operacion[0]);

	    //realiarRetardo(configLFS.retardo);

	    if(!strcmp(operacion[0], "EXIT")) //###
	    {
	    	if(cantArgumentos == 0)
	    	{
	    		log_info(logger,"Finalizo LISSANDRA FS");
	    		break;
	    	}
	    	else
	    		printf("Esta operacion no admite parametros\n");

	    }
	    else if(!strcmp(operacion[0], "SELECT")) //###
	    {
	    	if(cantArgumentos == 2)
	    	{
	    		log_info(logger,"\n>SELECT [%s][%s]\n",operacion[1],operacion[2]);

	    		t_SELECT *unSELECT = definirT_SELECT(operacion[1],atoi(operacion[2]));
	    		//IMPRIMO POR PANTALLA EL RESULTADO DE LA OPERACION
	    		char *respuesta = realizarSELECT(unSELECT);

	    		printf("Respuesta: %s\n",respuesta);
	    		log_info(logger,"Respuesta: %s\n------------------------------------------",respuesta);
	    		free(respuesta);
	    		freeT_SELECT(unSELECT);
	    	}
	    	else
	    	{
	    		printf("\nFalta, o hay exceso, de parametros de SELECT\n");
	    		printf("SELECT [NOMBRE_TABLA] [KEY]\n\n");
	    	}

	    }
	    else if(!strcmp(operacion[0], "INSERT")) //###
	    {
	    	if(cantArgumentos == 3)
	    	{
	    		log_info(logger,"\n>INSERT [%s][%s][%s]",operacion[1],operacion[2],operacion[3]);
	    		//IMPRIMO POR PANTALLA EL RESULTADO DE LA OPERACION
	    		t_INSERT *unINSERT = definirT_INSERT(operacion[1],atoi(operacion[2]),operacion[3],0);
	    		//printf("\ntamanioLISTA: %d  (before memtable)\n",list_size(memTable)); ///
	    		char* respuesta = realizarINSERT(unINSERT);  //TESTEAR
	    		//printf("tamanioLISTA: %d  (after memtable)\n",list_size(memTable)); ///
	    		printf("Respuesta: %s\n",respuesta);
	    		log_info(logger,"Respuesta: %s\n------------------------------------------",respuesta);
	    		free(respuesta);
	    		freeT_INSERT(unINSERT);
	    	}
	    	else if(cantArgumentos == 4)
	    	{
	    		log_info(logger,"\n>INSERT [%s][%s][%s][%s]",operacion[1],operacion[2],operacion[3],operacion[4]);
	    		//IMPRIMO POR PANTALLA EL RESULTADO DE LA OPERACION
	    		t_INSERT *unINSERT = definirT_INSERT(operacion[1],atoi(operacion[2]),operacion[3],atoi(operacion[4]));
	    		//printf("\ntamanioLISTA: %d  (before memtable)\n",list_size(memTable)); ///
	    		char* respuesta = realizarINSERT(unINSERT);  //TESTEAR
	    		//printf("tamanioLISTA: %d  (after memtable)\n",list_size(memTable)); ///
	    		printf("Respuesta: %s\n",respuesta);
	    		log_info(logger,"Respuesta: %s\n------------------------------------------",respuesta);
	    		free(respuesta);
	    		freeT_INSERT(unINSERT);
	    	}
	    	else
	    	{
	    		printf("\nFalta, o hay exceso, de paramentros de INSERT\n");
	    		printf("INSERT [NOMBRE_TABLA] [KEY] “[VALUE]” (Timestamp)\n\n");
	    	}

	    }
	    else if(!strcmp(operacion[0], "CREATE")) //###
	    {
	    	if(cantArgumentos == 4)
	    	{
	    		log_info(logger,"\n>CREATE [%s][%s][%s][%s]\n",operacion[1],operacion[2],operacion[3],operacion[4]);

	    		t_CREATE *unCREATE = definirT_CREATE(operacion[1],operacion[2],atoi(operacion[3]),atoi(operacion[4]));
	    		char *respuesta = realizarCREATE(unCREATE);
	    		printf("Respuesta: %s\n",respuesta);
	    		log_info(logger,"Respuesta: %s\n------------------------------------------",respuesta);
	    		free(respuesta);
	    		freeT_CREATE(unCREATE);
	    		//mostrarBitsDeBloques(8);///

	    	}
	    	else
	    	{
	    		printf("\nFalta, o hay exceso, de parametros de CREATE\n");
	    		printf("CREATE [NOMBRE_TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]\n\n");
	    	}
	    }
	    else if(!strcmp(operacion[0], "DESCRIBE")) //###
	    {
	    	//EL UNICO ARGUMENTO PUEDE SER OPCIONAL
	    	if(cantArgumentos == 0)
	    	{
	    		log_info(logger,"\n>DESCRIBE [Sin Parametros]\n");

	    		t_DESCRIBE *unDESCRIBE = definirT_DESCRIBE("ALL_TABLES");
	    		char *respuesta = realizarDESCRIBE(unDESCRIBE);
	    		//printf("\nPor SOCKET: %s\n",respuesta);
	    		if(strcmp(respuesta,"NO_EXISTEN_TABLAS") == 0)
	    		{
	    			printf("Respuesta: \n%s\n",respuesta);
	    			log_info(logger,"Respuesta: \n%s\n------------------------------------------",respuesta);
	    		}
	    		else
	    		{
	    			char *respuestaAPRINTEAR = respuestaDESCRIBEaPrintear(respuesta);
	    			printf("Respuesta: \n%s\n",respuestaAPRINTEAR);
	    			log_info(logger,"Respuesta: \n%s\n------------------------------------------",respuestaAPRINTEAR);
	    			free(respuestaAPRINTEAR);
	    		}

	    		free(respuesta);
	    		freeT_DESCRIBE(unDESCRIBE);
	    	}
	    	else if(cantArgumentos == 1)
	    	{
	    		log_info(logger,"\n>DESCRIBE [%s]\n",operacion[1]);

	    		t_DESCRIBE *unDESCRIBE = definirT_DESCRIBE(operacion[1]);
	    		char *respuesta = realizarDESCRIBE(unDESCRIBE);
	    		//printf("\nPor SOCKET: %s\n",respuesta);
	    		if(strcmp(respuesta,"NO_EXISTE_TABLA") == 0)
	    		{
	    			printf("Respuesta: \n%s\n",respuesta);
	    			log_info(logger,"Respuesta: \n%s\n------------------------------------------",respuesta);
	    		}
	    		else
	    		{
	    			char *respuestaAPRINTEAR = respuestaDESCRIBEaPrintear(respuesta);
	    			printf("Respuesta: \n%s\n",respuestaAPRINTEAR);
	    			log_info(logger,"Respuesta: \n%s\n------------------------------------------",respuestaAPRINTEAR);
	    			free(respuestaAPRINTEAR);
	    		}
	    		free(respuesta);
	    		freeT_DESCRIBE(unDESCRIBE);
	    	}
	    	else
	    	{
	    		printf("\nExceso de parametros de DESCRIBE\n");
	    		printf("DESCRIBE (NOMBRE_TABLA)\n\n");
	    	}
	    }
	    else if(!strcmp(operacion[0], "DROP")) //###
	    {
	    	if(cantArgumentos == 1)
	    	{
	    		log_info(logger,"\n>DROP [%s]\n",operacion[1]);

	    		t_DROP *unDROP = definirT_DROP(operacion[1]);
	    		char *respuesta = realizarDROP(unDROP);

	    		printf("Respuesta: %s\n",respuesta);
	    		log_info(logger,"Respuesta: %s\n------------------------------------------",respuesta);
	    		free(respuesta);
	    		freeT_DROP(unDROP);
	    	}
	    	else
	    	{
	    		printf("\nFalta, o hay exceso, de parametros de DROP\n");
	    		printf("DROP [NOMBRE_TABLA]\n\n");
	    	}
	    }
	    else
	    {
	    	log_info(logger,"Operacion no valida por la API");
	    	//realizarDUMP(); ///
	    	/*t_list *unArchivoComoLista;
	    	unArchivoComoLista = obtenerArchivoComoLista("t4/dump0.tmp");
	    	printf("\nlist_size(unArchivoComoLista): %d\n",list_size(unArchivoComoLista));
	    	*/

	    	//char *unPath = string_from_format("%s/Tables",configLFS.puntoMontaje);
	    	//char **listado = enlistarElPath(unPath);

	    	/*char *unPath = string_from_format("t1");
	    	char **listado = enlistarCarpetaTabla(unPath);
	    	int j;
	    	for(j=0;listado[j]!=NULL;j++)
	    		printf("%s\n",listado[j]);
	    	free(unPath);
	    	freeArrayDePunteros(listado);*/

	    	/*t_Tabla *tablaEncontrada = existeEnMemtable("t1");
	    	realizarCOMPACTAR(tablaEncontrada,2);*/

	    	//remove("/home/utnso/workspace/tp-2019-1c-BEFGN/LISSANDRA_FS/asd");

	    	/*char *nombreActual = string_from_format("%s/Tables/%s/antes.before",configLFS.puntoMontaje,"t1");
	    	char *nombreNuevo = string_from_format("%sc",nombreActual);

	    	rename(nombreActual,nombreNuevo);*/

	    	/*unsigned  long int ini = 4000000000;
	    	uint32_t fin;

	    	fin = ini;
	    	printf("\nini: %lu\n",ini);
	    	printf("\nfin (lu): %lu\n",fin);
	    	printf("\nfin (d): %d\n",fin);*/


	    }

	    //freeArrayDePunteros(operacion);
	 }//if
	}
}

void iniciarEscucha()
{
	struct sockaddr_in dirServidor;

	int skEnUso;

	dirServidor.sin_family = AF_INET;
	dirServidor.sin_port = htons(configLFS.puertoEscucha); //serverPort
	dirServidor.sin_addr.s_addr = htonl(INADDR_ANY);

	//printf("IP server: %d\n",dirServidor.sin_addr.s_addr);

	log_info(logger, "Asignado un socket al proceso LFS");
	socketLFS = socket(AF_INET, SOCK_STREAM, 0);

	skEnUso = 1;
	setsockopt(socketLFS,SOL_SOCKET,SO_REUSEADDR,&skEnUso,sizeof(skEnUso));

	if(bind(socketLFS,(void*) &dirServidor,sizeof(dirServidor)) != 0)
	{
		log_error(logger, "Fallo del bind()");
		exitLFS(0);
	}

	if(listen(socketLFS,maximoDeConexiones)==0)
	{
		//printf("Esperando conexion del Diego (DAM)...");
		log_info(logger, "Esperando conexiones, listen()....");
	}
	else
	{
		log_error(logger,"fallo al establecer escucha, listen()");
		exitLFS(0);
	}

	printf("\n");

}

void realizarMultiplexacion(int socketEscuchando) //EN LISTEN()
{
	//
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
	while(i < 15) //1)  //OJO ACA !!!
	{
		FD_ZERO(&tempReadSet);
		tempReadSet = readSet; //NOSE SI ESTA BIEN ASI DE PREPO
		//memset(tempReadSet,readSet,sizeof(readSet));

		//memcpy(&tempReadSet, &readSet, sizeof(tempReadSet));
		resultado = select(maximoFD + 1, &tempReadSet, NULL, NULL, NULL);
		//printf("\nResultadoSELECT: %d\n",resultado); ///

		if(resultado == -1)
		{
			printf("Fallo en select().\n");
			exitLFS(1);
		}
		//VERIFICO SI HAY NUEVA CONEXION
		if (FD_ISSET(socketEscuchando, &tempReadSet))
		{
			nuevaConexion = aceptarConexiones(socketEscuchando);
			FD_SET(nuevaConexion,&readSet);
			maximoFD = (nuevaConexion > maximoFD)?nuevaConexion:maximoFD;
			FD_CLR(socketEscuchando,&tempReadSet); //LO SACO PARA Q SOLO ME QUEDEN FDs DE CONEXIONES PARA RECIVIR ALGUN PAQUETE
		}
		//BUSCO EN EL CONJUNTO, si hay datos para leer
		for(fd = 0; fd <= maximoFD; fd++)
		{
			if (FD_ISSET(fd, &tempReadSet)) //HAY UN PAQUETE PARA RECIBIR
			{
				package = recibirPaquete(fd); //MODIFICAR LA FUNCION PARA Q DEVUELVA NULL (SE DESCONECTO)
				//printf("fd: %d\n",fd); ///
				//printf("\npackage->ID: %d\n",package->ID); ///MODIFICAR EL ORDEN EN LA FUNCION DE HANDSHAKE
				realizarProtocoloDelPackage(package, fd); //POSIBLE, PASAR EL SOCKET DE DONDE VIENE EL PAQUETE PARA DEVOLVER RESULTADO
			}
			FD_CLR(fd,&tempReadSet);
			//printf("FOR\n");
		}
		//printf("WHILE\n");
		i++;
		//break;
	}
}

void realizarProtocoloDelPackage(t_PaqueteDeDatos *packageRecibido, int socketEmisor)
{
	//
	if(packageRecibido->ID == 11) //11; HANDSHAKE. 12: ES LA RESPUESTA
	{
		log_info(logger,"Realizando respectivo HandShake");
		char *tamanioValue = string_from_format("%d",configLFS.tamanioValue);
		realizarHandshakeAMemoria(logger,socketEmisor,packageRecibido,tamanioValue);
	}
	//realiarRetardo(configLFS.retardo);
	if(packageRecibido->ID == 13) //13: SELECT, osea q DATOS es una structura t_SELECT
	{
		t_SELECT *unSELECT;

		unSELECT = deserializarT_SELECT(packageRecibido->Datos);
		log_info(logger,"\nQuery recibido: SELECT [%s] [%d]",unSELECT->nombreTabla,unSELECT->KEY);

		char *respuesta = realizarSELECT(unSELECT);

		//printf("Respuesta: %s\n",respuesta); ///
		log_info(logger,"Respuesta: %s\n------------------------------------------",respuesta);

		enviarRespuesta(socketEmisor,14,respuesta); //14: RESPUESTA DE UN PROTOCOLO = 13

		free(respuesta);
		freeT_SELECT(unSELECT); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	if(packageRecibido->ID == 15) //15: INSERT, osea q DATOS es una structura t_INSERT
	{
		t_INSERT *unINSERT;

		unINSERT = deserializarT_INSERT(packageRecibido->Datos);
		log_info(logger,"\nQuery recibido: INSERT [%s] [%d] [%s] [%d]",unINSERT->nombreTabla,unINSERT->KEY,unINSERT->Value,unINSERT->timeStamp);

		//printf("\ntamanioLISTA: %d  (before memtable)\n",list_size(memTable)); ///
		char* respuesta = realizarINSERT(unINSERT);  //TESTEAR
		//printf("tamanioLISTA: %d  (after memtable)\n",list_size(memTable)); ///
		//printf("Respuesta: %s\n",respuesta); ///
		log_info(logger,"Respuesta: %s\n------------------------------------------",respuesta);

		enviarRespuesta(socketEmisor,16,respuesta); //16: RESPUESTA DE UN PROTOCOLO = 15

		free(respuesta);
		freeT_INSERT(unINSERT); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	if(packageRecibido->ID == 17) //17: CREATE, osea q DATOS es una structura t_CREATE
	{
		t_CREATE *unCREATE;

		unCREATE = deserializarT_CREATE(packageRecibido->Datos);
		log_info(logger,"\nQuery recibido: CREATE [%s] [%s] [%d] [%d]",unCREATE->nombreTabla,unCREATE->tipoConsistencia,unCREATE->nParticiones,unCREATE->tiempoCompactacion);

		char *respuesta = realizarCREATE(unCREATE);
		//printf("Respuesta: %s\n",respuesta); ///
		log_info(logger,"Respuesta: %s\n------------------------------------------",respuesta);

		//mostrarBitsDeBloques(8);///

		enviarRespuesta(socketEmisor,18,respuesta); //18: RESPUESTA DE UN PROTOCOLO = 17

		free(respuesta);
		freeT_CREATE(unCREATE); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	if(packageRecibido->ID == 19) //19: GUARDAR DATOS, osea q DATOS es una structura t_DESCRIBE
	{
		t_DESCRIBE *unDESCRIBE;

		unDESCRIBE = deserializarT_DESCRIBE(packageRecibido->Datos);
		log_info(logger,"\nQuery recibido: DESCRIBE [%s]",unDESCRIBE->nombreTabla);

		char *respuesta = realizarDESCRIBE(unDESCRIBE);
		log_info(logger,"Respuesta: %s\n------------------------------------------",respuesta);

		enviarRespuesta(socketEmisor,20,respuesta); //20: RESPUESTA DE UN PROTOCOLO = 19

		free(respuesta);
		freeT_DESCRIBE(unDESCRIBE); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	if(packageRecibido->ID == 21) //11:  ELIMINAR UN ARCHIVO, osea q DATOS es una structura t_DROP
	{
		t_DROP *unDROP;

		unDROP = deserializarT_DROP(packageRecibido->Datos);
		log_info(logger,"\nQuery recibido: DROP [%s]",unDROP->nombreTabla);

		char* respuesta = realizarDROP(unDROP);
		log_info(logger,"Respuesta: %s\n------------------------------------------",respuesta);

		enviarRespuesta(socketEmisor,22,respuesta); //22: RESPUESTA DE UN PROTOCOLO = 21


		free(respuesta);
		freeT_DROP(unDROP); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	freePackage(packageRecibido);

}
//###########################################################
char *realizarSELECT(t_SELECT *unSELECT)
{
	//t_Tabla *unaTabla = existeEnMemtable(unSELECT->nombreTabla);
	//pthread_mutex_lock(&unaTabla->noBloqueado);
	//pthread_mutex_lock(&elFS);
	pthread_mutex_lock(&LISSANDRA);

	char *respuesta = existeTablaEnFS(unSELECT->nombreTabla);

	if(strcmp(respuesta,"YA_EXISTE_TABLA") == 0)
	{
		t_list *listaRegistrosDeKEY = list_create();
		t_list *listaAUX;
		t_list *listaRegistrosFiltrados;

		t_MetadataTabla *unMetadataTabla = obtenerMetadataTabla(unSELECT->nombreTabla);
		//printf("\n[%s]\n%d\n%d\n",unMetadataTabla->Consistency,unMetadataTabla->Partitions,unMetadataTabla->Compaction_Time); ///

		int indexBIN = numeroDeParticion(unMetadataTabla->Partitions,unSELECT->KEY);
		//printf("indexBIN: %d\n",indexBIN);
		char *unNombreTablaArchivo = string_from_format("%s/%d.bin",unSELECT->nombreTabla,indexBIN);

		listaAUX = obtenerArchivoComoLista(unNombreTablaArchivo);
		free(unNombreTablaArchivo);
		//printf("\nlist_size(listaAUX): %d\n",list_size(listaAUX)); ///
		if(list_size(listaAUX) != 0)
		{
			bool conLaMismaKEY(void *elemento)
			{
				return (((t_Registro*)elemento)->KEY == unSELECT->KEY);
			}
			listaRegistrosFiltrados = list_filter(listaAUX,conLaMismaKEY);
			//freeListaDeRegistros(listaAUX); //NO TENGO IDEA XQ NO LO TENGO Q HACER
			//ver si filter referencia o copia

			list_add_all(listaRegistrosDeKEY,listaRegistrosFiltrados);
			//VERIFICAR SI LUEGO listaRegistrosFiltrados ESTA VACIO O HAY Q LIBERAR
		}

		//--------------------------------------------------------
		t_Tabla *tablaEncontrada = existeEnMemtable(unSELECT->nombreTabla);

		if(tablaEncontrada!= NULL)
		{

			//pthread_mutex_lock(&laMEMTABLE);
			int topeTEMPs = list_size(tablaEncontrada->temporales);
			//pthread_mutex_unlock(&laMEMTABLE);
			int i;
			t_ArchivoTemp *unArchivoTemp;

			for(i=0; i < topeTEMPs ;i++)
			{
				//pthread_mutex_lock(&laMEMTABLE);
				unArchivoTemp = list_get(tablaEncontrada->temporales,i);
				//pthread_mutex_unlock(&laMEMTABLE);


				unNombreTablaArchivo = string_from_format("%s/%s",unSELECT->nombreTabla,unArchivoTemp->nombre);

				listaAUX = obtenerArchivoComoLista(unNombreTablaArchivo);
				free(unNombreTablaArchivo);

				bool conLaMismaKEY(void *elemento)
				{
					return (((t_Registro*)elemento)->KEY == unSELECT->KEY);
				}
				listaRegistrosFiltrados = list_filter(listaAUX,conLaMismaKEY);
				//freeListaDeRegistros(listaAUX);

				list_add_all(listaRegistrosDeKEY,listaRegistrosFiltrados);
				//VERIFICAR SI LUEGO listaRegistrosFiltrados ESTA VACIO O HAY Q LIBERAR
			}
			//-----------------------------------------------------
			bool conLaMismaKEY(void *elemento)
			{
				return (((t_Registro*)elemento)->KEY == unSELECT->KEY);
			}

			//pthread_mutex_lock(&laMEMTABLE);
			listaRegistrosFiltrados = list_filter(tablaEncontrada->registros,conLaMismaKEY);
			//pthread_mutex_unlock(&laMEMTABLE);

			//freeListaDeRegistros(listaAUX);

			list_add_all(listaRegistrosDeKEY,listaRegistrosFiltrados);
			//
		}

		free(respuesta);
		//printf("list_size(listaRegistrosDeKEY): %d\n",list_size(listaRegistrosDeKEY)); ///
		if (list_size(listaRegistrosDeKEY) == 0)
		{
			respuesta = string_from_format("NO_EXISTE_KEY");
		}
		else
		{
			bool deMayorAMenorTIMESTAMP(void *elemento1,void *elemento2)
			{
				return (((t_Registro*)elemento1)->TIMESTAMP > ((t_Registro*)elemento2)->TIMESTAMP);
			}

			list_sort(listaRegistrosDeKEY,deMayorAMenorTIMESTAMP);

			t_Registro *elRegistro = list_get(listaRegistrosDeKEY,0);
			//free(respuesta);
			respuesta = string_from_format("%s",elRegistro->VALUE);
			//NO SE HACE FREE DE elRegistro XQ TIENE Q SEGUIR
			//freeListaDeRegistros(listaRegistrosDeKEY);
		}
		//freeListaDeRegistros(listaRegistrosDeKEY);
		//AL PARECER SON REFERENCIAS DE LOS REGISTROS


	}

	//pthread_mutex_unlock(&elFS);
	//pthread_mutex_unlock(&unaTabla->noBloqueado);
	pthread_mutex_unlock(&LISSANDRA);
	return respuesta;
}

//SE VA A TENER Q USAR MUTEX
char *realizarINSERT(t_INSERT *unINSERT)
{
	//t_Tabla *unaTabla = existeEnMemtable(unINSERT->nombreTabla);
	//pthread_mutex_lock(&unaTabla->noBloqueado);
	pthread_mutex_lock(&LISSANDRA);

	char *respuesta = existeTablaEnFS(unINSERT->nombreTabla);

	if(strcmp(respuesta,"YA_EXISTE_TABLA") == 0)
	{
		t_Tabla *tablaEncontrada = existeEnMemtable(unINSERT->nombreTabla);

		if(tablaEncontrada!= NULL)
		{	//YA HAY REGISTROS DE LA TABLA EN MEMTABLE
			t_Registro *nuevoRegistro = malloc(sizeof(t_Registro));

			if(unINSERT->timeStamp == 0)
				//nuevoRegistro.TIMESTAMP = 0.000001 * (unsigned)time(NULL);
				nuevoRegistro->TIMESTAMP = (unsigned long int)time(NULL);
			else
				nuevoRegistro->TIMESTAMP = (unsigned long int)unINSERT->timeStamp;
			//printf("\nTimestamp generado: %lu",nuevoRegistro->TIMESTAMP); ///
			nuevoRegistro->KEY = unINSERT->KEY;
			nuevoRegistro->VALUE = string_from_format("%s",unINSERT->Value);

			//printf("\n698 tamanioLISTA: %d  (before REGISTROS)\n",list_size(tablaEncontrada->registros)); ///

			//pthread_mutex_lock(&laMEMTABLE);
			list_add(tablaEncontrada->registros,nuevoRegistro);
			//pthread_mutex_unlock(&laMEMTABLE);
			//printf("700 tamanioLISTA: %d  (after REGISTROS)\n",list_size(tablaEncontrada->registros)); ///

			respuesta = string_from_format("SUCCESSFUL_INSERT");
		}
		else
		{
			t_Tabla *nuevaTabla = crearTablaEnMEMTABLE(unINSERT->nombreTabla);

			t_Registro *nuevoRegistro = malloc(sizeof(t_Registro));

			if(unINSERT->timeStamp == 0)
				//nuevoRegistro.TIMESTAMP = 0.000001 * (unsigned)time(NULL);
				nuevoRegistro->TIMESTAMP = (unsigned long int)time(NULL);
			else
				nuevoRegistro->TIMESTAMP = (unsigned long int)unINSERT->timeStamp;

			//printf("\nTimestamp generado: %lu",nuevoRegistro->TIMESTAMP); ///
			nuevoRegistro->KEY = unINSERT->KEY;
			nuevoRegistro->VALUE = string_from_format("%s",unINSERT->Value);

			//printf("\n539 tamanioLISTA: %d  (before REGISTROS)\n",list_size(nuevaTabla->registros)); ///

			//pthread_mutex_lock(&laMEMTABLE);
			list_add(nuevaTabla->registros,nuevoRegistro);
			//pthread_mutex_unlock(&laMEMTABLE);
			//printf("541 tamanioLISTA: %d  (before REGISTROS)\n",list_size(nuevaTabla->registros)); ///

			respuesta = string_from_format("SUCCESSFUL_INSERT");
		}


	}
	//pthread_mutex_unlock(&unaTabla->noBloqueado);
	pthread_mutex_unlock(&LISSANDRA);
	return respuesta;
}

t_Tabla *crearTablaEnMEMTABLE(char *nombreTabla)
{
	t_Tabla *nuevaTabla = malloc(sizeof(t_Tabla));

	nuevaTabla->nombreTabla = string_from_format("%s",nombreTabla);
	nuevaTabla->registros = list_create();
	nuevaTabla->temporales = list_create();

	pthread_mutex_init(&(nuevaTabla->noBloqueado), NULL);

	//pthread_mutex_lock(&laMEMTABLE);
	//OBTENER LA METADATA PARA SABER TIMEPO DE COMPACTACION <-- ya en hiloCOMPACTADOR()
	//CREARIA EL HILO COMPACTADOR Y GUARDO EL hiloID en nuevaTabla->hiloIDCompactador
	if( pthread_create( &nuevaTabla->hiloIDCompactador , NULL ,  (void*)hiloCOMPACTADOR , (void*) nuevaTabla) < 0) //(void*) nuevaTabla NO VA CON &
	{
		perror("No se pudo crear el hilo COMPACTADOR");
		exitLFS(1);
	}
	else
		log_info(logger,"\nSe creo hilo Compactador de [%s]",nuevaTabla->nombreTabla);

	list_add(memTable,nuevaTabla);

	//pthread_mutex_unlock(&laMEMTABLE);

	return nuevaTabla;
}

void borrarTablaEnMEMTABLE(char *unNombreTabla)
{
	//pthread_mutex_lock(&laMEMTABLE);

	bool existeTabla(void * elemento)
	{
		return (strcmp(((t_Tabla*)elemento)->nombreTabla,unNombreTabla) == 0);
	}

	t_Tabla *tablaEncontrada = (t_Tabla *)list_remove_by_condition(memTable,existeTabla);

	//pthread_mutex_lock(&tablaEncontrada->noBloqueado);

	if(tablaEncontrada != NULL)
	{
		freeT_Tabla(tablaEncontrada);
	}
	else
	{
		//printf("\nNo se encontro [%s] en MEMTABLE\n",unNombreTabla); ///
	}

	//pthread_mutex_unlock(&tablaEncontrada->noBloqueado);

	//pthread_mutex_unlock(&laMEMTABLE);
}

t_Tabla *existeEnMemtable(char *nombreTabla)
{
	bool existeTabla(void * elemento)
	{
		return (strcmp(((t_Tabla*)elemento)->nombreTabla,nombreTabla) == 0);
	}

	//pthread_mutex_lock(&laMEMTABLE);
	t_Tabla *unaTabla = (t_Tabla *)list_find(memTable,existeTabla);
	//pthread_mutex_unlock(&laMEMTABLE);

	return unaTabla;
}

char *realizarCREATE(t_CREATE *unCREATE)
{
	//t_Tabla *unaTabla = existeEnMemtable(unCREATE->nombreTabla);
	//pthread_mutex_lock(&unaTabla->noBloqueado);
	pthread_mutex_lock(&LISSANDRA);

	char *r = crearTablaEnFS(unCREATE);
	//pthread_mutex_unlock(&unaTabla->noBloqueado);

	pthread_mutex_unlock(&LISSANDRA);

	return r;
}

char *realizarDESCRIBE(t_DESCRIBE *unDESCRIBE)
{
	pthread_mutex_lock(&LISSANDRA);
	char *r = string_new();

	//t_Tabla *unaTabla = existeEnMemtable(unDESCRIBE->nombreTabla);
	//pthread_mutex_lock(&unaTabla->noBloqueado);

	//pthread_mutex_lock(&elFS);

	if(strcmp(unDESCRIBE->nombreTabla,"ALL_TABLES") == 0)
	{
		char *pathTablas = string_from_format("%s/Tables",configLFS.puntoMontaje);
		char **listaTablas = enlistarElPath(pathTablas);

		if(longitudArrayDePunteros(listaTablas) == 0)
		{
			string_append(&r,"NO_EXISTEN_TABLAS");
			//return r;
		}
		int i;
		t_MetadataTabla *unMetadataTabla;
		char *unaTabla;

		free(pathTablas);

		for(i=0; listaTablas[i] != NULL;i++)
		{
			unMetadataTabla = obtenerMetadataTabla(listaTablas[i]);
			unaTabla = string_from_format("#%s;%s;%d;%d",listaTablas[i],unMetadataTabla->Consistency,unMetadataTabla->Partitions,unMetadataTabla->Compaction_Time);
			//printf("\nunaTabla: %s\n",unaTabla);
			string_append(&r,unaTabla);
			free(unaTabla);
			freeT_MetadataTabla(unMetadataTabla);
		}

		freeArrayDePunteros(listaTablas);
	}
	else
	{
		r = existeTablaEnFS(unDESCRIBE->nombreTabla);

		if(strcmp(r,"YA_EXISTE_TABLA") == 0)
		{
			free(r);
			t_MetadataTabla *unMetadataTabla;

			unMetadataTabla = obtenerMetadataTabla(unDESCRIBE->nombreTabla);
			r = string_from_format("%s;%s;%d;%d",unDESCRIBE->nombreTabla,unMetadataTabla->Consistency,unMetadataTabla->Partitions,unMetadataTabla->Compaction_Time);

			freeT_MetadataTabla(unMetadataTabla);
		}
	}

	//pthread_mutex_unlock(&elFS);
	//pthread_mutex_unlock(&unaTabla->noBloqueado);
	pthread_mutex_unlock(&LISSANDRA);

	return r;
}
/* NO BORRA A NIVEL MEMTABLE */
char *realizarDROP(t_DROP *unDROP)
{
	pthread_mutex_lock(&LISSANDRA);
	char *r;

	//char *pathTabla = string_from_format("%s/Tables/%s",configLFS.puntoMontaje,unNombreTabla);

	//t_Tabla *unaTabla = existeEnMemtable(unDROP->nombreTabla);
	//pthread_mutex_lock(&unaTabla->noBloqueado);

	//pthread_mutex_lock(&elFS);

	r = existeTablaEnFS(unDROP->nombreTabla);

	if(strcmp(r,"YA_EXISTE_TABLA") == 0)
	{
		free(r);
		char *pathTabla = string_from_format("%s/Tables/%s",configLFS.puntoMontaje,unDROP->nombreTabla);
		char **listaArchivos  = enlistarCarpetaTabla(pathTabla);
		int i;
		char *unPathArchivo;

		for(i=0; listaArchivos[i] != NULL;i++)
		{
			unPathArchivo = string_from_format("%s/%s",unDROP->nombreTabla,listaArchivos[i]);
			//mostrarBitsDeBloques(8);///
			borrarArchivoEnFS(unPathArchivo);
			//mostrarBitsDeBloques(8);///
			free(unPathArchivo);

		}

		unPathArchivo = string_from_format("%s/Metadata",pathTabla);
		remove(unPathArchivo);
		free(unPathArchivo);
		freeArrayDePunteros(listaArchivos);

		remove(pathTabla); //LA CARPETA DEBE ESTAR VACIA PARA Q SE BORRE

		//printf("\ntamanioLISTA: %d  (before memtable)\n",list_size(memTable)); ///
		borrarTablaEnMEMTABLE(unDROP->nombreTabla);
		//printf("\ntamanioLISTA: %d  (after memtable)\n",list_size(memTable)); ///

		r = string_from_format("SUCCESSFUL_DROP");

	}

	//pthread_mutex_unlock(&elFS);
	//pthread_mutex_unlock(&unaTabla->noBloqueado);
	pthread_mutex_unlock(&LISSANDRA);

	return r;
}

void realizarHandshakeAMemoria(t_log *logger,int socketCliente, t_PaqueteDeDatos *packageRecibido,char* msjEnviado)
{

	packageRecibido->Datos[packageRecibido->longDatos] = '\0'; //PAQUETE RECIBIDO

	//log_info(logger,Recibiendo... ID: %d\n",packageRecibido->ID); ///

	if(packageRecibido->ID != 11) //NO ENTRA MAS POR ACA, se verifica anteriormente antes de entrar a esta funcion
	{
		log_info(logger,"el ID de protocolo no es el esperado, cerrando conexion");
		close(socketCliente);
		//exitLFS(1);
	}
	else
	{
		int protocoloID = 12; // 12: RESPUESTA AL HANDSHAKE

		log_info(logger,"Enviando tamanio de pagina: %s...\n\n",msjEnviado);
		empaquetarEnviarMensaje(socketCliente,protocoloID,strlen(msjEnviado),msjEnviado);
		//HAY Q VERIFICAR SI PASA: CLIENTE DESCONECTADO POR ALGUN MOTIVO AL ENVIAR EL SEND
		log_info(logger,"Handshake socket %d: [%s]... DONE!", socketCliente, packageRecibido->Datos);

	}


}

int aceptarConexiones(int socketLFS)
{
	struct sockaddr_in dirCliente;
	unsigned int tamanoDir;

	int skUnaConexion;

	tamanoDir= sizeof(struct sockaddr_in);
	skUnaConexion = accept(socketLFS,(void*) &dirCliente,&tamanoDir);

	if (skUnaConexion == -1)
	{
		log_error(logger, "Fallo en la conexion, accept()");
		exitLFS(0);
	}

	log_info(logger, "Acepte una conexion en socket: %d",skUnaConexion);


	return skUnaConexion;
}
/*
void enviarRespuesta(int socketReceptor, int protocoloID, char *respuesta)
{
	char* stringSerializado;
	t_UnString *unString = definirT_UnString(respuesta);


	//printf("\nRespuesta definida:\n	longString: %d\n	String: %s\n",unString->longString,unString->String);///

	stringSerializado = serializarT_UnString(unString);

	//printf("\nstrlen(pathSerializado): %d\npathSerializado: %s\n",strlen(pathSerializado),pathSerializado);

	//printf("empaquetarEnviarMensaje():\n"); ///

	//ESTO VARIA SEGUN LA STRUCTURA Q PIENSO MANDAR, hay q hacerlo a mano
	int tamanioStructSerializado = sizeof(uint32_t) + unString->longString;

	empaquetarEnviarMensaje(socketReceptor,protocoloID,tamanioStructSerializado,stringSerializado);

	free(stringSerializado);
	freeT_UnString(unString);

	//printf("-----------------------------------------------\n"); ///
}*/
/**/
void *asignadorLISSANDRA(void *arg)
{
	int socketNuevaConexion;

	while( (socketNuevaConexion = aceptarConexiones(socketLFS)) )
	{
		t_Memoria *unaMemoria = malloc(sizeof(t_Memoria));

		if( pthread_create( &unaMemoria->hiloID , NULL ,  receptorDePackages , (void*) &socketNuevaConexion) < 0)
		{
			perror("No se pudo crear el hilo receptorDePackages");
			exitLFS(1);
		}

		unaMemoria->socket = socketNuevaConexion;
		list_add(listaMemorias,unaMemoria);
		//printf("Se asigno una memoria\n");
		log_info(logger,"Se asigno una memoria");
	}

	if (socketNuevaConexion < 0)
	{
		perror("accept failed");
		exitLFS(1);
	}

	return 0;
}

void *receptorDePackages(void *unSocketCliente)
{
	int socketCliente = *(int*)unSocketCliente;
	//int socketCliente = (int)unSocketCliente;
	t_PaqueteDeDatos *package;
	//int i=0;

	while(1)//i < 10)//1)
	{
		package = recibirPaquete(socketCliente); //MODIFICAR LA FUNCION PARA Q DEVUELVA NULL (SE DESCONECTO)
		//printf("fd: %d\n",fd); ///
		if(package->ID == 0 )
		{
			log_info(logger,"Se perdio conexion con la memoria en socket %d\n",socketCliente);
			close(socketCliente);
			break;
		}
		else if(package->ID >= 1 && package->ID <=25)
		{
			//printf("\npackage->ID: %d\n",package->ID);
			realizarProtocoloDelPackage(package, socketCliente); //POSIBLE, PASAR EL SOCKET DE DONDE VIENE EL PAQUETE PARA DEVOLVER RESULTADO
			//sleep(2);
		}

		//i++;
	}
	return 0;
}

void crearListasGenerales()
{
	listaMemorias = list_create();
	memTable = list_create();
}

void destruirListasGenerales(void)
{
	//eliminarListaMemorias(listaMemorias);

	//eliminarListaMemTable();
}

void freeT_Registro(t_Registro *unRegistro)
{
	free(unRegistro->VALUE);
	free(unRegistro);
}

void freeT_ArchivoTemp(t_ArchivoTemp *unArchivoTemp)
{
	free(unArchivoTemp->nombre);
	free(unArchivoTemp);
}

void freeT_Tabla(t_Tabla *unaTabla)
{
	//MATAR EL RESPECTIVO HILO COMPACTADOR  unaTabla->hiloIDCompactador
	//pthread_kill(unaTabla->hiloIDCompactador,SIGUSR1); //????
	pthread_mutex_destroy(&unaTabla->noBloqueado);
	pthread_cancel(unaTabla->hiloIDCompactador);


	free(unaTabla->nombreTabla);
	freeListaDeRegistros(unaTabla->registros);
	freeListaDeTemporales(unaTabla->temporales);

	free(unaTabla);
}

void realiarRetardo(int cantSegundos)
{
	sleep(cantSegundos);
}


//####################################
//compactador.c
/*
char *getNombreArchivoTEMP(t_Tabla *unaTabla)
{
	int numeroDUMP = cuantosArchivosTempHayEn(unaTabla->temporales);

	printf("nombre del archivo TMP: dump%d.tmp\n",numeroDUMP);///
	return string_from_format("dump%d.tmp",numeroDUMP);
}*/
/*
void persistirRegistrarDUMP(t_Tabla *unaTabla,char *laTablaDUMPEADA)
{
	if (strcmp(laTablaDUMPEADA,"sinRegistros") != 0)
	{
		int cantBytesParaGuardar = strlen(laTablaDUMPEADA);
		if(hayEspacioDisponible(cantBytesParaGuardar))
		{
			char *nombreArchivoTEMP = getNombreArchivoTEMP(unaTabla);
			//char *nombreAbsoluto = string_from_format("%s%s%s%s",configLFS.puntoMontaje,dirTables,nombreArchivoTEMP);
			char *unPath = string_from_format("%s/%s",unaTabla->nombreTabla,nombreArchivoTEMP);

			crearArchivoEnFS(unPath);

			char *r = guardarDatosEnArchivoEnFS(unPath,laTablaDUMPEADA);
			free(unPath);
			free(r);
			agregarArchivoTempALista(unaTabla->temporales,nombreArchivoTEMP,0);
			char *unBuffer = string_from_format("Tabla %s DUMPeada en %s",unaTabla->nombreTabla,nombreArchivoTEMP);
			log_info(logger,unBuffer);

			free(unBuffer);
			free(nombreArchivoTEMP);
		}
		else
		{
			char *unBuffer = string_from_format("No se puede realizar DUMP con la tabla %s: Espacio insuficiente",unaTabla->nombreTabla);
			log_info(logger,unBuffer);

			free(unBuffer);
		}
	}
}*/
