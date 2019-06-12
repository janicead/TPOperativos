/*
 * lfsSerializacion.c
 *
 *  Created on: 27 abr. 2019
 *      Author: utnso
 */

//#include <stdio.h>
#include "lfsSerializacion.h"


//##########################################################
t_ValidarPath *definirT_ValidarPath(char *unPath)
{
	t_ValidarPath *unStructPath = (t_ValidarPath *) malloc(sizeof(t_ValidarPath));

	unStructPath->longPath = strlen(unPath);

	unStructPath->Path = malloc((unStructPath->longPath * sizeof(char)) +1); //+1 V

	strcpy(unStructPath->Path,unPath);  //EXPLOTA TOD0 ASI, NI IDEA XQ  (CREO Q AL PASAR EL PATH X LA FUNCION COMO NO TIENEE \0, AL MOMENTO DE HACER EL COPY COPIA DEMAS :/)
	//no alocaba y escribia en sectores no alocados  -.-

	return unStructPath;
}

char *serializarT_ValidarPath(t_ValidarPath *structASerializar)
{
	char *unPathSerializado = malloc(sizeof(uint32_t) + structASerializar->longPath); // => 40  (4 + 36)
	/// EN EL MALLOC EXPLOTA TOD0, al ser 40
	if (unPathSerializado==NULL)
		printf("\nEN NULL");
	int offset;

	//printf("\ndentro de serializarPath():\nlongPath: %d\nPath: %s\n",unPath->longPath,unPath->Path);

	offset = 0;
	memcpy(unPathSerializado + offset, &(structASerializar->longPath),sizeof(structASerializar->longPath));

	offset += sizeof(structASerializar->longPath);
	memcpy(unPathSerializado + offset, (structASerializar->Path),structASerializar->longPath);

	//printf("unPathSerializado: %s\nstrlen(unPathSerializado): %d",unPathSerializado,strlen(unPathSerializado));
	return unPathSerializado;
}

t_ValidarPath *deserializarAT_ValidarPath(char *t_ValidarPathSerializado)
{
	t_ValidarPath *unPath = (t_ValidarPath*) malloc(sizeof(t_ValidarPath));
	int offset = 0;


	memcpy(&unPath->longPath, t_ValidarPathSerializado + offset,sizeof(uint32_t));

	unPath->Path = (char *) malloc(unPath->longPath +1); //+1 V

	offset += sizeof(unPath->longPath);
	//offset += sizeof(uint32_t);  // es lo mismo q lo anteriro ,
	memcpy(unPath->Path, t_ValidarPathSerializado + offset,(unPath->longPath)); //ojo Q ACA NO VA EL "&"

	unPath->Path[unPath->longPath]='\0';  //ES NECESARIO? -> SI


	return unPath;
}

void freeT_ValidarPath(t_ValidarPath *unStruct)
{
	free(unStruct->Path);
	free(unStruct);
}

//##########################################################
t_CrearArchivo *definirT_CrearArchivo(char *unPath, int nCantidadDeBytes)
{
	t_CrearArchivo *unStruct = (t_CrearArchivo *) malloc(sizeof(t_CrearArchivo));

	unStruct->NcantidadBytes = nCantidadDeBytes;
	unStruct->longPath = strlen(unPath);

	unStruct->Path = malloc(unStruct->longPath * sizeof(char) +1); //V

	strcpy(unStruct->Path,unPath);

	return unStruct;
}

char *serializarT_CrearArchivo(t_CrearArchivo *structASerializar)
{
	char *structSerializado = malloc((sizeof(uint32_t) * 2) + structASerializar->longPath);
	int offset;

	offset = 0;
	memcpy(structSerializado + offset,&(structASerializar->NcantidadBytes),sizeof(structASerializar->NcantidadBytes));

	offset += sizeof(structASerializar->NcantidadBytes);
	memcpy(structSerializado + offset,&(structASerializar->longPath),sizeof(structASerializar->longPath));

	offset += sizeof(structASerializar->longPath);
	memcpy(structSerializado + offset, (structASerializar->Path),structASerializar->longPath);

	return structSerializado;
}

t_CrearArchivo *deserializarAT_CrearArchivo(char *t_CrearArchivoSerializado)
{
	t_CrearArchivo *structDeserializado = (t_CrearArchivo *) malloc(sizeof(t_CrearArchivo));

	int offset = 0;
	memcpy(&structDeserializado->NcantidadBytes, t_CrearArchivoSerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->longPath, t_CrearArchivoSerializado + offset, sizeof(uint32_t));

	structDeserializado->Path = (char *) malloc(structDeserializado->longPath +1); //+1 V?
	offset += sizeof(structDeserializado->longPath); // = OFFSET += SIZEOF(uint32_t);
	memcpy(structDeserializado->Path, t_CrearArchivoSerializado + offset, structDeserializado->longPath);//ojo Q ACA NO VA EL "&"

	structDeserializado->Path[structDeserializado->longPath] = '\0'; //ES NECESARIO? -> SI

	return structDeserializado;
}

void freeT_CrearArchivo(t_CrearArchivo *unStruct)
{
	free(unStruct->Path);
	free(unStruct);
}


//##########################################################
t_ObtenerDatos *definirT_ObtenerDatos(char *unPath, int offset, int size)
{
	t_ObtenerDatos *unStruct = (t_ObtenerDatos *) malloc(sizeof(t_ObtenerDatos));

	unStruct->Offset = offset;
	unStruct->Size = size;
	unStruct->longPath = strlen(unPath);

	unStruct->Path = malloc(unStruct->longPath * sizeof(char));

	strcpy(unStruct->Path,unPath);

	return unStruct;
}

char *serializarT_ObtenerDatos(t_ObtenerDatos *structASerializar)
{
	char *structSerializado = malloc((sizeof(uint32_t) * 3) + structASerializar->longPath);
	int offset;

	offset = 0;
	memcpy(structSerializado + offset,&(structASerializar->Offset),sizeof(structASerializar->Offset));

	offset += sizeof(structASerializar->Offset);
	memcpy(structSerializado + offset,&(structASerializar->Size),sizeof(structASerializar->Size));

	offset += sizeof(structASerializar->Size);
	memcpy(structSerializado + offset,&(structASerializar->longPath),sizeof(structASerializar->longPath));

	offset += sizeof(structASerializar->longPath);
	memcpy(structSerializado + offset, (structASerializar->Path),structASerializar->longPath);

	return structSerializado;
}

t_ObtenerDatos *deserializarAT_ObtenerDatos(char *t_ObtenerDatosSerializado)
{
	t_ObtenerDatos *structDeserializado = (t_ObtenerDatos *) malloc(sizeof(t_ObtenerDatos));

	int offset = 0;
	memcpy(&structDeserializado->Offset, t_ObtenerDatosSerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->Size, t_ObtenerDatosSerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->longPath, t_ObtenerDatosSerializado + offset, sizeof(uint32_t));

	structDeserializado->Path = (char *) malloc(structDeserializado->longPath +1);
	offset += sizeof(structDeserializado->longPath); // = OFFSET += SIZEOF(uint32_t);
	memcpy(structDeserializado->Path, t_ObtenerDatosSerializado + offset, structDeserializado->longPath);//ojo Q ACA NO VA EL "&"

	structDeserializado->Path[structDeserializado->longPath] = '\0'; //ES NECESARIO? -> SI

	return structDeserializado;
}

void freeT_ObtenerDatos(t_ObtenerDatos *unStruct)
{
	free(unStruct->Path);
	free(unStruct);
}

//##########################################################
t_GuardarDatos *definirT_GuardarDatos(char *unPath, int offset, int size, char *buffer)
{
	t_GuardarDatos *unStruct = (t_GuardarDatos *) malloc(sizeof(t_GuardarDatos));

	unStruct->Offset = offset;
	unStruct->Size = size;
	unStruct->longPath = strlen(unPath);
	unStruct->longBuffer = strlen(buffer);

	unStruct->Path = malloc(unStruct->longPath * sizeof(char) +1);
	strcpy(unStruct->Path,unPath);

	unStruct->Buffer = malloc(unStruct->longBuffer * sizeof(char) +1);
	strcpy(unStruct->Buffer,buffer);

	if(unStruct->Size > unStruct->longBuffer)
	{
		printf("\nERROR al definir SIZE, es mayor que la longitud del BUFFER\n");
		freeT_GuardarDatos(unStruct);
		exit(1);
	}

	return unStruct;
}

char *serializarT_GuardarDatos(t_GuardarDatos *structASerializar)
{
	char *structSerializado = malloc((sizeof(uint32_t) * 4) + structASerializar->longPath + structASerializar->longBuffer);
	int offset;

	offset = 0;
	memcpy(structSerializado + offset,&(structASerializar->Offset),sizeof(structASerializar->Offset));

	offset += sizeof(structASerializar->Offset);
	memcpy(structSerializado + offset,&(structASerializar->Size),sizeof(structASerializar->Size));

	offset += sizeof(structASerializar->Size);
	memcpy(structSerializado + offset,&(structASerializar->longPath),sizeof(structASerializar->longPath));

	offset += sizeof(structASerializar->longPath);
	memcpy(structSerializado + offset,&(structASerializar->longBuffer),sizeof(structASerializar->longBuffer));

	offset += sizeof(structASerializar->longBuffer);
	memcpy(structSerializado + offset, (structASerializar->Path),structASerializar->longPath);

	offset += structASerializar->longPath;
	memcpy(structSerializado + offset, (structASerializar->Buffer),structASerializar->longBuffer);

	return structSerializado;
}

t_GuardarDatos *deserializarAT_GuardarDatos(char *t_GuardarDatosSerializado)
{
	t_GuardarDatos *structDeserializado = (t_GuardarDatos *) malloc(sizeof(t_GuardarDatos));

	int offset = 0;
	memcpy(&structDeserializado->Offset, t_GuardarDatosSerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->Size, t_GuardarDatosSerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->longPath, t_GuardarDatosSerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->longBuffer, t_GuardarDatosSerializado + offset, sizeof(uint32_t));



	structDeserializado->Path = (char *) malloc(structDeserializado->longPath +1);
	offset += sizeof(structDeserializado->longBuffer); // = OFFSET += SIZEOF(uint32_t);
	memcpy(structDeserializado->Path, t_GuardarDatosSerializado + offset, structDeserializado->longPath);//ojo Q ACA NO VA EL "&"

	structDeserializado->Path[structDeserializado->longPath] = '\0'; //ES NECESARIO? -> SI


	structDeserializado->Buffer = (char *) malloc(structDeserializado->longBuffer +1);
	offset += structDeserializado->longPath;
	memcpy(structDeserializado->Buffer, t_GuardarDatosSerializado + offset, structDeserializado->longBuffer);//ojo Q ACA NO VA EL "&"

	structDeserializado->Buffer[structDeserializado->longBuffer] = '\0'; //ES NECESARIO? -> SI

	return structDeserializado;
}

void freeT_GuardarDatos(t_GuardarDatos *unStruct)
{
	free(unStruct->Path);
	free(unStruct->Buffer);
	free(unStruct);
}

//##########################################################

char *deserializarRespuesta(char *PayloadRespuesta)
{
	t_ValidarPath *respuestaPATH;
	respuestaPATH = deserializarAT_ValidarPath(PayloadRespuesta);

	char *Respuesta = string_from_format("%s",respuestaPATH->Path);
	freeT_ValidarPath(respuestaPATH);

	return Respuesta;
}
//##########################################################
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
void freeArrayDePunteros(char **unArrayDePunteros)
{
	int i;

	for(i=0; unArrayDePunteros[i] != NULL ;i++)
	{
		free(unArrayDePunteros[i]);
	}

	//free(unArrayDePunteros[i+1]);

	free(unArrayDePunteros);
}
//##########################################################


