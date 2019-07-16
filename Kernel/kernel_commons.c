#include "kernel_commons.h"

void configure_logger_kernel(){
	loggerKernel = log_create("kernel.log","kernel",1,LOG_LEVEL_INFO);
	loggerKernelConsola = log_create("kernelConsola.log","kernelConsola",1,LOG_LEVEL_INFO);
	return;
}

void exit_gracefully(int exitInfo){
	pthread_mutex_lock(&log_sem);
	log_destroy(loggerKernel);
	pthread_mutex_unlock(&log_sem);
	log_destroy(loggerKernelConsola);
	free(puertoMemoria);
	destruir_colas();
	destruir_listas();
	destruir_semaforos();
	free(hilos);
	exit(exitInfo);
}

void inicializarIds(){
	idLCB = 0;
	return;
}

void crear_listas(){
	tablas = list_create();
	strong_hash_consistency = list_create();
	eventual_consistency = list_create();
	memorias = list_create();
	hilos_ejec = list_create();
	inserts_ejecutados = list_create();
	selects_ejecutados = list_create();
	return;
}

void crear_colas(){
	queue_ready = queue_create();
	queue_exit = queue_create();
	return;
}

void iniciar_semaforos(){
	pthread_mutex_init(&queue_ready_sem,NULL);
	pthread_mutex_init(&queue_exit_sem,NULL);
	pthread_mutex_init(&config_sem,NULL);
	pthread_mutex_init(&memorias_sem,NULL);
	pthread_mutex_init(&strong_consistency_sem,NULL);
	pthread_mutex_init(&strong_hash_consistency_sem,NULL);
	pthread_mutex_init(&eventual_consistency_sem,NULL);
	pthread_mutex_init(&tablas_sem,NULL);
	pthread_mutex_init(&log_sem,NULL);
	pthread_mutex_init(&inserts_ejecutados_sem,NULL);
	pthread_mutex_init(&selects_ejecutados_sem,NULL);
	sem_init(&execute_sem,0,0);
	return;
}

void agregar_memoria(int puerto, char* ip, int nro_memoria){
	bool sameID(t_memoria* mem){
		return mem->id_mem == nro_memoria;
	}

	t_memoria* memoria = (t_memoria*)malloc(sizeof(t_memoria));
	memoria->id_mem = nro_memoria;
	memoria->puerto = puerto;
	memoria->ip = malloc(1+strlen(ip));
	strcpy(memoria->ip,ip);
	memoria->valida = true;
	memoria->cant_selects_inserts_ejecutados = 0;
	pthread_mutex_lock(&memorias_sem);
	if(!list_any_satisfy(memorias,(void*) sameID)){
		list_add(memorias,memoria);
	}
	else{
		free_memoria(memoria);
	}
	pthread_mutex_unlock(&memorias_sem);
	return;
}

bool memoria_existente(t_list* l_memorias,int id){
	bool same_id(t_memoria* mem){
		return mem->id_mem == id;
	}

	return list_any_satisfy(l_memorias,(void*) same_id);
}

void agregar_socket_mem(int nro_memoria, int socket){
	bool sameID(t_memoria* mem){
		return mem->id_mem == nro_memoria;
	}
	pthread_mutex_lock(&memorias_sem);
	t_memoria* memoria = list_find(memorias,(void*)sameID);
	memoria->socket_mem = socket;
	pthread_mutex_unlock(&memorias_sem);
}

void sacar_memoria(int nro_memoria){
	bool sameID(t_memoria* mem){
		return mem->id_mem == nro_memoria;
	}
	bool sameNroMem(t_memoriaTablaDeGossip* mem){
		return mem->numeroDeMemoria == nro_memoria;
	}
	pthread_mutex_lock(&strong_consistency_sem);
	if(strong_consistency != NULL){
		if(strong_consistency->id_mem == nro_memoria){
			strong_consistency = NULL;
		}
	}
	pthread_mutex_unlock(&strong_consistency_sem);
	pthread_mutex_lock(&strong_hash_consistency_sem);
	list_remove_by_condition(strong_hash_consistency,(void*)sameID);
	pthread_mutex_unlock(&strong_hash_consistency_sem);
	pthread_mutex_lock(&eventual_consistency_sem);
	list_remove_by_condition(eventual_consistency,(void*)sameID);
	pthread_mutex_unlock(&eventual_consistency_sem);
	pthread_mutex_lock(&memorias_sem);
	list_remove_and_destroy_by_condition(memorias,(void*)sameID,(void*)free_memoria);
	pthread_mutex_unlock(&memorias_sem);

	list_remove_and_destroy_by_condition(memoriasALasQueMeConecte,(void*)sameNroMem, (void*)free_memoria_gossip);
	printf("Aca borre la memoria %d porque se desconecto\n", nro_memoria);
	puts("MEMORIAS A LAS QUE ME CONECTE \n");
	mostrarmeMemoriasTablaGossip(memoriasALasQueMeConecte);
	puts("MEMORIAS EN MI TABLA DE GOSSIP \n");
	mostrarmeMemoriasTablaGossip(tablaDeGossipKernel);

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
	pthread_mutex_lock(&queue_ready_sem);
	queue_push(queue_ready,lcb);
	pthread_mutex_unlock(&queue_ready_sem);
	sem_post(&execute_sem);
	return;
}

void pasar_lcb_a_exit(t_lcb* lcb){
	lcb->estado = EXIT;
	pthread_mutex_lock(&queue_exit_sem);
	queue_push(queue_exit,lcb);
	pthread_mutex_unlock(&queue_exit_sem);
	log_info(loggerKernel,"LCB %d agregado a la cola de exit.",lcb->id_lcb);
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
	pthread_mutex_lock(&tablas_sem);
	list_add(tablas,tabla);
	pthread_mutex_unlock(&tablas_sem);
	return;
}

bool validar_consistencia(char* consistencia){
	return string_equals_ignore_case(consistencia,"sc") || string_equals_ignore_case(consistencia,"shc") || string_equals_ignore_case(consistencia,"ec");
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
	pthread_mutex_lock(&tablas_sem);
	list_destroy_and_destroy_elements(tablas,(void*) free_tabla);
	pthread_mutex_unlock(&tablas_sem);
	pthread_mutex_lock(&memorias_sem);
	list_destroy_and_destroy_elements(memorias,(void*) free_memoria);
	pthread_mutex_unlock(&memorias_sem);
	pthread_mutex_lock(&strong_hash_consistency_sem);
	list_destroy(strong_hash_consistency);
	pthread_mutex_unlock(&strong_hash_consistency_sem);
	pthread_mutex_lock(&eventual_consistency_sem);
	list_destroy(eventual_consistency);
	pthread_mutex_unlock(&eventual_consistency_sem);
	list_destroy_and_destroy_elements(hilos_ejec,(void*) free);
	pthread_mutex_lock(&inserts_ejecutados_sem);
	list_destroy_and_destroy_elements(inserts_ejecutados,(void*) free);
	pthread_mutex_unlock(&inserts_ejecutados_sem);
	pthread_mutex_lock(&selects_ejecutados_sem);
	list_destroy_and_destroy_elements(selects_ejecutados,(void*) free);
	pthread_mutex_unlock(&selects_ejecutados_sem);
	return;
}

void destruir_colas(){
	pthread_mutex_lock(&queue_ready_sem);
	queue_destroy_and_destroy_elements(queue_ready,(void*) free_lcb);
	pthread_mutex_unlock(&queue_ready_sem);
	pthread_mutex_lock(&queue_exit_sem);
	queue_destroy_and_destroy_elements(queue_exit,(void*) free_lcb);
	pthread_mutex_unlock(&queue_exit_sem);
	return;
}

void destruir_semaforos(){
	pthread_mutex_destroy(&queue_ready_sem);
	pthread_mutex_destroy(&queue_exit_sem);
	pthread_mutex_destroy(&config_sem);
	pthread_mutex_destroy(&memorias_sem);
	pthread_mutex_destroy(&strong_consistency_sem);
	pthread_mutex_destroy(&strong_hash_consistency_sem);
	pthread_mutex_destroy(&eventual_consistency_sem);
	pthread_mutex_destroy(&tablas_sem);
	pthread_mutex_destroy(&selects_ejecutados_sem);
	pthread_mutex_destroy(&inserts_ejecutados_sem);
	sem_destroy(&execute_sem);
	return;
}

void free_tabla(t_tabla* tabla){
	free(tabla);
	return;
}

void free_memoria(t_memoria* memoria){
	if(memoria->valida){
		free(memoria->ip);
	}
	free(memoria);
	return;
}

void free_memoria_gossip(t_memoriaTablaDeGossip* memoria){
	free(memoria->ip);
	free(memoria);
}
