#include "kernel_commons.h"

void configure_logger_kernel(){
	loggerKernel = log_create("kernel.log","kernel",1,LOG_LEVEL_INFO);
	return;
}

void exit_gracefully(int exitInfo){
	log_destroy(loggerKernel);
	config_destroy(archivoConfigKernel);
	exit(exitInfo);
}

void inicializarId(){
	id = 0;
	return;
}

void crear_colas(){
	queue_new = queue_create();
	queue_ready = queue_create();
	queue_exit = queue_create();
	return;
}

t_lcb* crear_lcb(){
	t_lcb* new_lcb = (t_lcb*)malloc(sizeof(t_lcb));
	new_lcb->id_lcb = id++;
	new_lcb->estado = NEW;
	new_lcb->program_counter = 0;
	new_lcb->operaciones = list_create();
	return new_lcb;
}

void free_lcb(t_lcb* lcb){
	if(!list_is_empty(lcb->operaciones)){
		list_destroy_and_destroy_elements(lcb->operaciones,(void*) destruir_operacion);
	}
	free(lcb);
	return;
}

void destruir_operacion(t_LQL_operacion* op){
	if(op->_raw){
		string_iterate_lines(op->_raw, (void*) free);
		free(op->_raw);
	}
	return;
}
