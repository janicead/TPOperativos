#include "compactador.h"
//#include "lfs.h"

t_list *obtenerArchivoComoLista(char *unNombreTablaArchivo)
{
	t_list *unaLista = list_create();

	char *todoElArchivo = obtenerDatosDeArchivoEnFS(unNombreTablaArchivo);
	printf("\ntodoElArchivo: \n[%s]\n",todoElArchivo); ///

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

	if(tope == 0)
	{
		return string_from_format("sinRegistros");
	}

	t_list *listaADumpear = list_take_and_remove(unaListaRegistros,tope);

	int i;
	t_Registro *unRegistro;
	char *linea;

	for(i=0; i < tope-1 ;i++)
	{
		unRegistro = list_get(listaADumpear,i);
		linea = string_from_format("%lu;%d;%s\n",unRegistro->TIMESTAMP,unRegistro->KEY,unRegistro->VALUE);
		string_append(&listaDumpeada,linea);
		free(linea);
	}

	unRegistro = list_get(listaADumpear,i);
	linea = string_from_format("%lu;%d;%s",unRegistro->TIMESTAMP,unRegistro->KEY,unRegistro->VALUE);
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

void hiloDUMP(void)
{
	while(1)
	{
		sleep(configLFS.tiempoDump);
		realizarDUMP();
	}
}

void realizarDUMP(void)
{
	int topTablas = list_size(memTable);
	int i;

	t_Tabla *unaTabla;

	char *unaTablaDUMPEADA;

	for(i=0; i < topTablas ;i++)
	{
		unaTabla = list_get(memTable,i);

		unaTablaDUMPEADA = dumpearUnaListaDeRegistros(unaTabla->registros);
		printf("\ntabla %s Dumpeada: \n[%s]\n",unaTabla->nombreTabla,unaTablaDUMPEADA); ///

		persistirRegistrarDUMP(unaTabla,unaTablaDUMPEADA);

		//free(unaTablaDUMPEADA);  //SE LIBERA EN persistirRegistrarDUMP()
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

	printf("nombre del archivo TMP: dump%d.tmp\n",numeroDUMP);///
	return string_from_format("dump%d.tmp",numeroDUMP);
}

int cuantosArchivosTempHayEn(t_list *unaListaTemporales)
{
	bool esArchivoTEMP(void *elemento)
	{
		return (((t_ArchivoTemp*)elemento)->tipo == 0);
	}

	return list_count_satisfying(unaListaTemporales,esArchivoTEMP);
}

/* tipo = 0 -> .TEMP
   tipo = 0 -> .TEMPC */
void agregarArchivoTempALista(t_list *unaListaTemporales,char *nombreArchivo, int unTipo)
{
	t_ArchivoTemp *unArchivoTemp = malloc(sizeof(t_ArchivoTemp));

	unArchivoTemp->nombre = string_from_format("%s",nombreArchivo);
	unArchivoTemp->tipo = unTipo;

	printf("\n114 tamanioLISTA: %d  (before TEMPORALES)\n",list_size(unaListaTemporales)); ///
	list_add(unaListaTemporales,unArchivoTemp);
	printf("\n116 tamanioLISTA: %d  (before TEMPORALES)\n",list_size(unaListaTemporales)); ///

}

int numeroDeParticion(int cantParticiones, int unaKey)
{
	return unaKey % cantParticiones;
}

