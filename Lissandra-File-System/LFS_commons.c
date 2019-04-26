#include "LFS_commons.h"

void configure_logger_lfs(){
	loggerLFS = log_create("lfs.log","lfs",1,LOG_LEVEL_INFO);
}

void exit_gracefully(int exitInfo){
	log_destroy(loggerLFS);
	config_destroy(archivoConfigLFS);
	exit(exitInfo);
}
