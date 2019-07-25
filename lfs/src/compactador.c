#include "compactador.h"
//#include "lfs.h"

t_list *obtenerArchivoComoLista(char *unNombreTablaArchivo)
{
	t_list *unaLista = list_create();

	char *todoElArchivo = obtenerDatosDeArchivoEnFS(unNombreTablaArchivo);
	//printf("\ntodoElArchivo: \n[%s]\n",todoElArchivo); ///

	if(strcmp(todoElArchivo,"NO_EXISTE_ARCHIVO") == 0)
	{
		free(todoElArchivo);
		log_info(logger,"\nNo existe el archivo %s, para obtener como lista de registros\n",unNombreTablaArchivo);
		//printf("No existe el archivo %s, para obtener como lista de registros\n",unNombreTablaArchivo);

		return unaLista;
	}

	char **todoElArchivoEnPedazos = string_split(todoElArchivo,"\n");

	free(todoElArchivo);

	int i;
	char **unRegistroEnpedazos;
	t_Registro *unRegistro;

	//printf("\nlongitudArrayDePunteros(): %d",longitudArrayDePunteros(todoElArchivoEnPedazos));
	for(i=0; todoElArchivoEnPedazos[i] != NULL ;i++)
	{ ///no entra al for si la logitud del char** es 0 (el archivo solo tenia'\n')
		unRegistroEnpedazos = string_split(todoElArchivoEnPedazos[i],";");

		unRegistro = malloc(sizeof(t_Registro));

		unRegistro->TIMESTAMP = atol(unRegistroEnpedazos[0]);
		unRegistro->KEY = atol(unRegistroEnpedazos[1]);
		unRegistro->VALUE = string_from_format("%s",unRegistroEnpedazos[2]);

		list_add(unaLista,unRegistro);
		//printf("unREG: [%lu] [%d] [%s]\n",unRegistro->TIMESTAMP,unRegistro->KEY,unRegistro->VALUE);///
		freeArrayDePunteros(unRegistroEnpedazos);
	}

	freeArrayDePunteros(todoElArchivoEnPedazos);

	return unaLista;
}

char *dumpearUnaListaDeRegistros(t_list *unaListaRegistros)
{
	char *listaDumpeada = string_new();
	int tope = list_size(unaListaRegistros);
	//printf("\ndumpearUnaListaDeRegistros.tope: %d\n",tope); ///
	if(tope == 0)
	{
		return string_from_format("sinRegistros");
	}

	t_list *listaADumpear = list_take_and_remove(unaListaRegistros,tope);

	int i;
	//t_Registro *unRegistro;
	char *linea;
//printf("entrando al FOR()\n"); ///
	for(i=0; i < tope-1 ;i++)
	{
		t_Registro *unRegistro = list_get(listaADumpear,i);
		//printf("TIMESTAMP: %lu\n",unRegistro->TIMESTAMP); ///
		//printf("KEY: %d\n",unRegistro->KEY); ///
		//printf("VALUE: %s\n",unRegistro->VALUE); ///
		linea = string_from_format("%lu;%d;%s\n",unRegistro->TIMESTAMP,unRegistro->KEY,unRegistro->VALUE);
		//printf("\nlinea: %s\n",linea); ///
		string_append(&listaDumpeada,linea);
		free(linea);
	}
//printf("saliendo del FOR()\n"); ///
	t_Registro *unRegistro  = list_get(listaADumpear,i);
	linea = string_from_format("%lu;%d;%s",unRegistro->TIMESTAMP,unRegistro->KEY,unRegistro->VALUE);
	//printf("\nlinea: %s\n",linea); ///
	string_append(&listaDumpeada,linea);
	free(linea);

	freeListaDeRegistros(listaADumpear);

	return listaDumpeada;
}

void freeListaDeRegistros(t_list *unaListaRegistros)
{
	void destructorElemento(void *elemento)
	{
		freeT_Registro((t_Registro *)elemento);
	}

	list_destroy_and_destroy_elements(unaListaRegistros,destructorElemento);
}

void freeListaDeTemporales(t_list *unaListaTemporales)
{
	void destructorElemento(void *elemento)
	{
		freeT_ArchivoTemp((t_ArchivoTemp*)elemento);
	}

	list_destroy_and_destroy_elements(unaListaTemporales,destructorElemento);
}

void *hiloDUMP(void *arg)
{
	while(1)
	{
		sleep(configLFS.tiempoDump);
		//printf("\nPASO SLEEP(DUMP)\n");
		//(&laMEMTABLE);
		pthread_mutex_lock(&LISSANDRA);
		realizarDUMP();
		pthread_mutex_unlock(&LISSANDRA);
		//pthread_mutex_unlock(&laMEMTABLE);
	}
}

void realizarDUMP(void)
{
	int topTablas = list_size(memTable);
	int i;

	t_Tabla *unaTabla;

	char *unaTablaDUMPEADA;

	if (topTablas == 0)
	{
		//log_info(logger,"No hay ninguna tabla para DUMP");
	}
	else
	{
		log_info(logger,"Realizando DUMP de tablas...");

		for(i=0; i < topTablas ;i++)
		{
			unaTabla = list_get(memTable,i);

			//pthread_mutex_lock(&unaTabla->noBloqueado);
			unaTablaDUMPEADA = dumpearUnaListaDeRegistros(unaTabla->registros);
			//log_info(logger,"\nTabla [%s] Dumpeada: [%s]\n",unaTabla->nombreTabla,unaTablaDUMPEADA); ///???

			persistirRegistrarDUMP(unaTabla,unaTablaDUMPEADA);

			//pthread_mutex_unlock(&unaTabla->noBloqueado);
			//free(unaTablaDUMPEADA);  //SE LIBERA EN persistirRegistrarDUMP()
		}
	}
	//DEFNIR COMO JUNTAR LOS ARCHIOS TEMPORALES Y REALIZRA LO DUMPEADO AL FS

}

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

			char *r = guardarDatosEnArchivoEnFS(unPath,laTablaDUMPEADA); //SE LIBERA laTablaDUMPEADA
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
			char *unBuffer = string_from_format("\nNo se puede realizar DUMP con la tabla %s: Espacio insuficiente (Se perdieron los registros)",unaTabla->nombreTabla);
			log_info(logger,unBuffer);
//LOS REGISTROS TENDRIAN Q VOLVER A SU LISTA ORIGEN (TAKEBACKLIST)
			free(unBuffer);
		}
	}
}


char *getNombreArchivoTEMP(t_Tabla *unaTabla)
{
	int numeroDUMP = cuantosArchivosTempHayEn(unaTabla->temporales);

	//printf("nombre del archivo TMP: dump%d.tmp\n",numeroDUMP);///
	return string_from_format("dump%d.tmp",numeroDUMP);
}
/* SOLO LOS .TMP */
int cuantosArchivosTempHayEn(t_list *unaListaTemporales)
{
	bool esArchivoTEMP(void *elemento)
	{
		return (((t_ArchivoTemp*)elemento)->tipo == 0);
	}

	return list_count_satisfying(unaListaTemporales,esArchivoTEMP);
}

/* tipo = 0 -> .TMP
   tipo = 0 -> .TMPC */
void agregarArchivoTempALista(t_list *unaListaTemporales,char *nombreArchivo, int unTipo)
{
	t_ArchivoTemp *unArchivoTemp = malloc(sizeof(t_ArchivoTemp));

	unArchivoTemp->nombre = string_from_format("%s",nombreArchivo);
	unArchivoTemp->tipo = unTipo;

	//printf("\n209 tamanioLISTA: %d  (antes TEMPORALES)\n",list_size(unaListaTemporales)); ///
	list_add(unaListaTemporales,unArchivoTemp);
	//printf("\n211 tamanioLISTA: %d  (despues TEMPORALES)\n",list_size(unaListaTemporales)); ///

}

int numeroDeParticion(int cantParticiones, int unaKey)
{
	return unaKey % cantParticiones;
}

//##################################################
void *hiloCOMPACTADOR(void *algunaTabla)
{
	t_Tabla *unaTabla = (t_Tabla*)algunaTabla;
	t_MetadataTabla *unMetadataTabla = obtenerMetadataTabla(unaTabla->nombreTabla);

	while(1)
	{
		sleep(unMetadataTabla->Compaction_Time);

		//pthread_mutex_lock(&unaTabla->noBloqueado);
		//pthread_mutex_lock(&elFS);
		pthread_mutex_lock(&LISSANDRA);
		realizarCOMPACTAR(unaTabla,unMetadataTabla->Partitions);
		pthread_mutex_unlock(&LISSANDRA);
		//pthread_mutex_unlock(&elFS);
		//pthread_mutex_unlock(&unaTabla->noBloqueado);

		//printf("\nHILO COMPACTADOR DE [%s]\n",unaTabla->nombreTabla);

	}

	freeT_MetadataTabla(unMetadataTabla);
	return 0;
}

void realizarCOMPACTAR(t_Tabla *unaTabla, int cantParticiones)
{

	//if() // SI YA EXISTE ARCHIVOS .TMPC ANTES DE INICIAR COMPACTACION ???

	bool serArchivoTMP(void *elemento) // solo .TMP
	{
		return (((t_ArchivoTemp*)elemento)->tipo == 0);
	}

	t_list *listaTMPsFiltrados = list_filter(unaTabla->temporales,serArchivoTMP);

	int tope = list_size(listaTMPsFiltrados);
	t_list *particionesDeTabla;
	int indexBIN;
	time_t inicioDeCOMPACTADOR;

	if(tope != 0)
	{
		log_info(logger,"\nRealizando COMPACTACION en tabla [%s]...",unaTabla->nombreTabla);
		inicioDeCOMPACTADOR = time(NULL); //#######
		//sleep(5);

		//CARGO LAS PARTICIONES DE UNA TABLA A UNA LISTA
		particionesDeTabla = list_create();

		char *unNombreTablaBIN;

		for(indexBIN=0; indexBIN < cantParticiones ;indexBIN++)
		{
			t_ParticionBIN *nuevaParticion = malloc(sizeof(t_ParticionBIN));

			nuevaParticion->particion = indexBIN;

			unNombreTablaBIN = string_from_format("%s/%d.bin",unaTabla->nombreTabla,indexBIN);

			nuevaParticion->registros = obtenerArchivoComoLista(unNombreTablaBIN); //LA F CREA LA LISTA

			list_add(particionesDeTabla,nuevaParticion);

			free(unNombreTablaBIN);
		}
	}
	else // LO COLOCO POR LAS DUDAS
		log_info(logger,"\nNo hay archivos .TMP para compactar en tabla [%s]",unaTabla->nombreTabla);


	//CAMBIAR EXTENCION
	char *nombreActual;
	char *nombreNuevo;
	int i;

	for(i=0; i < tope ;i++)
	{
		t_ArchivoTemp *unArchivoTMP = list_get(listaTMPsFiltrados,i);

		//CAMBIAR A NIVEL FS
		nombreActual = string_from_format("%s/Tables/%s/%s",configLFS.puntoMontaje,unaTabla->nombreTabla,unArchivoTMP->nombre);
		nombreNuevo = string_from_format("%sc",nombreActual);

		//printf("\nnombreActual: %s\n",nombreActual); ///
		//printf("nombreNuevo: %s\n",nombreNuevo); ///
		rename(nombreActual,nombreNuevo);
		free(nombreActual);
		free(nombreNuevo);

		//A NIVEL MEMTABLE
		string_append(&unArchivoTMP->nombre,"c");
		unArchivoTMP->tipo = 1;


	}
	//
	char *unNombreTablaTMPC;
	int topeRegistros; //de un TMPC
	int j;
	//int particionAComparar;

	for(i=0; i < tope ;i++)
	{
		t_ArchivoTemp *unArchivoTMPC = list_get(listaTMPsFiltrados,i);

		unNombreTablaTMPC = string_from_format("%s/%s",unaTabla->nombreTabla,unArchivoTMPC->nombre);

		//printf("\nArchivo TMPC: [%s]\n",unNombreTablaTMPC); ///
		t_list *listaRegistrosDeUnTMPC = obtenerArchivoComoLista(unNombreTablaTMPC); //LA F CREA LA LISTA

		free(unNombreTablaTMPC);

		//COMPARAR... (LA MAGIA)
		topeRegistros = list_size(listaRegistrosDeUnTMPC);

		//printf("list_size(listaRegistrosDeUnTMPC): %d\n",topeRegistros); ///
		for(j=0; j < topeRegistros ;j++)
		{
			t_Registro *unRegistroParaComparar = list_get(listaRegistrosDeUnTMPC,j);
			//particionAComparar = numeroDeParticion(cantParticiones,unRegistroParaComparar->KEY);
			//printf("\nunRegistroParaComparar: \n");
			//printf("	unRegistroTMPC->TIMESTAMP: %lu\n",unRegistroParaComparar->TIMESTAMP); ///
			//printf("	unRegistroTMPC->KEY: %d\n",unRegistroParaComparar->KEY); ///
			//printf("	unRegistroTMPC->VALUE: %s\n",unRegistroParaComparar->VALUE); ///


			analizadorDelRegistro(unRegistroParaComparar,particionesDeTabla, cantParticiones);
		}


		//freeListaDeRegistros(listaRegistrosDeUnTMPC); /// LA Q TE PARIO!!!
		//hay q liberar los registros de
	}


	//BLOQUEAR TABLA
	//GUARDAR LAS PARTICIONES DE LA TABLA A NIVEL FS
	//ELIMINAR LAS REFERENCIAS DE .TMPCs (MEMTABLE)
	//ELIMINAR A NIVEL FS
	//DESBLOQUEAR TABLA
	//unsigned long int finDeCOMPACTADOR = (unsigned long int)time(NULL);

	if(tope != 0) //SE HICIERON COSAS
	{
		//(BLOQUEAR TABLA)
		//printf("\n#### bloquear TABLA ####\n"); ///
		bool seaUnTMPC(void *elemento) // solo .TMPC
		{
			return (((t_ArchivoTemp*)elemento)->tipo == 1);
		}
		//ELIMINO ARCHIVOS .TMPC A NIVEL FS Y MEMTABLE
		char *unPathArchivo;
		//printf("\ntamanio unaTabla->temporales: %d (antes)\n",list_size(unaTabla->temporales));///
		for(i=0; i < tope ;i++)
		{
			t_ArchivoTemp *unArchivo_TMPC = list_remove_by_condition(unaTabla->temporales,seaUnTMPC);

			unPathArchivo = string_from_format("%s/%s",unaTabla->nombreTabla,unArchivo_TMPC->nombre);
			//printf("\nunPathArchivo: %s (borrarArchivoEnFS)\n",unPathArchivo);///
			//mostrarBitsDeBloques(8);///
			borrarArchivoEnFS(unPathArchivo);
			//mostrarBitsDeBloques(8);///
			free(unPathArchivo);

			freeT_ArchivoTemp(unArchivo_TMPC);
		}
		//printf("tamanio unaTabla->temporales: %d (despues)\n",list_size(unaTabla->temporales)); ///
		//printf("#### borre archivos a nivel fs y memtable ####\n"); ///


		//particionesDeTabla --> persistir
		for(indexBIN=0; indexBIN < cantParticiones ;indexBIN++)
		{
			//t_ParticionBIN *unaParticionBIN = list_get(particionesDeTabla,indexBIN);

			bool mismaParticion(void *elemento)
			{
				return (((t_ParticionBIN*)elemento)->particion == indexBIN);
			}

			t_ParticionBIN *unaParticionBIN = list_find(particionesDeTabla,mismaParticion);

			//printf("\nlist_size(unaParticionBIN->registros): %d (%d.bin)\n",list_size(unaParticionBIN->registros),indexBIN); ///
			char *unaBINDUMPEADA = dumpearUnaListaDeRegistros(unaParticionBIN->registros); //unaTabla->registros);
			//printf("%d.bin de [%s] Dumpeada: \n[%s]\n",indexBIN,unaTabla->nombreTabla,unaBINDUMPEADA); ///

			persistirParticionBIN(unaTabla,unaBINDUMPEADA,indexBIN);
		}

		//LIBERAR LISTA DE PARTICIONES DE UNA TABLA
		int cantParticiones = list_size(particionesDeTabla);
		for(i=0; i < cantParticiones ;i++)
		{
			t_ParticionBIN *unaParticionBIN = list_get(particionesDeTabla,i);
			freeListaDeRegistros(unaParticionBIN->registros);
		}

		time_t finDeCOMPACTADOR = time(NULL); //#######

		log_info(logger,"\nTiempo de bloqueo de tabla [%s]: %f seg.",unaTabla->nombreTabla, difftime(finDeCOMPACTADOR, inicioDeCOMPACTADOR));
	}
}

/* unRegistroTMPC ES DE UNA LISTA DE UN .TMPC */
void analizadorDelRegistro(t_Registro *unRegistroTMPC,t_list *particionesDeTabla,int cantParticiones)
{
	int nroParticionAComparar = numeroDeParticion(cantParticiones,unRegistroTMPC->KEY);

	bool particionAcomparar(void *elemento)
	{
		return (((t_ParticionBIN*)elemento)->particion == nroParticionAComparar);
	}

	t_ParticionBIN *laParticion = (t_ParticionBIN *)list_find(particionesDeTabla,particionAcomparar);


	bool registroMismaKEY(void *elemento)
	{
		return (((t_Registro*)elemento)->KEY == unRegistroTMPC->KEY);
	}

	t_Registro *registroEncontrado = (t_Registro *)list_find(laParticion->registros,registroMismaKEY);

	//printf("nroParticionAComparar: %d\n",nroParticionAComparar); ///
	if(registroEncontrado == NULL)
	{
		//printf("\nNo se encontro Registro en la particion %d se agregara...\n",nroParticionAComparar); ///

		//printf("list_size(laParticion->registros): %d (antes)\n",list_size(laParticion->registros)); ///
		list_add(laParticion->registros,unRegistroTMPC);
		//printf("list_size(laParticion->registros): %d (despues)\n",list_size(laParticion->registros)); ///
	}
	else
	{
		//printf("\nSe encontro registro misma KEY:\n"); ///
		if(unRegistroTMPC->TIMESTAMP > registroEncontrado->TIMESTAMP)
		{
			//printf("	registroEncontrado->TIMESTAMP: %lu\n",registroEncontrado->TIMESTAMP); ///
			//printf("	registroEncontrado->KEY: %d\n",registroEncontrado->KEY);///
			//printf("	registroEncontrado->VALUE: %s\n",registroEncontrado->VALUE);///
			//printf("Se actualizo el registro encontrado (registroComparador->timestamp>)\n");///

			registroEncontrado->TIMESTAMP = unRegistroTMPC->TIMESTAMP;
			free(registroEncontrado->VALUE);
			registroEncontrado->VALUE = string_from_format("%s",unRegistroTMPC->VALUE);
		}
			//printf("Se descarto registro comparador (<timestamp)\n");///
	}
}

/* SE LIBERA EL CHAR* */
void persistirParticionBIN(t_Tabla *unaTabla,char *laBINDUMPEADA,int indexBIN)
{
	if (strcmp(laBINDUMPEADA,"sinRegistros") != 0)
	{
		char *unPath = string_from_format("%s/%d.bin",unaTabla->nombreTabla,indexBIN);

		//crearArchivoEnFS(unPath); // YA ESTA CREADO
		//printf("\nunPath: [%s] (guardarDatosEnArchivoEnFS)\n",unPath);///
		//printf("lo q guardo: [%s]\n",laBINDUMPEADA); ///
		char *r = guardarDatosEnArchivoEnFS(unPath,laBINDUMPEADA); //SE LIBERA laBINDUMPEADA
		free(unPath);
		//printf("r: [%s]\n",r); ///
		if(strcmp(r,"ARCHIVO_GUARDADO") == 0)
		{
			char *unBuffer = string_from_format("\n%d.BIN persistido de tabla [%s]",indexBIN,unaTabla->nombreTabla);
			log_info(logger,unBuffer);
			free(unBuffer);
		}
		else if(strcmp(r,"NO_HAY_ESPACIO") == 0)
		{
			char *unBuffer = string_from_format("\nSin actualizar %d.BIN de tabla [%s]: Espacio insuficiente (Se perdio la actualizacion)",indexBIN,unaTabla->nombreTabla);
			log_info(logger,unBuffer);
			free(unBuffer);
		}

		free(r);
	}
}




//########

