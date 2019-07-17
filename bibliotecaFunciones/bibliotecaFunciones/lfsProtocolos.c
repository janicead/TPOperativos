#include "lfsProtocolos.h"

/*char *msjEnviado = string_from_format("Memoria %d",1); //ACA EN VEZ DEL 1, IRIA EL NrO Q TIENE LA MEMORIA
  realizarHandshakeAlLFS(socketServer,msjEnviado);
*/
int realizarHandshakeAlLFS(t_log *logger,int socketServer, char *msjEnviado)
{
	int protocoloID = 11; // 11: Handshake
	int tamanioPagina;

	empaquetarEnviarMensaje(socketServer,protocoloID,strlen(msjEnviado),msjEnviado);
	//printf("\nPAQUETE ENVIADO\n");

	t_PaqueteDeDatos *package;
	//package->Datos = "";
	package = recibirPaquete(socketServer);

	if(package->ID != 12) //12: Respuesta del hancshake enviado
	{
		log_error(logger,"Protocolo respuesta handshake, distinto del esperado   ID:%d",package->ID);
		tamanioPagina = -1;
	}
	else
	{
		tamanioPagina = atoi(package->Datos);
		log_info(logger,"LISSANDRA: Tamanio de VALUE = %d",tamanioPagina);
		log_info(logger,"Handshake  con socket %d: DONE!", socketServer);

	}

	freePackage(package);

	return tamanioPagina;
}

char *opSELECT(int socketReceptor, char* unNombreTabla, int unaKey)
{
	int protocoloID = 13;
	char *selectSerializado;
	t_SELECT *unSELECT = definirT_SELECT(unNombreTabla,unaKey);

	printf("\nQuery:\n	SELECT [%s] [%d]\n",unSELECT->nombreTabla,unSELECT->KEY);

	selectSerializado = serializarT_SELECT(unSELECT);

	//printf("empaquetarEnviarMensaje():\n");///

	//ESTO VARIA SEGUN LA STRUCTURA Q PIENSO MANDAR, hay q hacerlo a mano
	int tamanioStructSerializado = (sizeof(uint32_t) *2) + unSELECT->longNombre;

	empaquetarEnviarMensaje(socketReceptor,protocoloID,tamanioStructSerializado,selectSerializado);

	free(selectSerializado);
	freeT_SELECT(unSELECT);

	//#####################################
	t_PaqueteDeDatos *packageRecibido = recibirPaquete(socketReceptor);
	printf("fd: %d\n",socketReceptor); ///
	printf("package->ID: %d\n",packageRecibido->ID); ///

	if(packageRecibido->ID == 0){
		return "MEMORIA_DESCONECTADA";
	}

	char *respuesta = deserializarRespuesta(packageRecibido->Datos);
	freePackage(packageRecibido);

	return respuesta;
}
/*
 *
*/
char *opINSERT(int socketReceptor, char* unNombreTabla, int unaKey, char* unValue, int unTimestamp)
{
	int protocoloID = 15;
	char *insertSerializado;
	t_INSERT *unINSERT = definirT_INSERT(unNombreTabla,unaKey,unValue,unTimestamp);

	printf("\nQuery:\n	INSERT [%s] [%d] [%s] [%d]\n",unINSERT->nombreTabla,unINSERT->KEY,unINSERT->Value,unINSERT->timeStamp);

	insertSerializado = serializarT_INSERT(unINSERT);

	//printf("empaquetarEnviarMensaje():\n");///

	//ESTO VARIA SEGUN LA STRUCTURA Q PIENSO MANDAR, hay q hacerlo a mano
	int tamanioStructSerializado = (sizeof(uint32_t) *4) + unINSERT->longNombre + unINSERT->longValue;

	empaquetarEnviarMensaje(socketReceptor,protocoloID,tamanioStructSerializado,insertSerializado);

	free(insertSerializado);
	freeT_INSERT(unINSERT);

	//#####################################
	t_PaqueteDeDatos *packageRecibido = recibirPaquete(socketReceptor);
	printf("fd: %d\n",socketReceptor); ///
	printf("package->ID: %d\n",packageRecibido->ID); ///

	if(packageRecibido->ID == 0){
		return "MEMORIA_DESCONECTADA";
	}

	char *respuesta = deserializarRespuesta(packageRecibido->Datos);
	freePackage(packageRecibido);

	return respuesta;
}

char *opCREATE(int socketReceptor, char* unNombreTabla, char* unaConsistencia, int unNParticiones, int unTiempoCompactacion)
{
	int protocoloID = 17;
	char *createSerializado;
	t_CREATE *unCREATE = definirT_CREATE(unNombreTabla,unaConsistencia,unNParticiones,unTiempoCompactacion);

	printf("\nQuery:\n	CREATE [%s] [%s] [%d] [%d]\n",unCREATE->nombreTabla,unCREATE->tipoConsistencia,unCREATE->nParticiones,unCREATE->tiempoCompactacion);

	createSerializado = serializarT_CREATE(unCREATE);

	//printf("empaquetarEnviarMensaje():\n");///

	//ESTO VARIA SEGUN LA STRUCTURA Q PIENSO MANDAR, hay q hacerlo a mano
	int tamanioStructSerializado = (sizeof(uint32_t) *4) + unCREATE->longNombre + unCREATE->longConsistencia;

	empaquetarEnviarMensaje(socketReceptor,protocoloID,tamanioStructSerializado,createSerializado);

	free(createSerializado);
	freeT_CREATE(unCREATE);

	//#####################################
	t_PaqueteDeDatos *packageRecibido = recibirPaquete(socketReceptor);
	printf("fd: %d\n",socketReceptor); ///
	printf("package->ID: %d\n",packageRecibido->ID); ///

	if(packageRecibido->ID == 0){
		return "MEMORIA_DESCONECTADA";
	}

	char *respuesta = deserializarRespuesta(packageRecibido->Datos);
	freePackage(packageRecibido);

	return respuesta;
}

char *opDESCRIBE(int socketReceptor, char* unNombreTabla)
{
	int protocoloID = 19;
	char *describeSerializado;
	t_DESCRIBE *unDESCRIBE = definirT_DESCRIBE(unNombreTabla);

	printf("\nQuery:\n	DESCRIBE [%s]\n",unDESCRIBE->nombreTabla);

	describeSerializado = serializarT_DESCRIBE(unDESCRIBE);

	//printf("empaquetarEnviarMensaje():\n");///

	//ESTO VARIA SEGUN LA STRUCTURA Q PIENSO MANDAR, hay q hacerlo a mano
	int tamanioStructSerializado = (sizeof(uint32_t)) + unDESCRIBE->longNombre;

	empaquetarEnviarMensaje(socketReceptor,protocoloID,tamanioStructSerializado,describeSerializado);

	free(describeSerializado);
	freeT_DESCRIBE(unDESCRIBE);

	//#####################################
	t_PaqueteDeDatos *packageRecibido = recibirPaquete(socketReceptor);
	printf("fd: %d\n",socketReceptor); ///
	printf("package->ID: %d\n",packageRecibido->ID); ///

	if(packageRecibido->ID == 0){
		return "MEMORIA_DESCONECTADA";
	}

	char *respuesta = deserializarRespuesta(packageRecibido->Datos);
	freePackage(packageRecibido);

	return respuesta;
}

char *opDROP(int socketReceptor, char* unNombreTabla)
{
	int protocoloID = 21;
	char *dropSerializado;
	t_DROP *unDROP = definirT_DROP(unNombreTabla);

	printf("\nQuery:\n	DROP [%s]\n",unDROP->nombreTabla);

	dropSerializado = serializarT_DROP(unDROP);

	//printf("empaquetarEnviarMensaje():\n");///

	//ESTO VARIA SEGUN LA STRUCTURA Q PIENSO MANDAR, hay q hacerlo a mano
	int tamanioStructSerializado = (sizeof(uint32_t)) + unDROP->longNombre;

	empaquetarEnviarMensaje(socketReceptor,protocoloID,tamanioStructSerializado,dropSerializado);

	free(dropSerializado);
	freeT_DROP(unDROP);

	//#####################################
	t_PaqueteDeDatos *packageRecibido = recibirPaquete(socketReceptor);
	printf("fd: %d\n",socketReceptor); ///
	printf("package->ID: %d\n",packageRecibido->ID); ///

	if(packageRecibido->ID == 0){
		return "MEMORIA_DESCONECTADA";
	}

	char *respuesta = deserializarRespuesta(packageRecibido->Datos);
	freePackage(packageRecibido);

	return respuesta;
}
char* opJOURNAL(int socketReceptor){

}
