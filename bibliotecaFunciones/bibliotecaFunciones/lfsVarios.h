#ifndef LFSVARIOS_H_
#define LFSVARIOS_H_

#include <stdio.h>
#include "lfsSerializacion.h"
#include "sockets.h"

char **splitDeOperaciones(char *unaLineaReadline);

void enviarRespuesta(int socketReceptor, int protocoloID, char *respuesta);



#endif /* LFSVARIOS_H_ */
