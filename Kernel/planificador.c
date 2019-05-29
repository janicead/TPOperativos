#include "planificador.h"

void ejecutar(){
	if(!queue_is_empty(queue_ready)){// luego irá un semáforo
		t_lcb* lcb = queue_pop(queue_ready);
		lcb->estado = EXEC;
		int quantum = configKernel.quantum;
		while(quantum > 0 && lcb->program_counter < list_size(lcb->operaciones)){
			t_LQL_operacion* operacion = obtener_op_actual(lcb);
			switch(operacion->keyword){
				case SELECT:
					lql_select(operacion);
					break;
				case INSERT:
					lql_insert(operacion);
					break;
				case CREATE:
					lql_create(operacion);
					break;
				case DESCRIBE:
					lql_describe(operacion);
					break;
				case DROP:
					lql_drop(operacion);
					break;
				case JOURNAL:
					lql_journal(memorias);
					break;
				case ADD:
					lql_add(operacion);
					break;
				case RUN:
					lql_run(abrirArchivo(operacion->argumentos.RUN.path));
					break;
				case METRICS:
					lql_metrics();
					break;
			}
			lcb->program_counter++;
			quantum--;
		}
		if(lcb->program_counter >= list_size(lcb->operaciones)){
			pasar_lcb_a_exit(lcb);
		}
		else{
			pasar_lcb_a_ready(lcb);
		}
	}
}


FILE* abrirArchivo(char* path){
	FILE* f = fopen(path,"r");
	if(f == NULL){
		log_error(loggerKernel,"Error al abrir el archivo LQL");
		return f;
	}
	return f;
}

void lql_select(t_LQL_operacion* operacion){
	t_tabla* tabla = devuelve_tabla(operacion->argumentos.SELECT.nombre_tabla);
	if(tabla == NULL){
		log_error(loggerKernel,"La tabla de nombre:  %s no existe",operacion->argumentos.SELECT.nombre_tabla);
		free_tabla(tabla);
		return;
	}
	t_memoria* memoria = obtener_memoria_consistencia(tabla->consistencia);
	if(!memoria->valida){
		log_error(loggerKernel,"No hay memoria para el criterio: %s de la tabla: %s",tabla->consistencia,operacion->argumentos.SELECT.nombre_tabla);
		return;
	}
	puts("SELECT OK");
	//enviar paquete a la memoria seleccinada con los datos
	return;
}

void lql_insert(t_LQL_operacion* op){
	t_tabla* tabla = devuelve_tabla(op->argumentos.INSERT.nombre_tabla);
	if(tabla == NULL){
		log_error(loggerKernel,"La tabla de nombre:  %s no existe",op->argumentos.INSERT.nombre_tabla);
		free_tabla(tabla);
		return;
	}
	t_memoria* memoria = obtener_memoria_consistencia(tabla->consistencia);
	if(!memoria->valida){
		log_error(loggerKernel,"No hay memoria para el criterio: %s de la tabla: %s",tabla->consistencia,op->argumentos.INSERT.nombre_tabla);
		return;
	}
	puts("INSERT OK");
	return;
}

void lql_create(t_LQL_operacion* op){
	puts("CREATE OK");
	return;
}

void lql_describe(t_LQL_operacion* op){
	if(string_is_empty(op->argumentos.DESCRIBE.nombre_tabla)){
		puts("DESCRIBE TOTAL OK");
	}
	else{
		puts("DESCRIBE OK");
	}
}

void lql_drop(t_LQL_operacion* op){
	t_tabla* tabla = devuelve_tabla(op->argumentos.DROP.nombre_tabla);
	if(tabla == NULL){
		log_error(loggerKernel,"La tabla de nombre:  %s no existe",op->argumentos.DROP.nombre_tabla);
		free_tabla(tabla);
		return;
	}
	t_memoria* memoria = obtener_memoria_consistencia(tabla->consistencia);
	if(!memoria->valida){
		log_error(loggerKernel,"No hay memoria para el criterio: %s de la tabla: %s",tabla->consistencia,op->argumentos.DROP.nombre_tabla);
		return;
	}
	puts("DROP OK");
	return;
}

void lql_journal(t_list* list_mem){
	for(int i = 0; i < list_size(list_mem); i++){
		//t_memoria* mem = list_get(list_mem,i);
		puts("JOURNAL OK");
	}
	return;
}

void lql_add(t_LQL_operacion* op){
	if(string_equals_ignore_case(op->argumentos.ADD.criterio,"SC")){
		if(strong_consistency == NULL){
			strong_consistency = obtener_memoria_por_id(op->argumentos.ADD.nro_memoria);
			log_info(loggerKernel,"Se ha asignado la memoria %d al criterio Strong Consistency",strong_consistency->id_mem);
			return;
		}
		else{
			log_error(loggerKernel,"El criterio Strong Consistency ya posee una memoria asociada");
			return;
		}
	}
	if(string_equals_ignore_case(op->argumentos.ADD.criterio,"SHC")){
		if(memoria_existente(strong_hash_consistency,op->argumentos.ADD.nro_memoria)){
			log_error(loggerKernel,"La memoria %d ya se encuentra asignada al criterio Strong Hash Consistency",op->argumentos.ADD.nro_memoria);
			return;
		}
		else{
			lql_journal(strong_hash_consistency);
			list_add(strong_hash_consistency,obtener_memoria_por_id(op->argumentos.ADD.nro_memoria));
			log_info(loggerKernel,"Se ha asignado la memoria %d al criterio Strong Hash Consistency",op->argumentos.ADD.nro_memoria);
			return;
		}
	}
	if(string_equals_ignore_case(op->argumentos.ADD.criterio,"EV")){
		if(memoria_existente(eventual_consistency,op->argumentos.ADD.nro_memoria)){
			log_error(loggerKernel,"La memoria %d ya se encuentra asignada al criterio Eventual Consistency",op->argumentos.ADD.nro_memoria);
			return;
		}
		else{
			list_add(eventual_consistency,obtener_memoria_por_id(op->argumentos.ADD.nro_memoria));
			log_info(loggerKernel,"Se ha asignado la memoria %d al criterio Eventual Consistency",op->argumentos.ADD.nro_memoria);
			return;
		}
	}
	return;
}

void lql_run(FILE* archivo){
	if(archivo == NULL){
		return;
	}
	log_info(loggerKernel,"Iniciando ejecución de archivo LQL");
	char* linea = NULL;
	size_t len = 0;
	t_lcb* lcb = crear_lcb();
	 while ((getline(&linea, &len, archivo)) != -1) {
	        t_LQL_operacion* operacion = parse(linea);
	        if(operacion->valido){
	        	agregar_op_lcb(lcb,operacion);
	        }
	        else {
	            log_error(loggerKernel, "La linea %s no es valida", linea);
	            exit(EXIT_FAILURE);
	        }
	}
	if(!list_is_empty(lcb->operaciones)){
		pasar_lcb_a_ready(lcb);
	}
	fclose(archivo);
	if (linea){
		free(linea);
	}
}

void lql_metrics(){
	puts("Metricas en desarrollo");
	return;
}

t_tabla* devuelve_tabla(char* nombre){

	bool same_table(t_tabla* table){
		return string_equals_ignore_case(table->nombre_tabla,nombre);
	}

	return list_find(tablas,(void*) same_table);
}

t_memoria* obtener_memoria_consistencia(char* consistencia){
	t_memoria* mem = (t_memoria*)malloc(sizeof(t_memoria));

	if(string_equals_ignore_case("SC",consistencia)){
		free_memoria(mem);
		return strong_consistency;
	}
	else if(string_equals_ignore_case("SHC",consistencia)){
		if(!list_is_empty(strong_hash_consistency)){
			mem = list_get(strong_hash_consistency,0); // SOLO TEMPORAL, FUNCION DE HASH
			mem->valida = true;
			return mem;
		}
	}
	else if(string_equals_ignore_case("CEC",consistencia)){
		if(!list_is_empty(eventual_consistency)){
			mem = list_get(eventual_consistency,0);
			mem->valida = true;
			return mem;
		}
	}
	mem->valida = false;
	return mem;
}

t_memoria* obtener_memoria_por_id(int id){
	bool same_id(t_memoria* mem){
		return mem->id_mem == id;
	}

	return list_find(memorias,(void*) same_id);
}

bool memoria_existente(t_list* l_memorias,int id){
	bool same_id(t_memoria* mem){
		return mem->id_mem == id;
	}

	return list_any_satisfy(l_memorias,(void*) same_id);
}


