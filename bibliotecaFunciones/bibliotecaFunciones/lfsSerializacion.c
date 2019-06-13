/*
 * lfsSerializacion.c
 *
 *  Created on: 27 abr. 2019
 *      Author: utnso
 */

//#include <stdio.h>
#include "lfsSerializacion.h"


//##########################################################
t_UnString *definirT_UnString(char *unString)
{
	t_UnString *unStructPath = (t_UnString *) malloc(sizeof(t_UnString));

	unStructPath->longString = strlen(unString);

	unStructPath->String = malloc((unStructPath->longString * sizeof(char)) +1); //+1 V

	strcpy(unStructPath->String,unString);  //EXPLOTA TOD0 ASI, NI IDEA XQ  (CREO Q AL PASAR EL PATH X LA FUNCION COMO NO TIENEE \0, AL MOMENTO DE HACER EL COPY COPIA DEMAS :/)
	//no alocaba y escribia en sectores no alocados  -.-

	return unStructPath;
}

char *serializarT_UnString(t_UnString *structASerializar)
{
	char *unPathSerializado = malloc(sizeof(uint32_t) + structASerializar->longString); // => 40  (4 + 36)
	/// EN EL MALLOC EXPLOTA TOD0, al ser 40
	if (unPathSerializado==NULL)
		printf("\nEN NULL");
	int offset;

	//printf("\ndentro de serializarPath():\nlongPath: %d\nPath: %s\n",unPath->longPath,unPath->Path);

	offset = 0;
	memcpy(unPathSerializado + offset, &(structASerializar->longString),sizeof(structASerializar->longString));

	offset += sizeof(structASerializar->longString);
	memcpy(unPathSerializado + offset, (structASerializar->String),structASerializar->longString);

	//printf("unPathSerializado: %s\nstrlen(unPathSerializado): %d",unPathSerializado,strlen(unPathSerializado));
	return unPathSerializado;
}

t_UnString *deserializarT_UnString(char *t_UnStringSerializado)
{
	t_UnString *unPath = (t_UnString*) malloc(sizeof(t_UnString));
	int offset = 0;


	memcpy(&unPath->longString, t_UnStringSerializado + offset,sizeof(uint32_t));

	unPath->String = (char *) malloc(unPath->longString +1); //+1 V

	offset += sizeof(unPath->longString);
	//offset += sizeof(uint32_t);  // es lo mismo q lo anteriro ,
	memcpy(unPath->String, t_UnStringSerializado + offset,(unPath->longString)); //ojo Q ACA NO VA EL "&"

	unPath->String[unPath->longString]='\0';  //ES NECESARIO? -> SI


	return unPath;
}

void freeT_UnString(t_UnString *unStruct)
{
	free(unStruct->String);
	free(unStruct);
}

//##########################################################
t_SELECT *definirT_SELECT(char* nombreTabla, int unaKey)
{
	t_SELECT *unStruct = (t_SELECT *) malloc(sizeof(t_SELECT));

	unStruct->KEY = unaKey;
	unStruct->longNombre = strlen(nombreTabla);

	unStruct->nombreTabla = malloc(unStruct->longNombre * sizeof(char) +1); //V

	strcpy(unStruct->nombreTabla,nombreTabla);

	return unStruct;
}

char *serializarT_SELECT(t_SELECT *structASerializar)
{
	char *structSerializado = malloc((sizeof(uint32_t) * 2) + structASerializar->longNombre);
	int offset;

	offset = 0;
	memcpy(structSerializado + offset,&(structASerializar->KEY),sizeof(structASerializar->KEY));

	offset += sizeof(structASerializar->KEY);
	memcpy(structSerializado + offset,&(structASerializar->longNombre),sizeof(structASerializar->longNombre));

	offset += sizeof(structASerializar->longNombre);
	memcpy(structSerializado + offset, (structASerializar->nombreTabla),structASerializar->longNombre);

	return structSerializado;
}

t_SELECT *deserializarT_SELECT(char *t_SELECTSerializado)
{
	t_SELECT *structDeserializado = (t_SELECT *) malloc(sizeof(t_SELECT));

	int offset = 0;
	memcpy(&structDeserializado->KEY, t_SELECTSerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->longNombre, t_SELECTSerializado + offset, sizeof(uint32_t));

	structDeserializado->nombreTabla = (char *) malloc(structDeserializado->longNombre +1); //+1 V?
	offset += sizeof(structDeserializado->longNombre); // = OFFSET += SIZEOF(uint32_t);
	memcpy(structDeserializado->nombreTabla, t_SELECTSerializado + offset, structDeserializado->longNombre);//ojo Q ACA NO VA EL "&"

	structDeserializado->nombreTabla[structDeserializado->longNombre] = '\0'; //ES NECESARIO? -> SI

	return structDeserializado;
}

void freeT_SELECT(t_SELECT *unStruct)
{
	free(unStruct->nombreTabla);
	free(unStruct);
}

//##########################################################
t_INSERT *definirT_INSERT(char *nombreTabla, int unaKey, char* unValue, int timeStamp)
{
	t_INSERT *unStruct = (t_INSERT *) malloc(sizeof(t_INSERT));

	unStruct->KEY = unaKey;
	unStruct->timeStamp = timeStamp;
	unStruct->longNombre = strlen(nombreTabla);
	unStruct->longValue = strlen(unValue);

	unStruct->nombreTabla = malloc(unStruct->longNombre * sizeof(char) +1);
	strcpy(unStruct->nombreTabla,nombreTabla);

	unStruct->Value = malloc(unStruct->longValue * sizeof(char) +1);
	strcpy(unStruct->Value,unValue);


	return unStruct;
}

char *serializarT_INSERT(t_INSERT *structASerializar)
{
	char *structSerializado = malloc((sizeof(uint32_t) * 4) + structASerializar->longNombre + structASerializar->longValue);
	int offset;

	offset = 0;
	memcpy(structSerializado + offset,&(structASerializar->KEY),sizeof(structASerializar->KEY));

	offset += sizeof(structASerializar->KEY);
	memcpy(structSerializado + offset,&(structASerializar->timeStamp),sizeof(structASerializar->timeStamp));

	offset += sizeof(structASerializar->timeStamp);
	memcpy(structSerializado + offset,&(structASerializar->longNombre),sizeof(structASerializar->longNombre));

	offset += sizeof(structASerializar->longNombre);
	memcpy(structSerializado + offset,&(structASerializar->longValue),sizeof(structASerializar->longValue));

	offset += sizeof(structASerializar->longValue);
	memcpy(structSerializado + offset, (structASerializar->nombreTabla),structASerializar->longNombre);

	offset += structASerializar->longNombre;
	memcpy(structSerializado + offset, (structASerializar->Value),structASerializar->longValue);

	return structSerializado;
}

t_INSERT *deserializarT_INSERT(char *t_INSERTSerializado)
{
	t_INSERT *structDeserializado = (t_INSERT *) malloc(sizeof(t_INSERT));

	int offset = 0;
	memcpy(&structDeserializado->KEY, t_INSERTSerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->timeStamp, t_INSERTSerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->longNombre, t_INSERTSerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->longValue, t_INSERTSerializado + offset, sizeof(uint32_t));



	structDeserializado->nombreTabla = (char *) malloc(structDeserializado->longNombre +1);
	offset += sizeof(structDeserializado->longValue); // = OFFSET += SIZEOF(uint32_t);
	memcpy(structDeserializado->nombreTabla, t_INSERTSerializado + offset, structDeserializado->longNombre);//ojo Q ACA NO VA EL "&"

	structDeserializado->nombreTabla[structDeserializado->longNombre] = '\0'; //ES NECESARIO? -> SI


	structDeserializado->Value = (char *) malloc(structDeserializado->longValue +1);
	offset += structDeserializado->longNombre;
	memcpy(structDeserializado->Value, t_INSERTSerializado + offset, structDeserializado->longValue);//ojo Q ACA NO VA EL "&"

	structDeserializado->Value[structDeserializado->longValue] = '\0'; //ES NECESARIO? -> SI

	return structDeserializado;
}

void freeT_INSERT(t_INSERT *unStruct)
{
	free(unStruct->nombreTabla);
	free(unStruct->Value);
	free(unStruct);
}

//##########################################################
t_CREATE *definirT_CREATE(char *nombreTabla, char* tipoConsistencia, int nroParticiones, int tiempoCompactacion)
{
	t_CREATE *unStruct = (t_CREATE *) malloc(sizeof(t_CREATE));

	unStruct->nParticiones = nroParticiones;
	unStruct->tiempoCompactacion = tiempoCompactacion;
	unStruct->longNombre = strlen(nombreTabla);
	unStruct->longConsistencia = strlen(tipoConsistencia);

	unStruct->nombreTabla = malloc(unStruct->longNombre * sizeof(char) +1);
	strcpy(unStruct->nombreTabla,nombreTabla);

	unStruct->tipoConsistencia = malloc(unStruct->longConsistencia * sizeof(char) +1);
	strcpy(unStruct->tipoConsistencia,tipoConsistencia);


	return unStruct;
}

char *serializarT_CREATE(t_CREATE *structASerializar)
{
	char *structSerializado = malloc((sizeof(uint32_t) * 4) + structASerializar->longNombre + structASerializar->longConsistencia);
	int offset;

	offset = 0;
	memcpy(structSerializado + offset,&(structASerializar->nParticiones),sizeof(structASerializar->nParticiones));

	offset += sizeof(structASerializar->nParticiones);
	memcpy(structSerializado + offset,&(structASerializar->tiempoCompactacion),sizeof(structASerializar->tiempoCompactacion));

	offset += sizeof(structASerializar->tiempoCompactacion);
	memcpy(structSerializado + offset,&(structASerializar->longNombre),sizeof(structASerializar->longNombre));

	offset += sizeof(structASerializar->longNombre);
	memcpy(structSerializado + offset,&(structASerializar->longConsistencia),sizeof(structASerializar->longConsistencia));

	offset += sizeof(structASerializar->longConsistencia);
	memcpy(structSerializado + offset, (structASerializar->nombreTabla),structASerializar->longNombre);

	offset += structASerializar->longNombre;
	memcpy(structSerializado + offset, (structASerializar->tipoConsistencia),structASerializar->longConsistencia);

	return structSerializado;
}

t_CREATE *deserializarT_CREATE(char *t_CREATESerializado)
{
	t_CREATE *structDeserializado = (t_CREATE *) malloc(sizeof(t_CREATE));

	int offset = 0;
	memcpy(&structDeserializado->nParticiones, t_CREATESerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->tiempoCompactacion, t_CREATESerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->longNombre, t_CREATESerializado + offset, sizeof(uint32_t));

	offset += sizeof(uint32_t);
	memcpy(&structDeserializado->longConsistencia, t_CREATESerializado + offset, sizeof(uint32_t));



	structDeserializado->nombreTabla = (char *) malloc(structDeserializado->longNombre +1);
	offset += sizeof(structDeserializado->longConsistencia); // = OFFSET += SIZEOF(uint32_t);
	memcpy(structDeserializado->nombreTabla, t_CREATESerializado + offset, structDeserializado->longNombre);//ojo Q ACA NO VA EL "&"

	structDeserializado->nombreTabla[structDeserializado->longNombre] = '\0'; //ES NECESARIO? -> SI


	structDeserializado->tipoConsistencia = (char *) malloc(structDeserializado->longConsistencia +1);
	offset += structDeserializado->longNombre;
	memcpy(structDeserializado->tipoConsistencia, t_CREATESerializado + offset, structDeserializado->longConsistencia);//ojo Q ACA NO VA EL "&"

	structDeserializado->tipoConsistencia[structDeserializado->longConsistencia] = '\0'; //ES NECESARIO? -> SI

	return structDeserializado;
}

void freeT_CREATE(t_CREATE *unStruct)
{
	free(unStruct->nombreTabla);
	free(unStruct->tipoConsistencia);
	free(unStruct);
}

//##########################################################
t_DESCRIBE *definirT_DESCRIBE(char *unNombreTabla)
{
	t_DESCRIBE *unStructPath = (t_DESCRIBE *) malloc(sizeof(t_DESCRIBE));

	unStructPath->longNombre = strlen(unNombreTabla);

	unStructPath->nombreTabla = malloc((unStructPath->longNombre * sizeof(char)) +1); //+1 V

	strcpy(unStructPath->nombreTabla,unNombreTabla);  //EXPLOTA TOD0 ASI, NI IDEA XQ  (CREO Q AL PASAR EL PATH X LA FUNCION COMO NO TIENEE \0, AL MOMENTO DE HACER EL COPY COPIA DEMAS :/)
	//no alocaba y escribia en sectores no alocados  -.-

	return unStructPath;
}

char *serializarT_DESCRIBE(t_DESCRIBE *structASerializar)
{
	char *unPathSerializado = malloc(sizeof(uint32_t) + structASerializar->longNombre); // => 40  (4 + 36)
	/// EN EL MALLOC EXPLOTA TOD0, al ser 40
	if (unPathSerializado==NULL)
		printf("\nEN NULL");
	int offset;

	//printf("\ndentro de serializarPath():\nlongPath: %d\nPath: %s\n",unPath->longPath,unPath->Path);

	offset = 0;
	memcpy(unPathSerializado + offset, &(structASerializar->longNombre),sizeof(structASerializar->longNombre));

	offset += sizeof(structASerializar->longNombre);
	memcpy(unPathSerializado + offset, (structASerializar->nombreTabla),structASerializar->longNombre);

	//printf("unPathSerializado: %s\nstrlen(unPathSerializado): %d",unPathSerializado,strlen(unPathSerializado));
	return unPathSerializado;
}

t_DESCRIBE *deserializarT_DESCRIBE(char *t_DESCRIBESerializado)
{
	t_DESCRIBE *unPath = (t_DESCRIBE*) malloc(sizeof(t_DESCRIBE));
	int offset = 0;


	memcpy(&unPath->longNombre, t_DESCRIBESerializado + offset,sizeof(uint32_t));

	unPath->nombreTabla = (char *) malloc(unPath->longNombre +1); //+1 V

	offset += sizeof(unPath->longNombre);
	//offset += sizeof(uint32_t);  // es lo mismo q lo anteriro ,
	memcpy(unPath->nombreTabla, t_DESCRIBESerializado + offset,(unPath->longNombre)); //ojo Q ACA NO VA EL "&"

	unPath->nombreTabla[unPath->longNombre]='\0';  //ES NECESARIO? -> SI


	return unPath;
}

void freeT_DESCRIBE(t_DESCRIBE *unStruct)
{
	free(unStruct->nombreTabla);
	free(unStruct);
}

//##########################################################
t_DROP *definirT_DROP(char *unNombreTabla)
{
	t_DROP *unStructPath = (t_DROP *) malloc(sizeof(t_DROP));

	unStructPath->longNombre = strlen(unNombreTabla);

	unStructPath->nombreTabla = malloc((unStructPath->longNombre * sizeof(char)) +1); //+1 V

	strcpy(unStructPath->nombreTabla,unNombreTabla);  //EXPLOTA TOD0 ASI, NI IDEA XQ  (CREO Q AL PASAR EL PATH X LA FUNCION COMO NO TIENEE \0, AL MOMENTO DE HACER EL COPY COPIA DEMAS :/)
	//no alocaba y escribia en sectores no alocados  -.-

	return unStructPath;
}

char *serializarT_DROP(t_DROP *structASerializar)
{
	char *unPathSerializado = malloc(sizeof(uint32_t) + structASerializar->longNombre); // => 40  (4 + 36)
	/// EN EL MALLOC EXPLOTA TOD0, al ser 40
	if (unPathSerializado==NULL)
		printf("\nEN NULL");
	int offset;

	//printf("\ndentro de serializarPath():\nlongPath: %d\nPath: %s\n",unPath->longPath,unPath->Path);

	offset = 0;
	memcpy(unPathSerializado + offset, &(structASerializar->longNombre),sizeof(structASerializar->longNombre));

	offset += sizeof(structASerializar->longNombre);
	memcpy(unPathSerializado + offset, (structASerializar->nombreTabla),structASerializar->longNombre);

	//printf("unPathSerializado: %s\nstrlen(unPathSerializado): %d",unPathSerializado,strlen(unPathSerializado));
	return unPathSerializado;
}

t_DROP *deserializarT_DROP(char *t_DROPSerializado)
{
	t_DROP *unPath = (t_DROP*) malloc(sizeof(t_DROP));
	int offset = 0;


	memcpy(&unPath->longNombre, t_DROPSerializado + offset,sizeof(uint32_t));

	unPath->nombreTabla = (char *) malloc(unPath->longNombre +1); //+1 V

	offset += sizeof(unPath->longNombre);
	//offset += sizeof(uint32_t);  // es lo mismo q lo anteriro ,
	memcpy(unPath->nombreTabla, t_DROPSerializado + offset,(unPath->longNombre)); //ojo Q ACA NO VA EL "&"

	unPath->nombreTabla[unPath->longNombre]='\0';  //ES NECESARIO? -> SI


	return unPath;
}

void freeT_DROP(t_DROP *unStruct)
{
	free(unStruct->nombreTabla);
	free(unStruct);
}


//##########################################################

char *deserializarRespuesta(char *PayloadRespuesta)
{
	t_UnString *respuestaSTRING;
	respuestaSTRING = deserializarT_UnString(PayloadRespuesta);

	char *Respuesta = string_from_format("%s",respuestaSTRING->String);
	freeT_UnString(respuestaSTRING);

	return Respuesta;
}
//##########################################################
