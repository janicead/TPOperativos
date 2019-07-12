#ifndef LFSVARIOS_H_
#define LFSVARIOS_H_

//#include <commons/string.h>
#include <stdio.h>
#include "lfsSerializacion.h"
#include "sockets.h"
#include "usoVariado.h"

char **splitDeOperaciones(char *unaLineaReadline);

void enviarRespuesta(int socketReceptor, int protocoloID, char *respuesta);

char *respuestaDESCRIBEaPrintear(char *unaRespDESCRIBE);

#endif /* LFSVARIOS_H_ */
