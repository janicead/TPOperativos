#ifndef LFS_COMMONS_H_
#define LFS_COMMONS_H_

#include <commons/log.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>

t_log* loggerLFS;
t_config* archivoConfigLFS;

void configure_logger_lfs();
void exit_gracefully(int exitInfo);

#endif /* LFS_COMMONS_H_ */
