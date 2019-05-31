#include "kernel_commons.h"

void configure_logger_kernel(){
	loggerKernel = log_create("kernel.log","kernel",1,LOG_LEVEL_INFO);
	return;
}

void exit_gracefully(int exitInfo){
	log_destroy(loggerKernel);
	config_destroy(archivoConfigKernel);
	//free_memoria(strong_consistency);
	free(puertoMemoria);
	destruir_colas();
	destruir_listas();
	exit(exitInfo);
}

void inicializarIds(){
	idLCB = 0;
	idMEM = 0;
	return;
}

int getID(int id){
	if(id == 0){
		int aux = id;
		id++;
		return aux;
	}
	return ++id;
}

void crear_listas(){
	tablas = list_create();
	strong_hash_consistency = list_create();
	eventual_consistency = list_create();
	memorias = list_create();
	return;
}

void crear_colas(){
	queue_ready = queue_create();
	queue_exit = queue_create();
	return;
}

void agregar_memoria(int socket){
	t_memoria* memoria = (t_memoria*)malloc(sizeof(t_memoria));
	memoria->id_mem = idMEM;
	idMEM++;
	memoria->socket_mem = socket;
	list_add(memorias,memoria);
	return;
}

t_lcb* crear_lcb(){
	t_lcb* new_lcb = (t_lcb*)malloc(sizeof(t_lcb));
	new_lcb->id_lcb = idLCB;
	idLCB++;
	new_lcb->estado = NEW;
	new_lcb->program_counter = 0;
	new_lcb->operaciones = list_create();
	return new_lcb;
}

void status_lcb(t_lcb* lcb){
	printf("ID: %d\n",lcb->id_lcb);
	printf("Program Counter: %d\n", lcb->program_counter);
	printf("Cantidad operaciones: %d\n",list_size(lcb->operaciones));
	return;
}

void pasar_lcb_a_ready(t_lcb* lcb){
	lcb->estado = READY;
	queue_push(queue_ready,lcb);
	return;
}

void pasar_lcb_a_exit(t_lcb* lcb){
	lcb->estado = EXIT;
	queue_push(queue_exit,lcb);
	return;
}

t_LQL_operacion* obtener_op_actual(t_lcb* lcb){
	return list_get(lcb->operaciones,lcb->program_counter);
}

void agregar_op_lcb(t_lcb* lcb,t_LQL_operacion* op){
	list_add(lcb->operaciones,op);
	return;
}

void agregar_tabla(t_tabla* tabla){
	list_add(tablas,tabla);
	return;
}

void free_lcb(t_lcb* lcb){
	list_destroy_and_destroy_elements(lcb->operaciones,(void*) destruir_operacion);
	free(lcb);
	return;
}

void destruir_operacion(t_LQL_operacion* op){
	if(op->_raw){
		string_iterate_lines(op->_raw, (void*) free);
		free(op->_raw);
		free(op);
	}
	else if(op->_raw == NULL){
		free(op);
	}
	return;
}

void destruir_listas(){
	list_destroy_and_destroy_elements(tablas,(void*) free_tabla);
	list_destroy_and_destroy_elements(memorias,(void*) free_memoria);
	list_destroy(strong_hash_consistency);
	list_destroy(eventual_consistency);
	return;
}

void destruir_colas(){
	queue_destroy_and_destroy_elements(queue_ready,(void*) free_lcb);
	queue_destroy_and_destroy_elements(queue_exit,(void*) free_lcb);
	return;
}

void free_tabla(t_tabla* tabla){
	free(tabla);
	return;
}

void free_memoria(t_memoria* memoria){
	free(memoria);
	return;
}

