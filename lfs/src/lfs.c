#include "lfs.h"
//#include "fs.h"

int socketLFS; //SOCKET ESCUCHANDO, SERVIDOR

int main(void) {

	leerConfigLFS();
	mostrarValoresDeConfig();

	crearListasGenerales();


	iniciarFileSystemLISSANDRA();

	/*char * tabla = string_from_format("tabla1");
	char *r = existeTablaEnFS(tabla);
	printf("\nr: %s\n",r);
	free(r);
	free(tabla);*/

//############################
/*	t_CREATE *unCREATE = definirT_CREATE("TablaX","SC",2,300);

	printf("\nQuery:\n	CREATE [%s] [%s] [%d] [%d]\n",unCREATE->nombreTabla,unCREATE->tipoConsistencia,unCREATE->nParticiones,unCREATE->tiempoCompactacion);

	r = crearTablaEnFS(unCREATE);
	printf("\nr: %s\n",r);
	mostrarBitsDeBloques(8);
	freeT_CREATE(unCREATE);
*/

/*
	r = eliminarArchivoEnFS("TablaA/23.bin");
	printf("\nr: %s\n",r);
	free(r);
	mostrarBitsDeBloques(8);
*/
	/*char *todoElArchivo = string_from_format("%s","12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901");
	r = guardarDatosEnArchivoEnFS("TablaA/29.bin",todoElArchivo);
	printf("\nr: %s\n",r);
	free(r);
	//mostrarBitsDeBloques(8);
*/

	/*char *unPath = string_from_format("%s","TablaA/20.bin");
	r = obtenerDatosDeArchivoEnFS(unPath);
	printf("\nr: [%s]\n",r);
	free(r);
	free(unPath);*/

/*
	t_MetadataTabla *unMetaTabla;
	unMetaTabla = obtenerMetadataTabla("TablaFULL");
	printf("\nConsistencia: %s\nParticiones: %d\nTiempoCompactavion: %d\n",unMetaTabla->Consistency,unMetaTabla->Partitions,unMetaTabla->Compaction_Time);
	freeT_MetadataTabla(unMetaTabla);

	unMetaTabla = obtenerMetadataTabla("TablaX");
	printf("\nConsistencia: [%s]\nParticiones: %d\nTiempoCompactavion: %d\n",unMetaTabla->Consistency,unMetaTabla->Partitions,unMetaTabla->Compaction_Time);
	freeT_MetadataTabla(unMetaTabla);


	unMetaTabla = obtenerMetadataTabla("TablaA");
	printf("\nConsistencia: %s\nParticiones: %d\nTiempoCompactavion: %d\n",unMetaTabla->Consistency,unMetaTabla->Partitions,unMetaTabla->Compaction_Time);
	freeT_MetadataTabla(unMetaTabla);
*/

	iniciarEscucha();
	pthread_t hiloIDAsignador;
	pthread_create( &hiloIDAsignador , NULL ,  asignadorLISSANDRA , NULL);


	printf("CONSOLA...\n");
	consolaAPI();

	destruirListasGenerales();

	exitLFS(0);

	return 0; //T_SUCCESS;
}

void leerConfigLFS()
{
	archivoConfig = config_create("../../lfs.conf");
	logger = log_create("lfs.log", "lfs", 1,LOG_LEVEL_INFO);

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

	    operacion = string_split(linea," ");
	    free(linea); //ES NECESARIO
	    cantArgumentos = longitudArrayDePunteros(operacion) - 1; //XQ LA OPERACION CUENTA
	    string_to_upper(operacion[0]);

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
	    		log_info(logger,"Operacion SELECT");
	    		printf("Parametros: [%s][%s]\n",operacion[1],operacion[2]);
	    		//IMPRIMO POR PANTALLA EL RESULTADO DE LA OPERACION
	    	}
	    	else
	    		printf("Falta, o hay exceso, de parametros de SELECT\n");

	    }
	    else if(!strcmp(operacion[0], "INSERT")) //###
	    {
	    	if(cantArgumentos == 3)
	    	{
	    		log_info(logger,"Operacion INSERT");
	    		printf("Parametros: [%s][%s][%s]\n",operacion[1],operacion[2],operacion[3]);

	    		//definir t_INSERT, y luego el parametro timestamp = 0
	    		//IMPRIMO POR PANTALLA EL RESULTADO DE LA OPERACION
	    	}
	    	else if(cantArgumentos == 4)
	    	{
	    		log_info(logger,"Operacion INSERT");
	    		printf("Parametros: [%s][%s][%s][%s]\n",operacion[1],operacion[2],operacion[3],operacion[4]);
	    		//IMPRIMO POR PANTALLA EL RESULTADO DE LA OPERACION
	    	}
	    	else
	    		printf("Falta, o hay exceso, de paramentros de INSERT\n");

	    }
	    else if(!strcmp(operacion[0], "CREATE")) //###
	    {
	    	if(cantArgumentos == 4)
	    	{
	    		log_info(logger,"Operacion CREATE");
	    		printf("Parametros: [%s][%s][%s][%s]\n",operacion[1],operacion[2],operacion[3],operacion[4]);
	    		//IMPRIMO POR PANTALLA EL RESULTADO DE LA OPERACION
	    		/*
	    		t_CREATE *unCREATE = definirT_CREATE(operacion[1],operacion[2],atoi(operacion[3]),atoi(operacion[4]));
	    		char *respuesta = realizarCREATE(unCREATE);
	    		printf("Respuesta: %s\n",respuesta);
	    		free(respuesta);
	    		*/
	    	}
	    	else
	    		printf("Falta, o hay exceso, de parametros de CREATE\n");
	    }
	    else if(!strcmp(operacion[0], "DESCRIBE")) //###
	    {
	    	//EL UNICO ARGUMENTO PUEDE SER OPCIONAL
	    	if(cantArgumentos == 0)
	    	{
	    		log_info(logger,"Operacion DESCRIBE");
	    		printf("Parametros: [sin Parametros]\n");
	    		//IMPRIMO POR PANTALLA EL RESULTADO DE LA OPERACION (TODAS LAS TABLAS)
	    	}
	    	else if(cantArgumentos == 1)
	    	{
	    		log_info(logger,"Operacion DESCRIBE");
	    		printf("Parametros: [%s]\n",operacion[1]);
	    		//IMPRIMO POR PANTALLA EL RESULTADO DE LA OPERACION (TABLA ESPECIFICA)
	    	}
	    	else
	    		printf("Exceso de parametros de DESCRIBE\n");
	    }
	    else if(!strcmp(operacion[0], "DROP")) //###
	    {
	    	if(cantArgumentos == 1)
	    	{
	    		log_info(logger,"Operacion DROP");
	    		printf("[%s]\n",operacion[1]);
	    	}
	    	else
	    		printf("Falta, o hay exceso, de parametros de DROP\n");
	    }
	    else
	    {
	    	log_info(logger,"Operacion no valida por la API");
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
				printf("\npackage->ID: %d\n",package->ID); ///MODIFICAR EL ORDEN EN LA FUNCION DE HANDSHAKE
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

	if(packageRecibido->ID == 13) //13: SELECT, osea q DATOS es una structura t_SELECT
	{
		t_SELECT *unSELECT;

		unSELECT = deserializarT_SELECT(packageRecibido->Datos);
		printf("Query recibido: SELECT [%s] [%d]",unSELECT->nombreTabla,unSELECT->KEY);


		//char *Respuesta = realizarSELECT(unSELECT->nombreTabla,unSELECT->KEY);
		char* Respuesta = string_from_format("asd123QWE");
		enviarRespuesta(socketEmisor,14,Respuesta); //14: RESPUESTA DE UN PROTOCOLO = 13


		free(Respuesta);
		freeT_SELECT(unSELECT); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	if(packageRecibido->ID == 15) //15: INSERT, osea q DATOS es una structura t_INSERT
	{
		t_INSERT *unINSERT;

		unINSERT = deserializarT_INSERT(packageRecibido->Datos);
		printf("Query recibido: INSERT [%s] [%d] [%s] [%d]",unINSERT->nombreTabla,unINSERT->KEY,unINSERT->Value,unINSERT->timeStamp);


		char* Respuesta = string_from_format("insert123"); //
		//char* Respuesta = realizarINSERT(unINSERT,socketEmisor);  ///TESTEAR
		enviarRespuesta(socketEmisor,16,Respuesta); //16: RESPUESTA DE UN PROTOCOLO = 15


		free(Respuesta);
		freeT_INSERT(unINSERT); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	if(packageRecibido->ID == 17) //17: CREATE, osea q DATOS es una structura t_CREATE
	{
		t_CREATE *unCREATE;

		unCREATE = deserializarT_CREATE(packageRecibido->Datos);
		printf("Query recibido: CREATE [%s] [%s] [%d] [%d]",unCREATE->nombreTabla,unCREATE->tipoConsistencia,unCREATE->nParticiones,unCREATE->tiempoCompactacion);


		//char *Respuesta = realizarCREATE(unCREATE);
		char* Respuesta = string_from_format("TABLA_CREADA");
		enviarRespuesta(socketEmisor,18,Respuesta); //18: RESPUESTA DE UN PROTOCOLO = 17


		free(Respuesta);
		freeT_CREATE(unCREATE); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	if(packageRecibido->ID == 19) //19: GUARDAR DATOS, osea q DATOS es una structura t_DESCRIBE
	{
		t_DESCRIBE *unDESCRIBE;

		unDESCRIBE = deserializarT_DESCRIBE(packageRecibido->Datos);
		printf("Query recibido: DESCRIBE [%s]",unDESCRIBE->nombreTabla);


		//char *Respuesta = realizarDESCRIBE();
		char* Respuesta = string_from_format("FIN_DESCRIBE");
		enviarRespuesta(socketEmisor,20,Respuesta); //20: RESPUESTA DE UN PROTOCOLO = 19


		free(Respuesta);
		freeT_DESCRIBE(unDESCRIBE); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	if(packageRecibido->ID == 21) //11:  ELIMINAR UN ARCHIVO, osea q DATOS es una structura t_DROP
	{
		t_DROP *unDROP;

		unDROP = deserializarT_DROP(packageRecibido->Datos);
		printf("Query recibido: DESCRIBE [%s]",unDROP->nombreTabla);


		//char *Respuesta = realizarDESCRIBE();
		char* Respuesta = string_from_format("SUCCESSFUL_DROP");
		enviarRespuesta(socketEmisor,22,Respuesta); //22: RESPUESTA DE UN PROTOCOLO = 21


		free(Respuesta);
		freeT_DROP(unDROP); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	freePackage(packageRecibido);

}

//SE VA A TENER Q USAR MUTEX
char *realizarINSERT(t_INSERT *unINSERT, int socketMemoria)
{
	//char* respuesta = string_from_format("insert123");

	char *respuesta = existeTablaEnFS(unINSERT->nombreTabla);

	if(strcmp(respuesta,"YA_EXISTE_TABLA") == 0)
	{
		t_Tabla *tablaEncontrada = existeEnMemtable(unINSERT->nombreTabla);

		if(tablaEncontrada!= NULL)
		{
			t_Registro *nuevoRegistro = malloc(sizeof(t_Registro));

			if(unINSERT->timeStamp == 0)
				//nuevoRegistro.TIMESTAMP = 0.000001 * (unsigned)time(NULL);
				nuevoRegistro->TIMESTAMP = (unsigned)time(NULL);
			else
				nuevoRegistro->TIMESTAMP = unINSERT->timeStamp;

			nuevoRegistro->KEY = unINSERT->KEY;
			nuevoRegistro->VALUE = string_from_format("%s",unINSERT->Value);

			list_add(tablaEncontrada->registros,nuevoRegistro);

			respuesta = string_from_format("SUCCESSFUL_INSERT");
		}
		else
		{
			t_Tabla *nuevaTabla = malloc(sizeof(t_Tabla));

			nuevaTabla->nombreTabla = string_from_format("%s",unINSERT->nombreTabla);
			nuevaTabla->registros = list_create();
			nuevaTabla->temporales = list_create();

			list_add(memTable,nuevaTabla);

			t_Registro *nuevoRegistro = malloc(sizeof(t_Registro));

			if(unINSERT->timeStamp == 0)
				//nuevoRegistro.TIMESTAMP = 0.000001 * (unsigned)time(NULL);
				nuevoRegistro->TIMESTAMP = (unsigned)time(NULL);
			else
				nuevoRegistro->TIMESTAMP = unINSERT->timeStamp;

			nuevoRegistro->KEY = unINSERT->KEY;
			nuevoRegistro->VALUE = string_from_format("%s",unINSERT->Value);

			list_add(nuevaTabla->registros,nuevoRegistro);

			respuesta = string_from_format("SUCCESSFUL_INSERT");
		}
	}

	return respuesta;
}

t_Tabla *existeEnMemtable(char *nombreTabla)
{
	bool existeTabla(void * element)
	{
		return (strcmp(((t_Tabla*)element)->nombreTabla,nombreTabla) == 0);
	}

	t_Tabla *unaTabla = (t_Tabla *)list_find(memTable,existeTabla);

	return unaTabla;
}

char *realizarCREATE(t_CREATE *unCREATE)
{
	return crearTablaEnFS(unCREATE);
}

void realizarHandshakeAMemoria(t_log *logger,int socketCliente, t_PaqueteDeDatos *packageRecibido,char* msjEnviado)
{

	packageRecibido->Datos[packageRecibido->longDatos] = '\0'; //PAQUETE RECIBIDO

	printf("Recibiendo... ID: %d\n",packageRecibido->ID); ///

	if(packageRecibido->ID != 11) //NO ENTRA MAS POR ACA, se verifica anteriormente antes de entrar a esta funcion
	{
		printf("el ID de protocolo no es el esperado, cerrando conexion");
		close(socketCliente);
		//exitLFS(1);
	}
	else
	{
		int protocoloID = 12; // 12: RESPUESTA AL HANDSHAKE

		printf("Enviando tamanio de pagina: %s...\n\n",msjEnviado);
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

void enviarRespuesta(int socketReceptor, int protocoloID, char *respuesta)
{
	char* stringSerializado;
	t_UnString *unString = definirT_UnString(respuesta);


	printf("\nRespuesta definida:\n	longString: %d\n	String: %s\n",unString->longString,unString->String);///

	stringSerializado = serializarT_UnString(unString);

	//printf("\nstrlen(pathSerializado): %d\npathSerializado: %s\n",strlen(pathSerializado),pathSerializado);

	//printf("empaquetarEnviarMensaje():\n"); ///

	//ESTO VARIA SEGUN LA STRUCTURA Q PIENSO MANDAR, hay q hacerlo a mano
	int tamanioStructSerializado = sizeof(uint32_t) + unString->longString;

	empaquetarEnviarMensaje(socketReceptor,protocoloID,tamanioStructSerializado,stringSerializado);

	free(stringSerializado);
	freeT_UnString(unString);

	printf("-----------------------------------------------\n");
}

void *asignadorLISSANDRA(void *arg)
{
	int socketNuevaConexion;

	while( (socketNuevaConexion = aceptarConexiones(socketLFS)) )
	{
		t_Memoria *unaMemoria = malloc(sizeof(t_Memoria));

		if( pthread_create( &unaMemoria->hiloID , NULL ,  receptorDePackages , (void*) &socketNuevaConexion) < 0)
		{
			perror("No se pudo crear el hilo");
			exitLFS(1);
		}

		unaMemoria->socket = socketNuevaConexion;
		list_add(listaMemorias,unaMemoria);
		printf("Se asigno una memoria\n");
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
			printf("Se perdio conexion con la memoria en socket %d\n",socketCliente);
			close(socketCliente);
			break;
		}
		else if(package->ID >= 1 && package->ID <=25)
		{
			printf("\npackage->ID: %d\n",package->ID);
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
