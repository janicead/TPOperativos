#include "kernel_commons.h"

void configure_logger_kernel(){
	loggerKernel = log_create("kernel.log","kernel",1,LOG_LEVEL_INFO);
}

void exit_gracefully(int exitInfo){
	log_destroy(loggerKernel);
	config_destroy(archivoConfigKernel);
	exit(exitInfo);
}
