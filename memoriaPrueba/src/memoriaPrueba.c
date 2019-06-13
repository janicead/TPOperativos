#include "memoriaPrueba.h"

int main() {
	//t_PaqueteDeDatos *packageRecibido;

  configure_logger();
  int socketServer = connect_to_server(IP, PUERTO);

  log_info(logger,"Realizando respectivo handshake");

//##########################
  //char msjEsperado[] = "aca LISSANDRA FS";


  char *msjEnviado = string_from_format("Memoria %d",1); //ACA EN VEZ DEL 1, IRIA EL NrO Q TIENE LA MEMORIA
  realizarHandshakeAlLFS(logger,socketServer,msjEnviado);

  printf("\n-----------------------------------------------");

//########## OPERACIONES MEMORIA-> LFS ######################

  char unNombreTabla[] = "TaBla1";
  int unaKey = 23;
  while(1){
  char *respuestaSELECT = opSELECT(socketServer,unNombreTabla,unaKey);
  printf("RespuestaSELECT: [%s]\n",respuestaSELECT);
  printf("\n-----------------------------------------------");
  //sleep(1);
  }
//-----------------------------------------------------------------
  /*
  char unNombreTabla2[] = "TaBla2";
  int unaKey2 = 23;
  char unValue2[] = "valor123";
  int unTimestamp2 = 666;
  /*while(1){
  char *respuestaINSERT = opINSERT(socketServer,unNombreTabla2,unaKey2,unValue2,unTimestamp2);
  printf("RespuestaINSERT: [%s]\n",respuestaINSERT);
  printf("\n-----------------------------------------------");
  //sleep(1);
  }*/

//-----------------------------------------------------------------
 /*
  char unNombreTabla3[] = "Ta3";
  char unaConsistencia3[] = "SC";
  int unNParticiones3 = 3;
  int unTiempoCompactacion3 = 60000;

  char *respuestaCREATE = opCREATE(socketServer,unNombreTabla3,unaConsistencia3,unNParticiones3,unTiempoCompactacion3);
  printf("RespuestaCREATE: [%s]\n",respuestaCREATE);
  printf("\n-----------------------------------------------");

//-----------------------------------------------------------------
  char unNombreTabla4[] = "TablaDES";

  char *respuestaDESCRIBE = opDESCRIBE(socketServer,unNombreTabla4);
  printf("RespuestaDESCRIBE: [%s]\n",respuestaDESCRIBE);
  printf("\n-----------------------------------------------");

//-----------------------------------------------------------------
  char unNombreTabla5[] = "TablaDROP";

  char *respuestaDROP = opDROP(socketServer,unNombreTabla5);
  printf("RespuestaDROP: [%s]\n",respuestaDROP);
  printf("\n-----------------------------------------------");
*/

  printf("\n");

//#########################################################

 log_destroy(logger);
  exit(0);
}

void configure_logger() {

   logger = log_create("tp0.log", "tp0", 1,LOG_LEVEL_INFO);
}

int connect_to_server(char * ip, char * port) {
  struct addrinfo hints;
  struct addrinfo *server_info;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
  hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP

  getaddrinfo(ip, port, &hints, &server_info);
  int server_socket = socket( server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol );

  int retorno = connect(server_socket,server_info->ai_addr,server_info->ai_addrlen);

  freeaddrinfo(server_info);  // No lo necesitamos mas


  if (retorno==-1)
  {
	  log_error(logger, "No se pudo conectar con el Servidor");
	  exit(0);
  }


  log_info(logger, "Conectado!");
  return server_socket;
}

/*
void realizarHandshakeAlLFS(int socketServer, char* msjEnviado)
{
	int protocoloID = 11; // 11: Handshake

	printf("Enviando '%s'...\n",msjEnviado);
	empaquetarEnviarMensaje(socketServer,protocoloID,strlen(msjEnviado),msjEnviado);
	//printf("\nPAQUETE ENVIADO\n");

	t_PaqueteDeDatos *package;
	//package->Datos = "";
	package = recibirPaquete(socketServer);

	if(package->ID != 12) //12: Respuesta del hancshake enviado
	{
		log_error(logger,"Protocolo respuesta handshake, distinto del esperado   ID:%d",package->ID);
	}
	else
	{
		printf("LISSANDRA: Tamanio de pagina = %d\n\n",atoi(package->Datos)); ///
		log_info(logger,"Handshake  con socket %d: DONE!", socketServer);


	}

	freePackage(package);

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

	char *respuesta = deserializarRespuesta(packageRecibido->Datos);
	freePackage(packageRecibido);

	return respuesta;
}

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

	char *respuesta = deserializarRespuesta(packageRecibido->Datos);
	freePackage(packageRecibido);

	return respuesta;
}
*/

