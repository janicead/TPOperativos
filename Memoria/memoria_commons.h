#ifndef LFS_COMMONS_H_
#define LFS_COMMONS_H_

#include <commons/log.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>

t_log* loggerMemoria;
t_config* archivoConfigMemoria;

void configure_logger_memoria();
void exit_gracefully(int exitInfo);

#endif /* LFS_COMMONS_H_ */
