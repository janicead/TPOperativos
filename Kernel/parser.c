#include "parser.h"

t_LQL_operacion* parse_error(){
	t_LQL_operacion* op = malloc(sizeof(t_LQL_operacion));
	op->valido = false;
	return op;
}

t_LQL_operacion* parse(char* linea){
	if(linea == NULL || string_equals_ignore_case(linea, "")){
		log_error(loggerKernel, "No pude interpretar una linea vacia\n");
		return parse_error();
	}

	t_LQL_operacion* ret = malloc(sizeof(t_LQL_operacion));
	ret->valido = true;

	char* auxLine = string_duplicate(linea);
	string_trim(&auxLine);
	char** split = string_n_split(auxLine, 5, " ");

	char* keyword = split[0];

	ret->_raw = split;

	if(string_equals_ignore_case(keyword, "SELECT")){
		ret->keyword = SELECT;
		ret->argumentos.SELECT.nombre_tabla = split[1];
		ret->argumentos.SELECT.key = atoi(split[2]);
	}
	else if(string_equals_ignore_case(keyword, "INSERT")){
		ret->keyword = INSERT;
		ret->argumentos.INSERT.nombre_tabla =  split[1];
		ret->argumentos.INSERT.key =  atoi(split[2]);
		ret->argumentos.INSERT.valor =  split[3];
	}
	else if(string_equals_ignore_case(keyword, "CREATE")){
		ret->keyword = CREATE;
		ret->argumentos.CREATE.nombre_tabla = split[1];
		ret->argumentos.CREATE.tipo_consistencia = split[2];
		ret->argumentos.CREATE.numero_particiones = atoi(split[3]);
		ret->argumentos.CREATE.compactation_time = atoi(split[4]);
	}
	else if(string_equals_ignore_case(keyword, "DESCRIBE")){
		ret->keyword = DESCRIBE;
		ret->argumentos.DESCRIBE.nombre_tabla = split[1];
	}
	else if(string_equals_ignore_case(keyword, "DROP")){
		ret->keyword = DROP;
		ret->argumentos.DROP.nombre_tabla = split[1];
	}
	else if(string_equals_ignore_case(keyword, "JOURNAL")){
		ret->keyword = JOURNAL;
	}
	else if(string_equals_ignore_case(keyword, "ADD")){
		ret->keyword = ADD;
		ret->argumentos.ADD.nro_memoria = atoi(split[2]);
		ret->argumentos.ADD.criterio = split[4];
	}
	else if(string_equals_ignore_case(keyword, "RUN")){
		ret->keyword = RUN;
		ret->argumentos.RUN.path = split[1];
	}
	else if(string_equals_ignore_case(keyword, "METRICS")){
		ret->keyword = METRICS;
	}
	else {
		log_error(loggerKernel, "No se encontro el keyword %s", keyword);
		return parse_error();
	}

	free(auxLine);
	return ret;
}
