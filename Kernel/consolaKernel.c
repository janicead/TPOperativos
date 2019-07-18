#include "consolaKernel.h"

void* setConsole(){
	char* linea;
	int cantidadParametros;

	while(1){
		linea = readline(">");
		if(linea){
			add_history(linea);
		}
		//INSTRUCCIÓN SELECT PARÁMETROS: [NOMBRE TABLA] [KEY]
		if(!strncmp(linea,"select",6) || !strncmp(linea,"SELECT",4)){
			cantidadParametros = 2;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				log_error(loggerKernel,"No se ha especificado un nombre de tabla");
				freeParametros(parametros);
			}
			else if(parametros[2] == NULL || string_is_empty(parametros[2])){
				log_error(loggerKernel,"No se ha especificado ninguna key");
				freeParametros(parametros);
			}
			else{
				crear_lql_select(parametros);
				log_info(loggerKernel,"La operación select fue ingresada la cola de ready");
			}
		}
		//INSTRUCCIÓN INSERT PARÁMETROS: [NOMBRE TABLA] [KEY] "[VALUE]"
		else if(!strncmp(linea,"insert",6) || !strncmp(linea,"INSERT",4)){
			cantidadParametros = 3;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				log_error(loggerKernel,"No se ha especificado un nombre de tabla");
				freeParametros(parametros);
			}
			else if(parametros[2] == NULL || string_is_empty(parametros[2])){
				log_error(loggerKernel,"No se ha especificado ninguna key");
				freeParametros(parametros);
			}
			else if(parametros[3] == NULL || string_is_empty(parametros[3])){
				log_error(loggerKernel,"No se ha especificado ningun valor");
				freeParametros(parametros);
			}
			else{
				crear_lql_insert(parametros);
				log_info(loggerKernel,"La operación insert fue ingresada la cola de ready");
			}
		}
		//INSTRUCCIÓN CREATE PARÁMETROS: [NOMBRE TABLA] [TIPO DE CONSISTENCIA] [CANTIDAD PARTICIONES] [TIEMPO DE COMPACTACION]
		else if(!strncmp(linea,"create",6) || !strncmp(linea,"CREATE",4)){
			cantidadParametros = 4;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				log_error(loggerKernel,"No se ha especificado un nombre de tabla");
				freeParametros(parametros);
			}
			else if(parametros[2] == NULL || string_is_empty(parametros[2])){
				log_error(loggerKernel,"No se ha especificado ningun tipo de consistencia");
				freeParametros(parametros);
			}
			else if(parametros[3] == NULL || string_is_empty(parametros[3])){
				log_error(loggerKernel,"No se ha especificado ninguna cantidad de particiones");
				freeParametros(parametros);
			}
			else if(parametros[4] == NULL || string_is_empty(parametros[4])){
				log_error(loggerKernel,"No se ha especificado ningun tiempo de compactación");
				freeParametros(parametros);
			}
			else if(!validar_consistencia(parametros[2])){
				log_error(loggerKernel,"La consistencia %s no es válida.",parametros[2]);
				freeParametros(parametros);
			}
			else{
				crear_lql_create(parametros);
				log_info(loggerKernel,"La operación create fue ingresada la cola de ready");
			}

		}
		//INSTRUCCIÓN DESCRIE PARÁMETROS: [NOMBRE TABLA]
		//SI NO HAY PARAMETROS SE DESCRIBEN TODAS LAS TABLAS
		else if(!strncmp(linea,"describe",8) || !strncmp(linea,"DESCRIBE",4)){
			cantidadParametros = 1;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				crear_lql_describe(parametros);
			}
			else{
				crear_lql_describe(parametros);
				log_info(loggerKernel,"La operación describe fue ingresada la cola de ready");
			}
		}
		//INSTRUCCIÓN DROP PARÁMETROS: [NOMBRE TABLA]
		else if(!strncmp(linea,"drop",4) || !strncmp(linea,"DROP",4)){
			cantidadParametros = 1;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				log_error(loggerKernel,"No se ha especificado un nombre de tabla");
				freeParametros(parametros);
			}
			else{
				crear_lql_drop(parametros);
				log_info(loggerKernel,"La operación drop fue ingresada la cola de ready");
			}
		}
		//INSTRUCCIÓN JOURNAL NO TIENE PARÁMETROS
		else if(string_equals_ignore_case(linea,"journal")){
			crear_lql_journal();
			log_info(loggerKernel,"La operación journal fue ingresada la cola de ready");
		}
		//INSTRUCCIÓN ADD PARÁMETROS: MEMORY [NRO DE MEMORIA] TO [CRITERIO]
		else if(!strncmp(linea,"add",3) || !strncmp(linea,"ADD",3)){
			cantidadParametros = 4;
			int memoria;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[2] == NULL || string_is_empty(parametros[2])){
				log_error(loggerKernel,"No se ha especificado un número de memoria");
			}
			else if(parametros[4] == NULL || string_is_empty(parametros[4])){
				log_error(loggerKernel,"No se ha especificado ningún criterio");
			}
			else if(!validar_consistencia(parametros[4])){
				log_error(loggerKernel,"La consistencia %s no es válida.",parametros[4]);
			}
			else{
				memoria = atoi(parametros[2]);
				crear_lql_add(parametros[4],memoria);
				log_info(loggerKernel,"La operación add fue ingresada la cola de ready");
			}
			freeParametros(parametros);
		}
		//INSTRUCCIÓN RUN PARÁMETROS: [PATH SCRIPT LQL]
		else if(!strncmp(linea,"run",3) || !strncmp(linea,"RUN",3)){
			cantidadParametros = 1;
			char** parametros = obtenerParametros(linea,cantidadParametros + 1);
			if(parametros[1] == NULL || string_is_empty(parametros[1])){
				log_error(loggerKernel,"No se ha especificado ningún path LQL");
			}
			else{
				char* path = string_from_format("%s",parametros[1]);
				crear_lql_run(path);
				log_info(loggerKernel,"La operación run fue ingresada la cola de ready");
				free(path);
			}
			freeParametros(parametros);
		}
		//INSTRUCCIÓN METRICS NO TIENE PARÁMETROS
		else if(string_equals_ignore_case(linea,"metrics")){
			crear_lql_metrics();
			log_info(loggerKernel,"La operación metrics fue ingresada la cola de ready");
		}
		else if(!strncmp(linea,"exit",4) || !strncmp(linea,"EXIT",4)){
			free(linea);
			return NULL;
		}
		else{
			log_error(loggerKernel,"No se ingreso ningun comando valido");
		}
		free(linea);
	}
	return NULL;
}

char** obtenerParametros(char* linea, int indice){
	return string_n_split(linea,indice," ");
}

void freeParametros(char** parametros){
    string_iterate_lines(parametros, (void*) free);
	free(parametros);
}

void crear_lql_select(char** parametros){
	t_lcb* lcb = crear_lcb();
	t_LQL_operacion* op = (t_LQL_operacion*) malloc(sizeof(t_LQL_operacion));
	op->keyword = SELECT;
	op->argumentos.SELECT.key = atoi(parametros[2]);
	op->argumentos.SELECT.nombre_tabla = parametros[1];
	op->success = true;
	op->consola = true;
	op->_raw = parametros;
	agregar_op_lcb(lcb,op);
	pasar_lcb_a_ready(lcb);
	return;
}

void crear_lql_insert(char** parametros){
	t_lcb* lcb = crear_lcb();
	t_LQL_operacion* op = (t_LQL_operacion*) malloc(sizeof(t_LQL_operacion));
	op->_raw = parametros;
	op->keyword = INSERT;
	op->argumentos.INSERT.key = atoi(parametros[2]);
	op->argumentos.INSERT.nombre_tabla = parametros[1];
	op->argumentos.INSERT.valor = quitarComillas(parametros[3]);
	op->success = true;
	op->consola = true;
	agregar_op_lcb(lcb,op);
	pasar_lcb_a_ready(lcb);
	return;
}

void crear_lql_create(char** parametros){
	t_lcb* lcb = crear_lcb();
	t_LQL_operacion* op = (t_LQL_operacion*) malloc(sizeof(t_LQL_operacion));
	op->_raw = parametros;
	op->keyword = CREATE;
	op->argumentos.CREATE.nombre_tabla = parametros[1];
	op->argumentos.CREATE.compactation_time = atoi(parametros[4]);
	op->argumentos.CREATE.numero_particiones = atoi(parametros[3]);
	op->argumentos.CREATE.tipo_consistencia = parametros[2];
	op->success = true;
	op->consola = true;
	agregar_op_lcb(lcb,op);
	pasar_lcb_a_ready(lcb);
	return;
}

void crear_lql_describe(char** parametros){
	t_lcb* lcb = crear_lcb();
	t_LQL_operacion* op = (t_LQL_operacion*) malloc(sizeof(t_LQL_operacion));
	op->keyword = DESCRIBE;
	op->success = true;
	op->consola = true;
	op->_raw = parametros;
	if(parametros[1] == NULL || string_is_empty(parametros[1])){
		op->argumentos.DESCRIBE.nombre_tabla = "";
	}
	else{
		op->argumentos.DESCRIBE.nombre_tabla = parametros[1];
	}
	agregar_op_lcb(lcb,op);
	pasar_lcb_a_ready(lcb);
	return;
}

void crear_lql_drop(char** parametros){
	t_lcb* lcb = crear_lcb();
	t_LQL_operacion* op = (t_LQL_operacion*) malloc(sizeof(t_LQL_operacion));
	op->_raw = parametros;
	op->keyword = DROP;
	op->argumentos.DROP.nombre_tabla = parametros[1];
	op->success = true;
	op->consola = true;
	agregar_op_lcb(lcb,op);
	pasar_lcb_a_ready(lcb);
	return;
}

void crear_lql_journal(){
	t_lcb* lcb = crear_lcb();
	t_LQL_operacion* op = (t_LQL_operacion*) malloc(sizeof(t_LQL_operacion));
	op->_raw = NULL;
	op->keyword = JOURNAL;
	op->success = true;
	op->consola = true;
	agregar_op_lcb(lcb,op);
	pasar_lcb_a_ready(lcb);
	return;
}

void crear_lql_add(char* criterio, int nro_memoria){
	t_lcb* lcb = crear_lcb();
	t_LQL_operacion* op = (t_LQL_operacion*) malloc(sizeof(t_LQL_operacion));
	op->_raw = NULL;
	op->keyword = ADD;
	op->argumentos.ADD.criterio = criterio;
	op->argumentos.ADD.nro_memoria = nro_memoria;
	op->success = true;
	op->consola = true;
	agregar_op_lcb(lcb,op);
	pasar_lcb_a_ready(lcb);
	return;
}

void crear_lql_run(char* path){
	t_lcb* lcb = crear_lcb();
	t_LQL_operacion* op = (t_LQL_operacion*) malloc(sizeof(t_LQL_operacion));
	op->_raw = NULL;
	op->keyword = RUN;
	op->argumentos.RUN.path = malloc(strlen(path)+1);
	strcpy(op->argumentos.RUN.path, path);
	op->success = true;
	op->consola = true;
	agregar_op_lcb(lcb,op);
	pasar_lcb_a_ready(lcb);
	return;
}

void crear_lql_metrics(){
	t_lcb* lcb = crear_lcb();
	t_LQL_operacion* op = (t_LQL_operacion*) malloc(sizeof(t_LQL_operacion));
	op->_raw = NULL;
	op->keyword = METRICS;
	op->success = true;
	op->consola = true;
	agregar_op_lcb(lcb,op);
	pasar_lcb_a_ready(lcb);
	return;
}

