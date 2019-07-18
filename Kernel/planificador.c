#include "planificador.h"

void* ejecutar(){
	while(1){
		sem_wait(&execute_sem);
		pthread_mutex_lock(&config_sem);
		int quantum = configKernel.quantum;
		int sleep_time = configKernel.sleep_execution;
		pthread_mutex_unlock(&config_sem);
		pthread_mutex_lock(&queue_ready_sem);
		t_lcb* lcb = queue_pop(queue_ready);
		pthread_mutex_unlock(&queue_ready_sem);
		lcb->estado = EXEC;
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
					pthread_mutex_lock(&memorias_sem);
					lql_journal(memorias, operacion);
					pthread_mutex_unlock(&memorias_sem);
					break;
				case ADD:
					lql_add(operacion);
					break;
				case RUN:
					lql_run(abrirArchivo(operacion->argumentos.RUN.path), operacion);
					break;
				case METRICS:
					lql_metrics(true);
					operacion->success = true;
					break;
			}
			sleep(sleep_time);
			if(!operacion->success){
				lcb->abortar = true;
				break;
			}
			lcb->program_counter++;
			lcb->abortar = false;
			quantum--;
		}
		if(lcb->program_counter >= list_size(lcb->operaciones) || lcb->abortar){
			pasar_lcb_a_exit(lcb);
		}
		else{
			pasar_lcb_a_ready(lcb);
		}
	}
	return NULL;
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
	time_t tiempo_inicio = time(NULL);
	pthread_mutex_lock(&tablas_sem);
	t_tabla* tabla = devuelve_tabla(operacion->argumentos.SELECT.nombre_tabla);
	pthread_mutex_unlock(&tablas_sem);
	if(tabla == NULL){
		if(operacion->consola){
			printf("ERROR: La tabla de nombre:  %s no existe.\n",operacion->argumentos.SELECT.nombre_tabla);
		}
		log_error(loggerKernel,"La tabla de nombre:  %s no existe",operacion->argumentos.SELECT.nombre_tabla);
		free_tabla(tabla);
		operacion->success = false;
		return;
	}
	t_memoria* memoria = obtener_memoria_consistencia(tabla->consistencia,operacion->argumentos.SELECT.key);
	if(!memoria->valida){
		printf("ERROR: No hay memoria para el criterio: %s de la tabla: %s.\n",tabla->consistencia,operacion->argumentos.SELECT.nombre_tabla);
		log_error(loggerKernel,"No hay memoria para el criterio: %s de la tabla: %s",tabla->consistencia,operacion->argumentos.SELECT.nombre_tabla);
		free_memoria(memoria);
		operacion->success = false;
		return;
	}
	pthread_mutex_lock(&(memoria->socket_mem_sem));
	char* respuesta = opSELECT(memoria->socket_mem,operacion->argumentos.SELECT.nombre_tabla, operacion->argumentos.SELECT.key);

	if(verificar_memoria_caida(respuesta,operacion,memoria->id_mem)){
		return;
	}
	pthread_mutex_unlock(&(memoria->socket_mem_sem));
	if(operacion->consola){
		if(string_equals_ignore_case(respuesta,"NO_EXISTE_TABLA")){
			printf("ERROR: No se pudo realizar SELECT %s %d ya que la tabla %s no existe.\n", operacion->argumentos.SELECT.nombre_tabla,operacion->argumentos.SELECT.key,operacion->argumentos.SELECT.nombre_tabla);
			log_error(loggerKernel, "No se realizar SELECT %s %d ya que la tabla %s no existe", operacion->argumentos.SELECT.nombre_tabla,operacion->argumentos.SELECT.key,operacion->argumentos.SELECT.nombre_tabla);
			operacion->success = false;
			free(respuesta);
			return;
		}
		if(string_equals_ignore_case(respuesta,"NO_EXISTE_VALUE")){
			printf("ERROR: No se pudo realizar SELECT %s %d ya que la key %d no existe.\n", operacion->argumentos.SELECT.nombre_tabla,operacion->argumentos.SELECT.key,operacion->argumentos.SELECT.key);
			log_error(loggerKernel, "No se realizar SELECT %s %d ya que la key %d no existe", operacion->argumentos.SELECT.nombre_tabla,operacion->argumentos.SELECT.key,operacion->argumentos.SELECT.key);
			operacion->success = false;
			free(respuesta);
			return;
		}
		printf("SELECT %s %d Value -> %s \n",operacion->argumentos.SELECT.nombre_tabla,operacion->argumentos.SELECT.key,respuesta);
		log_info(loggerKernel,"SELECT %s %d Value -> %s",operacion->argumentos.SELECT.nombre_tabla,operacion->argumentos.SELECT.key,respuesta);
	}
	else{
		if(string_equals_ignore_case(respuesta,"NO_EXISTE_TABLA")){
			log_error(loggerKernel, "No se pudo realizar SELECT %s %d ya que la tabla %s no existe", operacion->argumentos.SELECT.nombre_tabla,operacion->argumentos.SELECT.key,operacion->argumentos.SELECT.nombre_tabla);
			operacion->success = false;
			free(respuesta);
			return;
		}
		if(string_equals_ignore_case(respuesta,"NO_EXISTE_VALUE")){
			log_error(loggerKernel, "No se pudo realizar SELECT %s %d ya que la key %d no existe", operacion->argumentos.SELECT.nombre_tabla,operacion->argumentos.SELECT.key,operacion->argumentos.SELECT.key);
			operacion->success = false;
			free(respuesta);
			return;
		}
		log_info(loggerKernel,"SELECT %s %d Value -> %s",operacion->argumentos.SELECT.nombre_tabla,operacion->argumentos.SELECT.key,respuesta);
	}
	time_t tiempo_fin = time(NULL);
	t_select_ejecutado* select = (t_select_ejecutado*)malloc(sizeof(t_select_ejecutado));
	select->tiempo_fin = tiempo_fin;
	select->tiempo_inicio = tiempo_inicio;
	pthread_mutex_lock(&selects_ejecutados_sem);
	list_add(selects_ejecutados,select);
	pthread_mutex_unlock(&selects_ejecutados_sem);
	operacion->success = true;
	pthread_mutex_lock(&memorias_sem);
	memoria->cant_selects_inserts_ejecutados++;
	pthread_mutex_unlock(&memorias_sem);
	free(respuesta);
	return;
}

void lql_insert(t_LQL_operacion* op){
	time_t tiempo_inicio = time(NULL);
	pthread_mutex_lock(&tablas_sem);
	t_tabla* tabla = devuelve_tabla(op->argumentos.INSERT.nombre_tabla);
	pthread_mutex_unlock(&tablas_sem);
	if(tabla == NULL){
		if(op->consola){
			printf("ERROR: La tabla de nombre:  %s no existe.\n",op->argumentos.INSERT.nombre_tabla);
		}
		log_error(loggerKernel,"La tabla de nombre:  %s no existe",op->argumentos.INSERT.nombre_tabla);
		free_tabla(tabla);
		op->success = false;
		return;
	}
	t_memoria* memoria = obtener_memoria_consistencia(tabla->consistencia,op->argumentos.INSERT.key);
	if(!memoria->valida){
		if(op->consola){
			printf("ERROR: No hay memoria para el criterio: %s de la tabla: %s.\n",tabla->consistencia,op->argumentos.INSERT.nombre_tabla);
		}
		log_error(loggerKernel,"No hay memoria para el criterio: %s de la tabla: %s",tabla->consistencia,op->argumentos.INSERT.nombre_tabla);
		free_memoria(memoria);
		op->success = false;
		return;
	}
	pthread_mutex_lock(&(memoria->socket_mem_sem));
	unsigned long int timestamp = obtenerTimeStamp();
	char* resp = opINSERT(memoria->socket_mem, op->argumentos.INSERT.nombre_tabla, op->argumentos.INSERT.key,op->argumentos.INSERT.valor,timestamp);
	if(verificar_memoria_caida(resp,op,memoria->id_mem)){
		return;
	}
	pthread_mutex_unlock(&(memoria->socket_mem_sem));
	if(op->consola){
		if(string_equals_ignore_case(resp,"NO_EXISTE_TABLA")){
			printf("ERROR: No se pudo realizar: INSERT %s %d %s ya que la tabla %s no existe.\n", op->argumentos.INSERT.nombre_tabla, op->argumentos.INSERT.key, op->argumentos.INSERT.valor, op->argumentos.INSERT.nombre_tabla);
			log_error(loggerKernel, "No se pudo realizar: INSERT %s %d %s ya que la tabla %s no existe.", op->argumentos.INSERT.nombre_tabla, op->argumentos.INSERT.key, op->argumentos.INSERT.valor, op->argumentos.INSERT.nombre_tabla);
			op->success = false;
			free(resp);
			return;
		}
		else{
			printf("INSERT %s %d %s realizado correctamente.\n", op->argumentos.INSERT.nombre_tabla, op->argumentos.INSERT.key, op->argumentos.INSERT.valor);
			log_info(loggerKernel, "INSERT %s %d %s realizado correctamente.", op->argumentos.INSERT.nombre_tabla, op->argumentos.INSERT.key, op->argumentos.INSERT.valor);
		}
	}
	else{
		if(string_equals_ignore_case(resp,"NO_EXISTE_TABLA")){
			log_error(loggerKernel, "No se pudo realizar: INSERT %s %d %s ya que la tabla %s no existe.", op->argumentos.INSERT.nombre_tabla, op->argumentos.INSERT.key, op->argumentos.INSERT.valor, op->argumentos.INSERT.nombre_tabla);
			op->success = false;
			free(resp);
			return;
		}
		else{
			log_info(loggerKernel, "INSERT %s %d %s realizado correctamente.", op->argumentos.INSERT.nombre_tabla, op->argumentos.INSERT.key, op->argumentos.INSERT.valor);
		}
	}
	time_t tiempo_fin = time(NULL);
	t_insert_ejecutado* insert = (t_insert_ejecutado*)malloc(sizeof(t_insert_ejecutado));
	insert->tiempo_fin = tiempo_fin;
	insert->tiempo_inicio = tiempo_inicio;
	pthread_mutex_lock(&inserts_ejecutados_sem);
	list_add(inserts_ejecutados,insert);
	pthread_mutex_unlock(&inserts_ejecutados_sem);
	op->success = true;
	pthread_mutex_lock(&memorias_sem);
	memoria->cant_selects_inserts_ejecutados++;
	pthread_mutex_unlock(&memorias_sem);
	free(resp);
	return;
}

void lql_create(t_LQL_operacion* op){
	t_memoria* memoria = obtener_memoria_consistencia(op->argumentos.CREATE.tipo_consistencia,-1);
	if(!memoria->valida){
		if(op->consola){
			printf("ERROR: No hay memoria para el criterio: %s de la tabla: %s.\n",op->argumentos.CREATE.tipo_consistencia,op->argumentos.CREATE.nombre_tabla);
		}
		log_error(loggerKernel,"No hay memoria para el criterio: %s de la tabla: %s",op->argumentos.CREATE.tipo_consistencia,op->argumentos.CREATE.nombre_tabla);
		free_memoria(memoria);
		op->success = false;
		return;
	}
	pthread_mutex_lock(&(memoria->socket_mem_sem));
	char* resp = opCREATE(memoria->socket_mem, op->argumentos.CREATE.nombre_tabla, op->argumentos.CREATE.tipo_consistencia,
			op->argumentos.CREATE.numero_particiones, op->argumentos.CREATE.compactation_time);
	if(verificar_memoria_caida(resp,op,memoria->id_mem)){
		return;
	}
	pthread_mutex_unlock(&(memoria->socket_mem_sem));
	if(op->consola){
		if(string_equals_ignore_case(resp,"YA_EXISTE_TABLA")){
			printf("ERROR: CREATE %s %s %d %d falló ya que la tabla %s ya existe.\n",op->argumentos.CREATE.nombre_tabla, op->argumentos.CREATE.tipo_consistencia,
					op->argumentos.CREATE.numero_particiones, op->argumentos.CREATE.compactation_time, op->argumentos.CREATE.nombre_tabla);
			log_info(loggerKernel,"CREATE %s %s %d %d falló ya que la tabla %s ya existe.",op->argumentos.CREATE.nombre_tabla, op->argumentos.CREATE.tipo_consistencia,
					op->argumentos.CREATE.numero_particiones, op->argumentos.CREATE.compactation_time, op->argumentos.CREATE.nombre_tabla);
			op->success = false;
			free(resp);
			return;
		}
		if(string_equals_ignore_case(resp,"NO_HAY_ESPACIO")){
			printf("ERROR: CREATE %s %s %d %d falló ya que no hay espacio suficiente en el File System.\n",op->argumentos.CREATE.nombre_tabla, op->argumentos.CREATE.tipo_consistencia,
					op->argumentos.CREATE.numero_particiones, op->argumentos.CREATE.compactation_time);
			log_info(loggerKernel,"CREATE %s %s %d %d falló ya que no hay espacio suficiente en el File System.",op->argumentos.CREATE.nombre_tabla, op->argumentos.CREATE.tipo_consistencia,
			op->argumentos.CREATE.numero_particiones, op->argumentos.CREATE.compactation_time);
			op->success = false;
			free(resp);
			return;
		}
		else{
			printf("CREATE %s %s %d %d se realizó correctamente.\n",op->argumentos.CREATE.nombre_tabla, op->argumentos.CREATE.tipo_consistencia,
					op->argumentos.CREATE.numero_particiones, op->argumentos.CREATE.compactation_time);
			log_info(loggerKernel,"CREATE %s %s %d %d se realizó correctamente.",op->argumentos.CREATE.nombre_tabla, op->argumentos.CREATE.tipo_consistencia,
					op->argumentos.CREATE.numero_particiones, op->argumentos.CREATE.compactation_time);
		}
	}
	else{
		if(string_equals_ignore_case(resp,"YA_EXISTE_TABLA")){
			log_info(loggerKernel,"CREATE %s %s %d %d falló ya que la tabla %s ya existe.",op->argumentos.CREATE.nombre_tabla, op->argumentos.CREATE.tipo_consistencia,
					op->argumentos.CREATE.numero_particiones, op->argumentos.CREATE.compactation_time, op->argumentos.CREATE.nombre_tabla);
			op->success = false;
			free(resp);
			return;
		}
		if(string_equals_ignore_case(resp,"NO_HAY_ESPACIO")){
			log_info(loggerKernel,"CREATE %s %s %d %d falló ya que no hay espacio suficiente en el File System.",op->argumentos.CREATE.nombre_tabla, op->argumentos.CREATE.tipo_consistencia,
			op->argumentos.CREATE.numero_particiones, op->argumentos.CREATE.compactation_time);
			op->success = false;
			free(resp);
			return;
		}
		else{
			log_info(loggerKernel,"CREATE %s %s %d %d se realizó correctamente.",op->argumentos.CREATE.nombre_tabla, op->argumentos.CREATE.tipo_consistencia,
					op->argumentos.CREATE.numero_particiones, op->argumentos.CREATE.compactation_time);
		}
	}
	op->success = true;
	free(resp);
	return;
}

void lql_describe(t_LQL_operacion* op){
	if(string_is_empty(op->argumentos.DESCRIBE.nombre_tabla)){
		pthread_mutex_lock(&memorias_sem);
		t_memoria* mem = random_memory(memorias);
		pthread_mutex_unlock(&memorias_sem);
		pthread_mutex_lock(&(mem->socket_mem_sem));
		char* resp = opDESCRIBE(mem->socket_mem, "");
		if(verificar_memoria_caida(resp,op,mem->id_mem)){
			return;
		}
		pthread_mutex_unlock(&(mem->socket_mem_sem));
		describe_global(resp,op->consola);
		free(resp);
		op->success = true;
	}
	else{
		pthread_mutex_lock(&tablas_sem);
		t_tabla* tabla = devuelve_tabla(op->argumentos.DESCRIBE.nombre_tabla);
		pthread_mutex_unlock(&tablas_sem);
		if(tabla == NULL){
			log_error(loggerKernel,"La tabla de nombre:  %s no existe",op->argumentos.DESCRIBE.nombre_tabla);
			free_tabla(tabla);
			op->success = false;
			return;
		}
		t_memoria* memoria = obtener_memoria_consistencia(tabla->consistencia,-1);
		if(!memoria->valida){
			log_error(loggerKernel,"No hay memoria para el criterio: %s de la tabla: %s",tabla->consistencia,op->argumentos.DESCRIBE.nombre_tabla);
			free_memoria(memoria);
			op->success = false;
			return;
		}
		pthread_mutex_lock(&(memoria->socket_mem_sem));
		char* resp = opDESCRIBE(memoria->socket_mem,op->argumentos.DESCRIBE.nombre_tabla);
		if(verificar_memoria_caida(resp,op,memoria->id_mem)){
			return;
		}
		pthread_mutex_unlock(&(memoria->socket_mem_sem));
		puts(resp);
		free(resp);
		op->success = true;
	}
}

void lql_drop(t_LQL_operacion* op){
	pthread_mutex_lock(&tablas_sem);
	t_tabla* tabla = devuelve_tabla(op->argumentos.DROP.nombre_tabla);
	pthread_mutex_unlock(&tablas_sem);
	if(tabla == NULL){
		if(op->consola){
			printf("ERROR :La tabla de nombre:  %s no existe.\n",op->argumentos.DROP.nombre_tabla);
		}
		log_error(loggerKernel,"La tabla de nombre:  %s no existe",op->argumentos.DROP.nombre_tabla);
		free_tabla(tabla);
		op->success = false;
		return;
	}
	t_memoria* memoria = obtener_memoria_consistencia(tabla->consistencia,-1);
	if(!memoria->valida){
		if(op->consola){
			printf("ERROR: No hay memoria para el criterio: %s de la tabla: %s.\n",tabla->consistencia,op->argumentos.DROP.nombre_tabla);
		}
		log_error(loggerKernel,"No hay memoria para el criterio: %s de la tabla: %s",tabla->consistencia,op->argumentos.DROP.nombre_tabla);
		free_memoria(memoria);
		op->success = false;
		return;
	}
	pthread_mutex_lock(&(memoria->socket_mem_sem));
	char* resp = opDROP(memoria->socket_mem, op->argumentos.DROP.nombre_tabla);
	if(verificar_memoria_caida(resp,op,memoria->id_mem)){
		return;
	}
	pthread_mutex_unlock(&(memoria->socket_mem_sem));
	if(op->consola){
		if(string_equals_ignore_case(resp,"NO_EXISTE_TABLA")){
			printf("ERROR: No se pudo borrar la tabla %s ya que no existe.\n", op->argumentos.DROP.nombre_tabla);
			log_error(loggerKernel, "No se pudo borrar la tabla %s ya que no existe", op->argumentos.DROP.nombre_tabla);
			op->success = false;
			free(resp);
			return;
		}
		else{
			printf("Se borro la tabla %s correctamente.\n", op->argumentos.DROP.nombre_tabla);
			log_info(loggerKernel, "Se borro la tabla %s correctamente", op->argumentos.DROP.nombre_tabla);
		}
	}
	else{
		if(string_equals_ignore_case(resp,"NO_EXISTE_TABLA")){
			log_error(loggerKernel, "No se pudo borrar la tabla %s ya que no existe", op->argumentos.DROP.nombre_tabla);
			op->success = false;
			free(resp);
			return;
		}
		else{
			log_info(loggerKernel, "Se borro la tabla %s correctamente", op->argumentos.DROP.nombre_tabla);
		}
	}
	free(resp);
	op->success = true;
	return;
}

void lql_journal(t_list* list_mem, t_LQL_operacion* op){
	for(int i = 0; i < list_size(list_mem); i++){
		t_memoria* memoria = list_get(list_mem,i);
		pthread_mutex_lock(&(memoria->socket_mem_sem));
		char* resp = opJOURNAL(memoria->socket_mem);
		pthread_mutex_unlock(&(memoria->socket_mem_sem));
		if(!verificar_memoria_caida(resp,op,memoria->id_mem)){
			if(op->consola){
				printf("La memoria %d inició el proceso de Journal.\n", memoria->id_mem);
				log_info(loggerKernel, "La memoria %d inició el proceso de Journal", memoria->id_mem);
			}
			else{
				log_info(loggerKernel, "La memoria %d inició el proceso de Journal", memoria->id_mem);
			}
		}
		free(resp);
	}
	op->success = true;
	return;
}

void lql_add(t_LQL_operacion* op){
	if(!memoria_existente(memorias, op->argumentos.ADD.nro_memoria)){
		if(op->consola){
			printf("ERROR: La memoria %d no existe o no es conocida por el Kernel.\n",op->argumentos.ADD.nro_memoria);
		}
		log_error(loggerKernel,"La memoria %d no existe o no es conocida por el Kernel",op->argumentos.ADD.nro_memoria);
		op->success = false;
		return;
	}
	if(string_equals_ignore_case(op->argumentos.ADD.criterio,"SC")){
		pthread_mutex_lock(&strong_consistency_sem);
		if(strong_consistency == NULL){
			strong_consistency = obtener_memoria_por_id(op->argumentos.ADD.nro_memoria);
			strong_consistency->valida = true;
			if(op->consola){
				printf("Se ha asignado la memoria %d al criterio Strong Consistency.\n",strong_consistency->id_mem);
			}
			log_info(loggerKernel,"Se ha asignado la memoria %d al criterio Strong Consistency",strong_consistency->id_mem);
			pthread_mutex_unlock(&strong_consistency_sem);
			op->success = true;
			return;
		}
		else{
			if(op->consola){
				printf("ERROR: El criterio Strong Consistency ya posee una memoria asociada.\n");
			}
			log_error(loggerKernel,"El criterio Strong Consistency ya posee una memoria asociada");
			pthread_mutex_unlock(&strong_consistency_sem);
			op->success = true;
			return;
		}
	}
	if(string_equals_ignore_case(op->argumentos.ADD.criterio,"SHC")){
		pthread_mutex_lock(&strong_hash_consistency_sem);
		if(memoria_existente(strong_hash_consistency,op->argumentos.ADD.nro_memoria)){
			if(op->consola){
				printf("ERROR: La memoria %d ya se encuentra asignada al criterio Strong Hash Consistency.\n",op->argumentos.ADD.nro_memoria);
			}
			log_error(loggerKernel,"La memoria %d ya se encuentra asignada al criterio Strong Hash Consistency",op->argumentos.ADD.nro_memoria);
			pthread_mutex_unlock(&strong_hash_consistency_sem);
			op->success = true;
			return;
		}
		else{
			list_add(strong_hash_consistency,obtener_memoria_por_id(op->argumentos.ADD.nro_memoria));
			lql_journal(strong_hash_consistency, op);
			if(op->consola){
				printf("Se ha asignado la memoria %d al criterio Strong Hash Consistency.\n",op->argumentos.ADD.nro_memoria);
			}
			log_info(loggerKernel,"Se ha asignado la memoria %d al criterio Strong Hash Consistency",op->argumentos.ADD.nro_memoria);
			pthread_mutex_unlock(&strong_hash_consistency_sem);
			op->success = true;
			return;
		}
	}
	if(string_equals_ignore_case(op->argumentos.ADD.criterio,"EC")){
		pthread_mutex_lock(&eventual_consistency_sem);
		if(memoria_existente(eventual_consistency,op->argumentos.ADD.nro_memoria)){
			if(op->consola){
				printf("ERROR: La memoria %d ya se encuentra asignada al criterio Eventual Consistency.\n",op->argumentos.ADD.nro_memoria);
			}
			log_error(loggerKernel,"La memoria %d ya se encuentra asignada al criterio Eventual Consistency",op->argumentos.ADD.nro_memoria);
			pthread_mutex_unlock(&eventual_consistency_sem);
			op->success = true;
			return;
		}
		else{
			list_add(eventual_consistency,obtener_memoria_por_id(op->argumentos.ADD.nro_memoria));
			if(op->consola){
				printf("Se ha asignado la memoria %d al criterio Eventual Consistency.\n",op->argumentos.ADD.nro_memoria);
			}
			log_info(loggerKernel,"Se ha asignado la memoria %d al criterio Eventual Consistency",op->argumentos.ADD.nro_memoria);
			pthread_mutex_unlock(&eventual_consistency_sem);
			op->success = true;
			return;
		}
	}
	return;
}

void lql_run(FILE* archivo, t_LQL_operacion* op){
	if(archivo == NULL){
		op->success = false;
		return;
	}
	if(op->consola){
		printf("Iniciando ejecución de archivo LQL.\n");
	}
	log_info(loggerKernel,"Iniciando ejecución de archivo LQL");
	char* linea = NULL;
	size_t len = 0;
	t_lcb* lcb = crear_lcb();
	 while ((getline(&linea, &len, archivo)) != -1) {
	        t_LQL_operacion* operacion = parse(linea);
	        if(operacion->valido){
	        	op->success = true;
	        	op->consola = false;
	        	agregar_op_lcb(lcb,operacion);
	        }
	        else {
	        	if(op->consola){
	        		printf("ERROR: La linea %s no es valida.\n", linea);
	        	}
	            log_error(loggerKernel, "La linea %s no es valida", linea);
	            op->success = false;
	            op->consola = false;
	            free(linea);
	            return;
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

double tiempoPromedioSelect(){
	double tiempo_total = 0, promedio = 0;
	if(!list_is_empty(selects_ejecutados)){
		for(int i = 0; i < list_size(selects_ejecutados); i++){
			t_select_ejecutado* select = list_get(selects_ejecutados,i);
			tiempo_total += (1000 * difftime(select->tiempo_fin, select->tiempo_inicio)); //Multiplico por 1000 para tener el resultado en ms ya que en seg sería muy pequeño
		}
		promedio = tiempo_total / list_size(selects_ejecutados);
	}
	else{
		promedio = 0;
	}
	return promedio;
}

double tiempoPromedioInsert(){
	double tiempo_total = 0, promedio = 0;
	if(!list_is_empty(inserts_ejecutados)){
		for(int i = 0; i < list_size(inserts_ejecutados); i++){
			t_insert_ejecutado* insert = list_get(inserts_ejecutados,i);
			tiempo_total += (1000*difftime(insert->tiempo_fin, insert->tiempo_inicio)); //Multiplico por 1000 para tener el resultado en ms ya que en seg sería muy pequeño
		}
		promedio = tiempo_total / list_size(inserts_ejecutados);
	}
	else{
		promedio = 0;
	}
	return promedio;
}

int cantidadSelects(){
	int cantidad;
	cantidad = list_size(selects_ejecutados);
	return cantidad;
}

int cantidadInserts(){
	int cantidad;
	cantidad = list_size(inserts_ejecutados);
	return cantidad;
}

int porcentajeSelectsInserts(int cant_selects_inserts_ejecutados){
	if(!list_is_empty(selects_ejecutados) || !list_is_empty(inserts_ejecutados)){
		int cant_total_inserts_selects = list_size(selects_ejecutados) + list_size(inserts_ejecutados);
		return (cant_selects_inserts_ejecutados / cant_total_inserts_selects)*100;
	}
	return 0;
}

void memoryLoad(bool mostrarEnConsola){
	for(int i = 0; i < list_size(memorias); i++){
		t_memoria* mem = list_get(memorias,i);
		if(mostrarEnConsola){
			printf("Memory Load: Memory %d porcentaje de uso: %d%%.\n",mem->id_mem, porcentajeSelectsInserts(mem->cant_selects_inserts_ejecutados));
		}
		log_info(loggerKernel,"Memory Load: Memory %d porcentaje de uso: %d%%",mem->id_mem, porcentajeSelectsInserts(mem->cant_selects_inserts_ejecutados));
	}
}

void lql_metrics(bool mostrarEnConsola){
	pthread_mutex_lock(&memorias_sem);
	pthread_mutex_lock(&selects_ejecutados_sem);
	pthread_mutex_lock(&inserts_ejecutados_sem);
	if(mostrarEnConsola){
		printf("Read Latency / 30s: %lf ms.\n", tiempoPromedioSelect());
		printf("Write Latency / 30s: %lf ms.\n", tiempoPromedioInsert());
		printf("Reads / 30s: %d.\n", cantidadSelects());
		printf("Writes / 30s: %d.\n", cantidadInserts());
	}
	log_info(loggerKernel,"Read Latency / 30s: %lf ms", tiempoPromedioSelect());
	log_info(loggerKernel,"Write Latency / 30s: %lf ms", tiempoPromedioInsert());
	log_info(loggerKernel,"Reads / 30s: %d", cantidadSelects());
	log_info(loggerKernel,"Writes / 30s: %d", cantidadInserts());
	memoryLoad(mostrarEnConsola);
	pthread_mutex_unlock(&inserts_ejecutados_sem);
	pthread_mutex_unlock(&selects_ejecutados_sem);
	pthread_mutex_unlock(&memorias_sem);
	return;
}

void* metrics_timer(){
	while(1){
		sleep(30);
		lql_metrics(false);
		pthread_mutex_lock(&queue_exit_sem);
		queue_clean_and_destroy_elements(queue_exit,(void*) free_lcb);
		pthread_mutex_unlock(&queue_exit_sem);
		pthread_mutex_lock(&selects_ejecutados_sem);
		list_clean_and_destroy_elements(selects_ejecutados,(void*) free);
		pthread_mutex_unlock(&selects_ejecutados_sem);
		pthread_mutex_lock(&inserts_ejecutados_sem);
		list_clean_and_destroy_elements(inserts_ejecutados,(void*) free);
		pthread_mutex_unlock(&inserts_ejecutados_sem);
		pthread_mutex_lock(&memorias_sem);
		for(int i = 0; i < list_size(memorias); i++){
			t_memoria* mem = list_get(memorias,i);
			mem->cant_selects_inserts_ejecutados = 0;
		}
		pthread_mutex_unlock(&memorias_sem);
	}
	return NULL;
}

void* refresh_metadata_timer(){
	while(1){
		pthread_mutex_lock(&config_sem);
		int refresh_metadata = configKernel.metadata_refresh;
		pthread_mutex_unlock(&config_sem);
		sleep(refresh_metadata);
		char** parametros = string_n_split("DESCRIBE",2," ");
		crear_lql_describe(parametros,false);
	}
	return NULL;
}

t_tabla* devuelve_tabla(char* nombre){

	bool same_table(t_tabla* table){
		return string_equals_ignore_case(table->nombre_tabla,nombre);
	}

	return list_find(tablas,(void*) same_table);
}

t_memoria* obtener_memoria_consistencia(char* consistencia, int key){
	t_memoria* mem = NULL;
	if(string_equals_ignore_case("SC",consistencia)){
		pthread_mutex_lock(&strong_consistency_sem);
		if(strong_consistency == NULL){
			pthread_mutex_unlock(&strong_consistency_sem);
			mem = (t_memoria*)malloc(sizeof(t_memoria));
			mem->valida = false;
			return mem;
		}
		mem = strong_consistency;
		pthread_mutex_unlock(&strong_consistency_sem);
		return mem;
	}
	else if(string_equals_ignore_case("SHC",consistencia)){
		pthread_mutex_lock(&strong_hash_consistency_sem);
		if(!list_is_empty(strong_hash_consistency)){
			if(key >= 0){
				mem = hash_memory(key);
			}
			else{
				mem = random_memory(strong_hash_consistency);
			}
			mem->valida = true;
			pthread_mutex_unlock(&strong_hash_consistency_sem);
			return mem;
		}
		else{
			pthread_mutex_unlock(&strong_hash_consistency_sem);
			mem = (t_memoria*)malloc(sizeof(t_memoria));
			mem->valida = false;
			return mem;
		}
	}
	else if(string_equals_ignore_case("EC",consistencia)){
		pthread_mutex_lock(&eventual_consistency_sem);
		if(!list_is_empty(eventual_consistency)){
			mem = random_memory(eventual_consistency);
			mem->valida = true;
			pthread_mutex_unlock(&eventual_consistency_sem);
			return mem;
		}
		else{
			pthread_mutex_unlock(&eventual_consistency_sem);
			mem = (t_memoria*)malloc(sizeof(t_memoria));
			mem->valida = false;
			return mem;
		}
	}
	mem = (t_memoria*)malloc(sizeof(t_memoria));
	mem->valida = false;
	return mem;
}

t_memoria* obtener_memoria_por_id(int id){
	bool same_id(t_memoria* mem){
		return mem->id_mem == id;
	}

	return list_find(memorias,(void*) same_id);
}

t_memoria* hash_memory(int key){
	int index = key % list_size(strong_hash_consistency);
	return list_get(strong_hash_consistency,index);
}

t_memoria* random_memory(t_list* lista){
	int index = random() % list_size(lista);
	return list_get(lista,index);
}

bool verificar_memoria_caida(char* respuesta,t_LQL_operacion* op, int id_mem){
	if(string_equals_ignore_case(respuesta,"MEMORIA_DESCONECTADA")){
		if(op->consola){
			printf("La memoria %d fue desconectada.\n", id_mem);
		}
		log_error(loggerKernel,"La memoria %d fue desconectada.", id_mem);
		op->success = true; //ESTE TIPO DE ERROR NO CORTA LA EJECUCIÓN DEL RESTO DE ARCHIVO LQL
		sacar_memoria(id_mem);
		return true;
	}
	return false;
}

bool verificar_memoria_caida2(char* respuesta, int id_mem){
	if(string_equals_ignore_case(respuesta,"MEMORIA_DESCONECTADA")){
		log_error(loggerKernel,"La memoria %d fue desconectada.", id_mem);
		sacar_memoria(id_mem);
		return true;
	}
	return false;
}

void describe_global(char* data, bool mostrarPorConsola){
	char** metadata = string_split(data,"#");
	char** metadata_final;
	t_list* lista_aux = list_create();
	for(int i = 0;metadata[i] != NULL;i++){
		metadata_final = string_split(metadata[i],";");
		t_tabla* tabla = malloc(sizeof(t_tabla));
		tabla->nombre_tabla = metadata_final[0];
		tabla->consistencia = metadata_final[1];
		list_add(lista_aux,tabla);
	}
	freeParametros(metadata);
	pthread_mutex_lock(&tablas_sem);
	for(int i = 0; i < list_size(tablas); i++){
		t_tabla* tabla = list_get(tablas,i);
		int veces_repetida = 0;
		for(int j = 0; j < list_size(lista_aux); j++){
			t_tabla* tabla_aux = list_get(lista_aux,j);
			if(string_equals_ignore_case(tabla->nombre_tabla,tabla_aux->nombre_tabla)){
				veces_repetida++;
			}
		}
		if(veces_repetida==0){
			if(mostrarPorConsola){
				printf("La tabla %s fue removida de la metadata del Kernel.\n",tabla->nombre_tabla);
			}
			log_info(loggerKernel,"La tabla %s fue removida de la metadata del Kernel",tabla->nombre_tabla);
			list_remove_and_destroy_element(tablas,i,(void*)free_tabla);
		}
	}
	for(int i = 0; i < list_size(lista_aux); i++){
		t_tabla* tabla_aux = list_get(lista_aux,i);
		if(!existe_tabla(tabla_aux->nombre_tabla)){
			t_tabla* tabla_aux2 = list_remove(lista_aux,i);
			list_add(tablas,tabla_aux2);
			if(mostrarPorConsola){
				printf("La tabla %s fue agregada a la metadata del Kernel.\n",tabla_aux2->nombre_tabla);
			}
			log_info(loggerKernel,"La tabla %s fue agregada a la metadata del Kernel",tabla_aux2->nombre_tabla);
		}
	}
	freeParametros(metadata_final);
	list_destroy_and_destroy_elements(lista_aux,(void*)free_tabla);
	pthread_mutex_unlock(&tablas_sem);
}





