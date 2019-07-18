#include "fs.h"

#define dirTables "/Tables/"  //"/Archivos/"
#define dirBloques "/Bloques/"

struct addrinfo *server_info;

int socketMDJ; //servidor

char *todoElArchivo; // = string_new();

/*
int main(void) {

	todoElArchivo = string_new();

	leerConfigMDJ();

	mostrarValoresDeConfig();

	iniciarFileSystemFIFA();

	iniciarEscucha();

	realizarMultiplexacion(socketMDJ);


	exitGracefully(EXIT_SUCCESS);
}
*/




void exitGracefully(int return_nr)
{
	//config_destroy(archivoConfig);
	//config_destroy(archivoMetadata);
	//log_destroy(logger);

	//close(socketMDJ);
	exit(return_nr);
}
/*
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
	while(i < 15) //1)
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
			exitGracefully(1);
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
				printf("package->ID: %d\n",package->ID); ///MODIFICAR EL ORDEN EN LA FUNCION DE HANDSHAKE
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
*/


//##########################################
/*
void realizarProtocoloDelPackage(t_PaqueteDeDatos *packageRecibido, int socketEmisor)
{
	//
	if(packageRecibido->ID == 1) //1; HANDSHAKE. 2: ES LA RESPUESTA
	{
		//
		log_info(logger,"Realizando respectivo HandShake");
		realizarHandshakeAlCliente(logger,socketEmisor,packageRecibido,"Hola Diego!","Aca El 10");
	}

	if(packageRecibido->ID == 3) //3:  VALIDAR UN ARCHIVO, osea q DATOS es una structura t_ValidarPath
	{
		t_ValidarPath *elPath;

		elPath = deserializarAT_ValidarPath(packageRecibido->Datos);
		printf("t_ValidarPath recibido deserializado:\nlongPath: %d\nPath: %s#\n",elPath->longPath,elPath->Path);

		char *Respuesta = validarArchivoEnFS(elPath->Path);
		enviarRespuesta(socketEmisor,4,Respuesta); //4: RESPUESTA DE UN PROTOCOLO = 3

		free(Respuesta);
		freeT_ValidarPath(elPath); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	if(packageRecibido->ID == 5) //5: CREAR UN ARCHIVO, osea q DATOS es una structura t_CrearArchivo
	{
		t_CrearArchivo *structDeserializado;

		structDeserializado = deserializarAT_CrearArchivo(packageRecibido->Datos);
		printf("t_CrearArchivo recibido deserializado:\nNcantidadBytes: %d\nlongPath: %d\nPath: %s#\n",structDeserializado->NcantidadBytes,structDeserializado->longPath,structDeserializado->Path);

		char *Respuesta = validarArchivoEnFS(structDeserializado->Path);
		//printf("\nJJJJJ RESPUESTA: %s\n",Respuesta);

		if(strcmp(Respuesta,"NO_EXISTE_ARCHIVO") == 0)
		{
			if(hayEspacioDisponible(structDeserializado->NcantidadBytes))
			{
				crearArchivoEnFS(structDeserializado);

				free(Respuesta);
				Respuesta = string_from_format("ARCHIVO_CREADO");
			}
			else
			{
				free(Respuesta);
				Respuesta = string_from_format("NO_HAY_ESPACIO");
			}
		}

		enviarRespuesta(socketEmisor,6,Respuesta); //6: RESPUESTA DE UN PROTOCOLO = 5
		free(Respuesta);
		freeT_CrearArchivo(structDeserializado);
	}

	if(packageRecibido->ID == 7) //7: OBTENER DATOS, osea q DATOS es una structura t_ObtenerDatos
	{
		t_ObtenerDatos *structDeserializado;

		structDeserializado = deserializarAT_ObtenerDatos(packageRecibido->Datos);
		printf("t_ObtenerDatos recibido deserializado:\nOffset: %d\nSize: %d\nlongPath: %d\nPath: %s\n",structDeserializado->Offset,structDeserializado->Size,structDeserializado->longPath,structDeserializado->Path);

		char *Respuesta = validarArchivoEnFS(structDeserializado->Path);
		//printf("\n#JJJJJ### RESPUESTA: %s\n",Respuesta);
		if(strcmp(Respuesta,"EXISTE_ARCHIVO") == 0)
		{
			free(Respuesta);

			char *nombreAmsoluto = string_from_format("%s/Archivos/%s",configLFS.puntoMontaje,structDeserializado->Path);
			char *buffer = getTodoElArchivoBufferDe(nombreAmsoluto);
			//printf("BUFFERENTERO:\n[%s]\n",buffer);
			int offset = structDeserializado->Offset; //INICIAR EN = 0;
			int fin = structDeserializado->Size;
			int tope = cuantosBloquesNecesitoPara(strlen(buffer),fin);
			int i;
			//BUFFER ARRANCA DE 0, OJO PROBLEMA CON OFFSET  (POSIBLE OPCION,  offset -1)

			int resto = strlen(buffer) % structDeserializado->Size;

			for(i=0; i < tope ;i++)
			{
				if(i == tope -1 && resto != 0)
				{
					fin = resto;
				}

				Respuesta = string_substring(buffer,offset,fin);
				enviarRespuesta(socketEmisor,8,Respuesta); //8: RESPUESTA DE UN PROTOCOLO = 7
				free(Respuesta);

				offset += fin;
			}

			Respuesta = string_from_format("FIN_DE_ARCHIVO");
			enviarRespuesta(socketEmisor,8,Respuesta); //8: RESPUESTA DE UN PROTOCOLO = 7
			free(Respuesta);

			free(buffer);
		}
		else
		{
			enviarRespuesta(socketEmisor,8,Respuesta); //8: RESPUESTA DE UN PROTOCOLO = 7
			free(Respuesta);
		}
		freeT_ObtenerDatos(structDeserializado);
	}

	if(packageRecibido->ID == 9) //9: GUARDAR DATOS, osea q DATOS es una structura t_GuardarDatos
	{
		t_GuardarDatos *structDeserializado;

		structDeserializado = deserializarAT_GuardarDatos(packageRecibido->Datos);
		printf("t_GuardarDatos recibido deserializado:\nOffset: %d\nSize: %d\nlongPath: %d\nlongBuffer: %d\nPath: %s\nBuffer: %s\n",structDeserializado->Offset,structDeserializado->Size,structDeserializado->longPath,structDeserializado->longBuffer,structDeserializado->Path,structDeserializado->Buffer);


		if(strcmp(structDeserializado->Buffer,"FIN_ARCHIVO")!=0)
		{
			string_append(&todoElArchivo,structDeserializado->Buffer);
		}
		else
		{
			char *Respuesta = validarArchivoEnFS(structDeserializado->Path);

			if(strcmp(Respuesta,"EXISTE_ARCHIVO") == 0)
			{
				int tamanioArchivo = strlen(todoElArchivo);
				char *nombreAbsoluto = string_from_format("%s%s%s",configLFS.puntoMontaje,dirTables,structDeserializado->Path);

				liberarBloquesDe(nombreAbsoluto);
				bool hayEspacio = hayEspacioDisponible(tamanioArchivo);
				reAsignarBloquesDe(nombreAbsoluto);

				free(Respuesta);

				if(hayEspacio)
				{
					mostrarBitsDeBloques(8);
					borrarArchivoEnFS(structDeserializado->Path);
					mostrarBitsDeBloques(8);

					//### DE CREAR ARCHIVO #########################
					char *bloques = asignarBloques(tamanioArchivo);
					char *bufferLinea = string_from_format("TAMANIO=%d\nBLOQUES=%s\n",tamanioArchivo,bloques);

					escribirLineaEn(nombreAbsoluto,bufferLinea);
					guardarBufferArchivoEn(nombreAbsoluto,todoElArchivo,tamanioArchivo);
					mostrarBitsDeBloques(8);

					free(bloques);
					free(bufferLinea);
					//##############################

					Respuesta = string_from_format("ARCHIVO_GUARDADO");
				}
				else
				{
					Respuesta = string_from_format("NO_HAY_ESPACIO");
				}

				//printf("\ntamanio FILE: %d\n", tamanioArchivo);///

				enviarRespuesta(socketEmisor,10,Respuesta); //10: RESPUESTA DE UN PROTOCOLO = 9

				free(Respuesta);
				free(nombreAbsoluto);
			}
			else
			{
				enviarRespuesta(socketEmisor,10,Respuesta); //10: RESPUESTA DE UN PROTOCOLO = 9
				free(Respuesta);
			}

			//printf("\nArchivoRecibido:\n[%s]\n",todoElArchivo);///
			free(todoElArchivo);

			todoElArchivo = string_new();
		}


		freeT_GuardarDatos(structDeserializado);
	}

	if(packageRecibido->ID == 11) //11:  ELIMINAR UN ARCHIVO, osea q DATOS es una structura t_ValidarPath
	{
		t_ValidarPath *elPath;

		elPath = deserializarAT_ValidarPath(packageRecibido->Datos);
		printf("t_ValidarPath recibido deserializado (delete file):\nlongPath: %d\nPath: %s\n",elPath->longPath,elPath->Path); ///

		char *Respuesta = validarArchivoEnFS(elPath->Path);
		//printf("\n#JJJJJ### RESPUESTA: %s\n",Respuesta);
		if(strcmp(Respuesta,"EXISTE_ARCHIVO") == 0)
		{
			free(Respuesta);

			borrarArchivoEnFS(elPath->Path);
			Respuesta = string_from_format("ARCHIVO_BORRADO");
		}

		enviarRespuesta(socketEmisor,12,Respuesta); //12: RESPUESTA DE UN PROTOCOLO = 11
		free(Respuesta);

		freeT_ValidarPath(elPath); //HAY Q LIBERAR EL PAYLOAD SEGUN EL PROTOCOLO, particularmente en cada caso.
	}

	freePackage(packageRecibido);

}*/
/**/
void iniciarFileSystemLISSANDRA(void)
{
	leerConfigMetadata(1);

	//discoLogico = malloc(sizeof(char) * metadata.tamanioBloque * metadata.cantidadBloques);

	obtenerBitArrayPersistido();
	//crearBitArrayDeCero();
}

void leerConfigMetadata(int showMetadata)
{
	char *nombreAbsoluto = string_from_format("%s%s",configLFS.puntoMontaje,"/Metadata/Metadata.bin");
	//printf("nombreAbsoluto Metadata: %s\n",nombreAbsoluto); ///


	archivoMetadata = config_create(nombreAbsoluto);
	free(nombreAbsoluto);

	log_info(logger, "Leyendo archivo Metadata.bin...");


	if (config_has_property(archivoMetadata, "TAMANIO_BLOQUES")) {
		metadata.tamanioBloque = config_get_int_value(archivoMetadata,"TAMANIO_BLOQUES");
	} else {
		log_error(logger,"No se encontro la key TAMANIO_BLOQUES en el archivo Metadata.bin");
		exit(EXIT_FAILURE);
	}

	if (config_has_property(archivoMetadata, "CANTIDAD_BLOQUES")) {
		metadata.cantidadBloques = config_get_int_value(archivoMetadata,"CANTIDAD_BLOQUES");
	} else {
		log_error(logger,"No se encontro la key CANTIDAD_BLOQUES en el archivo Metadata.bin");
		exit(EXIT_FAILURE);
	}

	if (config_has_property(archivoMetadata, "MAGIC_NUMBER")) {
		metadata.numeroMagico = config_get_string_value(archivoMetadata,"MAGIC_NUMBER");
	} else {
		log_error(logger,"No se encontro la key MAGIC_NUMBER en el archivo Metadata.bin");
		exit(EXIT_FAILURE);
	}


	if(showMetadata)
	{
		printf("TAMANIO_BLOQUES: %d\n",metadata.tamanioBloque);
		printf("CANTIDAD_BLOQUES: %d\n",metadata.cantidadBloques);
		printf("MAGIC_NUMBER: %s\n\n",metadata.numeroMagico);
	}

}



char *validarArchivoEnFS(char *pathArchivo)
{
	FILE *unArchivo;
	char *r;
	char *nombreAbsoluto = string_new();

	///nombreAbsoluto = string_from_format("%s%s%s",configMDJ.puntoMontaje,"/Archivo/",pathArchivo);
	// USAR ALGO CON CONCATENACION
	//nombreAbsoluto = string_from_format("%s%s%s",configMDJ.puntoMontaje,directorio,pathArchivo);
	string_append(&nombreAbsoluto,configLFS.puntoMontaje);
	string_append(&nombreAbsoluto,dirTables);
	string_append(&nombreAbsoluto,pathArchivo);

	//printf("nombreAbsoluto: %s",nombreAbsoluto); ///
	unArchivo = fopen(nombreAbsoluto,"r");

	//r = (unArchivo == NULL)?"NO_EXISTE_ARCHIVO":"EXISTE_ARCHIVO";
	if(unArchivo == NULL)
	{
		r = string_from_format("NO_EXISTE_ARCHIVO");
	}
	else
	{
		r = string_from_format("EXISTE_ARCHIVO");
		fclose(unArchivo);
	}

	//fclose(unArchivo);  //CERARA ALGO Q NO SE PUDO ABRIR, segmetation fault
	free(nombreAbsoluto);

	return r;
}

void crearArchivoEnFS(char *unPathArchivo)
{
	char *nombreAbsoluto;
	char *directorios;
	char *copiaNA;
	char *mkdirSystem;
	int NcantidadBytes = 1;
	//nombreAbsoluto = string_from_format("%s%s%s",configMDJ.puntoMontaje,"/Archivos/",nuevoArchivo->Path);
	nombreAbsoluto = string_from_format("%s%s%s",configLFS.puntoMontaje,dirTables,unPathArchivo);
	copiaNA = string_from_format("%s%s%s",configLFS.puntoMontaje,dirTables,unPathArchivo);

	directorios = (char *) dirname(copiaNA); // NO HACER FREE(), a los q devuelve
	mkdirSystem = string_from_format("mkdir -p %s",directorios);

	//printf("\nDIR: %s\n",directorios);///
	system(mkdirSystem);

	char *bloques = asignarBloques(NcantidadBytes);
	char *bufferLinea = string_from_format("TAMANIO=%d\nBLOQUES=%s\n",NcantidadBytes,bloques);

	escribirLineaEn(nombreAbsoluto,bufferLinea);

	//char *bufferTodoElArchivo = string_repeat("/n",nuevoArchivo->NcantidadBytes);
	char *bufferTodoElArchivo = string_new();
	int i;

	for(i=0; i < NcantidadBytes ;i++)
	{
		string_append(&bufferTodoElArchivo, "\n");
	}

	guardarBufferArchivoEn(nombreAbsoluto,bufferTodoElArchivo,NcantidadBytes);

	free(nombreAbsoluto);
	free(copiaNA);
	free(mkdirSystem);
	free(bloques);
	free(bufferLinea);
	free(bufferTodoElArchivo);
	//config_destroy(metadataArchivoNuevo);
	//free(directorios);
}

void borrarArchivoEnFS(char *pathArchivo)
{
	char *nombreAbsoluto = string_from_format("%s%s%s",configLFS.puntoMontaje,dirTables,pathArchivo);

	liberarBloquesDe(nombreAbsoluto);
	remove(nombreAbsoluto); // LLAMA A UNLINK() PARA ARCHIVOS Y RMDIR PARA DIRECTORIOS
	//unlink(nombreAbsoluto);

	free(nombreAbsoluto);
}

//ANTES DE GUARDAR TIENE Q ESTAR DEFINIDO LOS BLOQUES ASIGNADOS PARA EL BUFFER, (pisa tod0 lo q estaba en cada bloque)
//GUARDA A NIVEL BLOQUES
void guardarBufferArchivoEn(char *pathAbsoluto,char* bufferArchivo,int longBuffer)
{
	int i = 0;
	char **bufferEnBloques;
	int tope;
	char *pathBloque;

	t_MetadataUnArchivo *unaMetadataArchivo = leerMetadataDeArchivo(pathAbsoluto,0);

	tope = cuantosBloquesNecesitoPara(longBuffer,metadata.tamanioBloque);
	bufferEnBloques = splitSegunTamanioCorte(bufferArchivo,longBuffer,metadata.tamanioBloque);

	for(i=0; i < tope ;i++)
	{
		pathBloque = string_from_format("%s%s%s.bin",configLFS.puntoMontaje,dirBloques,unaMetadataArchivo->Bloques[i]);
		escribirLineaEn(pathBloque,bufferEnBloques[i]);
	}

	freeArrayDePunteros(bufferEnBloques);
	//free(unaMetadataArchivo);
	freeT_MetadataUnArchivo(unaMetadataArchivo);
}

//char *getBufferArchivoDe(char *pathAbsoluto)
char *getTodoElArchivoBufferDe(char *pathAbsoluto)
{
	char *bufferArchivo = string_new();
	char *pathArchivoBloque;
	char *bloqueLeido;
	int i, resto;
	int fin = metadata.tamanioBloque;  //"tamnioBloque"

	t_MetadataUnArchivo *unaMetadataArchivo = leerMetadataDeArchivo(pathAbsoluto,0);

	resto = unaMetadataArchivo->Tamanio % metadata.tamanioBloque;
	//printf("\nTMNio ARCHIVO: %d\nFIN: %d\nRESTO: %d\n",unaMetadataArchivo->Tamanio,fin,resto); ///

	for(i=0; unaMetadataArchivo->Bloques[i] != NULL;i++)
	{
		if((unaMetadataArchivo->Bloques[i +1] == NULL) && (resto) != 0) //la 2da sentencia es para el caso de q entre justo en n Bloques (sin frag. interna)
		{
			fin = resto;
			//printf("RE ASIGNO fin\n"); ///
		}

		pathArchivoBloque = string_from_format("%s/Bloques/%s.bin",configLFS.puntoMontaje,unaMetadataArchivo->Bloques[i]);
		bloqueLeido = leerBloqueDe(pathArchivoBloque,fin);

		string_append(&bufferArchivo,bloqueLeido);
		//printf("[%s]\n\n",bloqueLeido); ///

		free(pathArchivoBloque);
		free(bloqueLeido);
	}

	//free(unaMetadataArchivo);
	freeT_MetadataUnArchivo(unaMetadataArchivo);
	return bufferArchivo;
}
/*EL archivoPath DEBE SER EL ABSOLUTO */
void escribirLineaEn(char *archivoPath, char *linea)
{
	FILE *f;


	f = fopen(archivoPath,"w");

	if(f == NULL)
	{
		printf("Error al abrir el archivo, fopen()\n");
		exit(1);
	}

	//fprintf(f,linea);
	fputs(linea,f);

	fclose(f);
}

char *leerBloqueDe(char *pathArchivoBloque,int nBytes)
{
	FILE *f;
	char *bloqueNBytes = malloc((sizeof(char) * nBytes) +1);

	f = fopen(pathArchivoBloque,"rb");
	if (f == NULL)
	{
		printf("Error no existe el archivo bloque \n");
		exitGracefully(EXIT_FAILURE);
	}

	fread(bloqueNBytes,sizeof(char),nBytes,f);
	bloqueNBytes[nBytes] = '\0';

	return bloqueNBytes;
}

void obtenerBitArrayPersistido()
{
	char *nombreAbsoluto;
	FILE *archivoBitArray;
	int tamanioArchivoAMapear;
	int fueCreadoBitMap = 0;

	//t_bitarray *bitsDeBloques; //LO TENGO DECLARADO YA GLOBALMENTE

	nombreAbsoluto = string_from_format("%s%s",configLFS.puntoMontaje,"/Metadata/Bitmap.bin");


	archivoBitArray = fopen(nombreAbsoluto,"r");
	if(archivoBitArray)
	{	//EXISTE
		fseek(archivoBitArray, 0L, SEEK_END);
		tamanioArchivoAMapear = ftell(archivoBitArray);
		printf("\nTamanio del archivo: %d (bytes)\n",tamanioArchivoAMapear); ///
		fclose(archivoBitArray);
	}
	else
	{
		printf("No exite el archivo Bitmap.bin, fopen()\n");
		//exit(EXIT_FAILURE);
		//free(nombreAbsoluto);
		//exitLFS(1); //exitGracefully(EXIT_FAILURE);

		//int fdArchivoBitArray = open(nombreAbsoluto,O_CREAT , S_IROTH +S_IWOTH+S_IXOTH); //S_IROTH | S_IRGRP | S_IWGRP);
		//close(fdArchivoBitArray);

		//archivoBitArray = fopen(nombreAbsoluto,"w");
		//fclose(archivoBitArray);

		tamanioArchivoAMapear = metadata.cantidadBloques / 8;
		if (metadata.cantidadBloques % 8 !=0)
			tamanioArchivoAMapear++;
		fueCreadoBitMap = 1;

		char *unaLinea = string_new();
		int i;
		for(i=0; i < tamanioArchivoAMapear ;i++)
		{
			string_append(&unaLinea, "\n");
		}

		escribirLineaEn(nombreAbsoluto,unaLinea);

		printf("\nTamanio del archivo: %d (bytes)\n",tamanioArchivoAMapear); ///
	}

	//int fd = open(nombreAbsoluto, O_RDWR,S_IRWXO);
	int fd = open(nombreAbsoluto, O_RDWR);
	if (fd == -1)
	{
		perror("Error al abrir bitmap.bin, open()");
		//exit(EXIT_FAILURE);
		free(nombreAbsoluto);
		//exitGracefully(EXIT_FAILURE);
		exitLFS(1);

		//tamanioArchivoAMapear
	}

	//printf("EXISTE_ARCHIVO (bitmap.bin)\n");
	//char *array = (char *) mmap(0,tamanioArchivoAMapear,PROT_READ | PROT_WRITE,MAP_PRIVATE,(int) fd,(off_t) 0);
    void *starting_addr = malloc(tamanioArchivoAMapear);

	char *arrayMap = mmap(starting_addr, tamanioArchivoAMapear, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);

	if (arrayMap == MAP_FAILED)
	{
		perror("Error al abrir bitmap.bin, mmap()");
		//exit(EXIT_FAILURE);
		close(fd);
		free(starting_addr);
		free(nombreAbsoluto);
		exitLFS(EXIT_FAILURE);
	}
	//bitsDeBloques = (t_bitarray *) mmap(0, tamanioArchivoAMapear, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	//printf("\n%s",arrayMap);
	bitsDeBloques = bitarray_create_with_mode(arrayMap,tamanioArchivoAMapear,MSB_FIRST); //LSB_FIRST //MSB_FIRST

	//printf("\nBITARRAY: %sJ\ncant: %d\n",arrayMap,(int) bitarray_get_max_bit(bitsDeBloques));
	//printf("\nBITARRAY: %s\nSize: %d\n",bitsDeBloques->bitarray,bitsDeBloques->size);
	if(fueCreadoBitMap)
	{
		int i;

		for(i=0;i < metadata.cantidadBloques;i++)
		{
			setBitDeBloque(i,0);
		}
	}
	log_info(logger, "Leyendo archivo Bitmap.bin...");

	mostrarBitsDeBloques(8);///

	munmap (bitsDeBloques, tamanioArchivoAMapear);
	//munmap (arrayMap, tamanioArchivoAMapear);

	close(fd);

	free(starting_addr);
	free(nombreAbsoluto);
	//fclose(archivoBitArray);
	//return bitsDeBloques;

}//Debug Rulz~

/*
void mostrarBitsDeBloques(int nBloquesPorLinea)
{
	int i,j;

	printf("\nBITMAP.BIN DE BLOQUES\n");
	for(i=0;i < metadata.cantidadBloques;)
	{
		for(j=0;j < nBloquesPorLinea;j++)
		{
			printf("[%d]",bitarray_test_bit(bitsDeBloques,i));
			i++;
		}
		printf("\n");
	}
	printf("\n");
	//printf("\nPRIMER LIBRE: %d\n",getPrimerBloqueLibre());
	//printf("%d bytes se puede guardar: %d (bool)\n",2951,hayEspacioDisponible(2951));
}*/

void mostrarBitsDeBloques(int nBloquesPorLinea)
{
	int i;

	printf("\nBITMAP.BIN DE BLOQUES\n");
	for(i=0;i < metadata.cantidadBloques;i++)
	{
		printf("[%d]",bitarray_test_bit(bitsDeBloques,i));
		if ((i + 1) % nBloquesPorLinea == 0)
			printf("\n");
	}
	printf("\n\n");
}
void setBitDeBloque(int indexBloque,int nuevoValor)
{
	if(nuevoValor == 1)
		bitarray_set_bit(bitsDeBloques,indexBloque);

	if(nuevoValor == 0)
		bitarray_clean_bit(bitsDeBloques,indexBloque);
}

bool hayEspacioDisponible(int cantBytesParaGuardar)
{
	//CONTAR LOS BITS = 0
	int i,contBloquesVacio = 0;

	//printf("BITMAP.BIN DE BLOQUES\n");
	for(i=0;i < metadata.cantidadBloques;i++)
	{
		if(bitarray_test_bit(bitsDeBloques,i) == 0)
			contBloquesVacio++;
	}

	int totalBytesDisponibles = metadata.tamanioBloque * contBloquesVacio;

	return (cantBytesParaGuardar <= totalBytesDisponibles)? 1 : 0;

}
int getPrimerBloqueLibre(void)
{
	int i;

	for(i=0;i < metadata.cantidadBloques && bitarray_test_bit(bitsDeBloques,i) == 1;i++);
		//printf("[%d]= %d",i,bitarray_test_bit(bitsDeBloques,i));

	return (i >= metadata.cantidadBloques)? -1:i;
}

char *asignarBloques(int cantidadDeBytes)
{
	char *bloques; // = malloc(sizeof(char) +1);
	int i,tope,indiceBloque;

	//ctrcpy(bloques,"[");
	// [2]
	// [2,2]
	// [2,2,2]
	// [2,2,2,2]
	indiceBloque = getPrimerBloqueLibre();
	bloques = string_from_format("[%d",indiceBloque);
	setBitDeBloque(indiceBloque,1);

	tope = cuantosBloquesNecesitoPara(cantidadDeBytes,metadata.tamanioBloque);
	//printf("Tope: %d\n",tope); ///
	for(i=1; i < tope ;i++)
	{
		indiceBloque = getPrimerBloqueLibre();
		string_append_with_format(&bloques,",%d",indiceBloque);
		setBitDeBloque(indiceBloque,1);
	}

	string_append(&bloques, "]");

	return bloques;
}

void liberarBloquesDe(char *nombreAbsolutoArchivoMeta)
{
	t_MetadataUnArchivo *unaMetadataArchivo = leerMetadataDeArchivo(nombreAbsolutoArchivoMeta,0);
	int i;
	int iBloque;

	for(i=0;unaMetadataArchivo->Bloques[i] != NULL;i++)
	{
		iBloque = atoi(unaMetadataArchivo->Bloques[i]);

		setBitDeBloque(iBloque,0);
	}

	//free(unaMetadataArchivo);
	freeT_MetadataUnArchivo(unaMetadataArchivo);
}

void reAsignarBloquesDe(char *nombreAbsolutoArchivoMeta)
{
	t_MetadataUnArchivo *unaMetadataArchivo = leerMetadataDeArchivo(nombreAbsolutoArchivoMeta,0);
	int i;
	int iBloque;

	for(i=0;unaMetadataArchivo->Bloques[i] != NULL;i++)
	{
		iBloque = atoi(unaMetadataArchivo->Bloques[i]);

		setBitDeBloque(iBloque,1);
	}

	free(unaMetadataArchivo);
}

t_MetadataUnArchivo *leerMetadataDeArchivo(char *unNombreAbsoluto,int showLoLeido)
{
	t_MetadataUnArchivo *unaMetadataArchivo = malloc(sizeof(t_MetadataUnArchivo));

	//printf("nombreAbsoluto de archivo Metadata: %s\n",unNombreAbsoluto); ///

	metadataUnArchivo = config_create(unNombreAbsoluto);
	//free(unNombreAbsoluto);

	//log_info(logger, "Leyendo un archivo Metadata..."); ///


	if (config_has_property(metadataUnArchivo, "TAMANIO")) {
		unaMetadataArchivo->Tamanio = config_get_int_value(metadataUnArchivo,"TAMANIO");
	} else {
		log_error(logger,"No se encontro la key TAMANIO en la metadata del archivo solicitado");
		exit(EXIT_FAILURE);
	}

	if (config_has_property(metadataUnArchivo, "BLOQUES")) {
		unaMetadataArchivo->Bloques = config_get_array_value(metadataUnArchivo,"BLOQUES");
	} else {
		log_error(logger,"No se encontro la key BLOQUES en la metadata del archivo solicitado");
		exit(EXIT_FAILURE);
	}

	if(showLoLeido)
	{
		printf("TAMANIO: %d\n",unaMetadataArchivo->Tamanio);

		int i=0;
		printf("BLOQUES: %s",unaMetadataArchivo->Bloques[i]);
		for(i=1;unaMetadataArchivo->Bloques[i] != NULL;i++)
		{
			printf(", %s",unaMetadataArchivo->Bloques[i]);
		}

		printf("\n");
	}

	config_destroy(metadataUnArchivo);

	return unaMetadataArchivo;
}

void freeT_MetadataUnArchivo(t_MetadataUnArchivo *unStruct)
{
	int i;

	for(i=0;unStruct->Bloques[i] != NULL;i++)
	{
		free(unStruct->Bloques[i]);
	}
	free(unStruct->Bloques);  //nose si sera necesario, al hacer free de tod0 el struct
	free(unStruct);
}



//### OTRAS FUNCIONES ######################################
char **splitSegunTamanioCorte(char *s, int longS,int tamanioCorte)//, int bloques)//
{
	char **sSplit;
	int nBytes = longS; //strlen(s);
	int tope = cuantosBloquesNecesitoPara(nBytes,tamanioCorte);
	int i;
	int ini,fin;

	sSplit = malloc((tope +1)* sizeof(char*));

	sSplit[tope] = NULL; //

	ini = 0;
	fin = tamanioCorte;

	for(i=0; i < tope;i++)
	{
		if(i == (tope - 1) && (nBytes % tamanioCorte) != 0) //la 2da sentencia es para el caso de q entre justo en n Bloques (sin frag. interna)
		{
			fin = nBytes % tamanioCorte;
		}
		sSplit[i] = string_substring(s,ini,fin);

		ini += tamanioCorte;
	}

	return sSplit;
}

int cuantosBloquesNecesitoPara(int nCantidadBytes, int tamanioBloque)//char *archivo)
{
	int resto;
	int cantBloquesOcupados;
	//int tamanioDelArchivo;

	//tamanioDelArchivo = strlen(archivo);
	cantBloquesOcupados = nCantidadBytes / tamanioBloque;
	if(cantBloquesOcupados == 0)
	{
		cantBloquesOcupados = 1;
	}
	else
	{
		resto = nCantidadBytes % tamanioBloque;
		if(resto != 0)
			cantBloquesOcupados++;
	}

	return cantBloquesOcupados;
}

/*
 * EL ARRAY DEBE TERMINAR CON EL ULTIMO ELEMENTO EN NULL
 */
/*void freeArrayDePunteros(char **unArrayDePunteros)
{
	int i;

	for(i=0; unArrayDePunteros[i] != NULL ;i++)
	{
		free(unArrayDePunteros[i]);
	}

	free(unArrayDePunteros);
}*/
//##########################################################

//################### PATH ARCHIVO = nombreTabla + nombreArchivo
/**/
char *existeTablaEnFS(char* unNombreTabla)
{
	char* respuesta;
	char* r;
	char* unPath = string_from_format("%s/Metadata",unNombreTabla);

	respuesta = validarArchivoEnFS(unPath);

	if(strcmp(respuesta,"EXISTE_ARCHIVO") == 0)
	{
		r = string_from_format("YA_EXISTE_TABLA");
	}
	else
	{
		r = string_from_format("NO_EXISTE_TABLA");
	}

	free(unPath);

	return r;
}

char *crearTablaEnFS(t_CREATE *unCREATE)
{
	char *r = existeTablaEnFS(unCREATE->nombreTabla);
	//printf("\nJJJJJ RESPUESTA: %s\n",Respuesta);

	if(strcmp(r,"NO_EXISTE_TABLA") == 0)
	{
		//CADA ARCHIVO .BIN PARTICION, AL CREAR LA TABLA, OCUPA SOLO 1 BYTE, PERO DEBE OCUPAR UN BLOQUE
		if(hayEspacioDisponible(unCREATE->nParticiones * metadata.tamanioBloque))
		{
			int i;
			char *nombreAbsoluto;

			//LOS .BIN DE UNA TABLA
			for(i=0;i < unCREATE->nParticiones;i++)
			{
				nombreAbsoluto = string_from_format("%s/%d.bin",unCREATE->nombreTabla,i);
				crearArchivoEnFS(nombreAbsoluto);
				free(nombreAbsoluto);
			}

			char *bufferLinea = string_from_format("CONSISTENCY=%s\nPARTITIONS=%d\nCOMPACTION_TIME=%d\n",unCREATE->tipoConsistencia,unCREATE->nParticiones,unCREATE->tiempoCompactacion);

			nombreAbsoluto = string_from_format("%s%s%s/Metadata",configLFS.puntoMontaje,dirTables,unCREATE->nombreTabla);
			escribirLineaEn(nombreAbsoluto,bufferLinea);
			free(nombreAbsoluto);
			free(bufferLinea);
			r = string_from_format("SUCCESSFUL_CREATE");
		}
		else
		{
			r = string_from_format("NO_HAY_ESPACIO");
		}
	}

	return r;
}

// unPath = nombreTabla + nombreArchivo  (.bin   .temp  .tempc)
char *obtenerDatosDeArchivoEnFS(char *unPath)
{
	char *Respuesta = validarArchivoEnFS(unPath);

	if(strcmp(Respuesta,"EXISTE_ARCHIVO") == 0)
	{
		free(Respuesta);

		char *nombreAmsoluto = string_from_format("%s%s%s",configLFS.puntoMontaje,dirTables,unPath);
		char *buffer = getTodoElArchivoBufferDe(nombreAmsoluto);
		printf("\nnombreAmsoluto: %s",nombreAmsoluto); ///
		free(nombreAmsoluto);

		return buffer;
	}

	return Respuesta;
}

/*unPathArchivo = nombreTabla + nombreArchivo
 * todoElArchivo SE LIBERA, Y DEBE USARCE MALLOC ANTES DE USAR LA FUNCION
 */
char *guardarDatosEnArchivoEnFS(char *unPathArchivo, char *todoElArchivo)
{
	char *Respuesta = validarArchivoEnFS(unPathArchivo);

	if(strcmp(Respuesta,"EXISTE_ARCHIVO") == 0)
	{
		int tamanioArchivo = strlen(todoElArchivo);
		char *nombreAbsoluto = string_from_format("%s%s%s",configLFS.puntoMontaje,dirTables,unPathArchivo);

		liberarBloquesDe(nombreAbsoluto);
		bool hayEspacio = hayEspacioDisponible(tamanioArchivo);
		reAsignarBloquesDe(nombreAbsoluto);

		free(Respuesta);

		if(hayEspacio)
		{
			mostrarBitsDeBloques(8); ///
			borrarArchivoEnFS(unPathArchivo);
			mostrarBitsDeBloques(8); ///

			//### DE CREAR ARCHIVO #########################
			char *bloques = asignarBloques(tamanioArchivo);
			char *bufferLinea = string_from_format("TAMANIO=%d\nBLOQUES=%s\n",tamanioArchivo,bloques);

			escribirLineaEn(nombreAbsoluto,bufferLinea);
			guardarBufferArchivoEn(nombreAbsoluto,todoElArchivo,tamanioArchivo);
			mostrarBitsDeBloques(8); ///

			free(bloques);
			free(bufferLinea);
			//##############################

			Respuesta = string_from_format("ARCHIVO_GUARDADO");
		}
		else
		{
			Respuesta = string_from_format("NO_HAY_ESPACIO");
		}

		free(nombreAbsoluto);
	}

	free(todoElArchivo);

	return Respuesta;
}

char *eliminarArchivoEnFS(char *unPathArchivo)
{
	char *Respuesta = validarArchivoEnFS(unPathArchivo);
	//printf("\n#JJJJJ### RESPUESTA: %s\n",Respuesta);
	if(strcmp(Respuesta,"EXISTE_ARCHIVO") == 0)
	{
		free(Respuesta);

		borrarArchivoEnFS(unPathArchivo);
		Respuesta = string_from_format("ARCHIVO_BORRADO");
	}

	return Respuesta;
}

t_MetadataTabla *obtenerMetadataTabla(char *unNombreTabla)
{
	t_MetadataTabla *unMetadataTabla = malloc(sizeof(t_MetadataTabla));
	char *unNombreAbsoluto = string_from_format("%s%s%s/Metadata",configLFS.puntoMontaje,dirTables,unNombreTabla);
	//printf("\n[%s]\n",unNombreAbsoluto); ///

	archivoMetadataTabla = config_create(unNombreAbsoluto);


	if (config_has_property(archivoMetadataTabla, "CONSISTENCY"))
		unMetadataTabla->Consistency = string_from_format("%s",config_get_string_value(archivoMetadataTabla,"CONSISTENCY"));
		//unMetadataTabla->Consistency = config_get_string_value(archivoMetadataTabla,"CONSISTENCY");
	else
	{
		log_error(logger,"No se encontro la key CONSISTENCY en el archivo de configuracion");
		exitLFS(EXIT_FAILURE);
	}//printf("\nA [%s]  long:%d",unMetadataTabla->Consistency,strlen(unMetadataTabla->Consistency));

	if (config_has_property(archivoMetadataTabla, "PARTITIONS"))
		unMetadataTabla->Partitions = config_get_int_value(archivoMetadataTabla,"PARTITIONS");
	else
	{
		log_error(logger,"No se encontro la key PARTITIONS en la metadata del archivo solicitado");
		exitLFS(EXIT_FAILURE);
	}

	if (config_has_property(archivoMetadataTabla, "COMPACTION_TIME"))
		unMetadataTabla->Compaction_Time = config_get_int_value(archivoMetadataTabla,"COMPACTION_TIME");
	else
	{
		log_error(logger,"No se encontro la key COMPACTION_TIME en la metadata del archivo solicitado");
		exitLFS(EXIT_FAILURE);
	}
	//printf("\n[%s]\n%d\n%d",unMetadataTabla->Consistency,unMetadataTabla->Partitions,unMetadataTabla->Compaction_Time);

	free(unNombreAbsoluto);
	config_destroy(archivoMetadataTabla);
	//printf("\n[%s]\n%d\n%d",unMetadataTabla->Consistency,unMetadataTabla->Partitions,unMetadataTabla->Compaction_Time);

	return unMetadataTabla;
}

void freeT_MetadataTabla(t_MetadataTabla *unStruct)
{
	free(unStruct->Consistency);
	free(unStruct);
}

/*ENLISTA, SOLAMENTE, DEL PATH: CARPETAS, *.TMP, *TMPC
*/
char **enlistarElPath(char *unPath)
{
    struct dirent *dp;
    DIR *dir = opendir(unPath);

    char **listado;
    char *unFile;
    char *aux = string_new();

    //if (!dir)
        //return;

    while ((dp = readdir(dir)) != NULL)
    {
    	if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && strcmp(dp->d_name, "Metadata"))
    		if(!string_ends_with(dp->d_name,".bin"))
    		{
    			unFile = string_from_format(";%s",dp->d_name);
    			string_append(&aux,unFile);
    			//printf("%s\n", dp->d_name); ///
    			free(unFile);
    		}
    }

    listado = string_split(aux,";");
    free(aux);

    closedir(dir);

    return listado;
}
/* ENLISTA TOD0 MENOS EL ARCHIVO "Metadata" (E . Y ..) */
char **enlistarCarpetaTabla(char *unPath)
{
	//char *pathTabla = string_from_format("%s/Tables/%s",configLFS.puntoMontaje,unNombreTabla);
    struct dirent *dp;
    DIR *dir = opendir(unPath);

    char **listado;
    char *unFile;
    char *aux = string_new();

    //if (!dir)
        //return;

    while ((dp = readdir(dir)) != NULL)
    {
    	if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && strcmp(dp->d_name, "Metadata"))
    	{	unFile = string_from_format(";%s",dp->d_name);
    		string_append(&aux,unFile);
    		//printf("%s\n", dp->d_name); ///
    		free(unFile);
    	}
    }

    listado = string_split(aux,";");
    free(aux);

    closedir(dir);

    return listado;
}

void cargarTablasPersistidasEnMEMTABLE(void)
{
	char *pathTablas = string_from_format("%s/Tables",configLFS.puntoMontaje);
	char **listaTablas = enlistarElPath(pathTablas);
	char *pathUnaTabla;
	char **listaArchivosTMP;
	int i;
	int j;
	int tipo;

	log_info(logger,"Cargando Tablas persistidas...");

	for(i=0; listaTablas[i] != NULL;i++)
	{
		pathUnaTabla = string_from_format("%s/Tables/%s",configLFS.puntoMontaje,listaTablas[i]);
		listaArchivosTMP = enlistarElPath(pathUnaTabla);

		t_Tabla *nuevaTabla = crearTablaEnMEMTABLE(listaTablas[i]);
		printf("\nTabla: [%s]\n",listaTablas[i]);
		for(j=0;listaArchivosTMP[j] != NULL;j++)
		{
			tipo = (string_ends_with(listaArchivosTMP[j],".tmp"))? 0:1;
			agregarArchivoTempALista(nuevaTabla->temporales,listaArchivosTMP[j],tipo);
			printf("archivo: [%s]\ntipo: %d",listaArchivosTMP[j],tipo);
		}

		free(pathUnaTabla);
		freeArrayDePunteros(listaArchivosTMP);
	}

	freeArrayDePunteros(listaTablas);
}
