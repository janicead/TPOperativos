#ifndef KERNEL_COMMONS_H_
#define KERNEL_COMMONS_H_

#include <commons/log.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>

t_log* loggerKernel;
t_config* archivoConfigKernel;

void configure_logger_kernel();
void exit_gracefully(int exitInfo);

#endif /* KERNEL_COMMONS_H_ */
