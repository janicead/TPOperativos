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

	char* auxLine = malloc(strlen(linea)+1);
	strcpy(auxLine, linea);
	string_trim(&auxLine);
	char** split = string_split(auxLine," ");

	char* keyword = split[0];



	if(string_equals_ignore_case(keyword, "SELECT")){
		ret->_raw = split;
		ret->keyword = SELECT;
		ret->argumentos.SELECT.nombre_tabla = split[1];
		ret->argumentos.SELECT.key = atoi(split[2]);
	}
	else if(string_equals_ignore_case(keyword, "INSERT")){

		char*lineadup = string_new();
		char* value= armarValue(split);
		char* stringFinal= string_new();
		string_append(&stringFinal, split[0]);
		string_append(&stringFinal," ");
		string_append(&stringFinal,split[1]);
		string_append(&stringFinal," ");
		string_append(&stringFinal,split[2]);
		string_append(&stringFinal," ");
		string_append(&stringFinal, value);
		hacerFreeArray((void**)split);
		free(split);
		split = string_split(stringFinal, " ");
		int tamanio2 = tamanioArray((void**)split);
		for(int i = 0; i <tamanio2; i++){
			string_append(&lineadup, split[i]);
			if(i!=tamanio2-1){
				string_append(&lineadup, " ");
			}
		}
		strcpy(linea,lineadup);
		free(value);
		free(stringFinal);
		free(lineadup);
		ret->_raw = split;
		ret->keyword = INSERT;
		ret->argumentos.INSERT.nombre_tabla =  split[1];
		ret->argumentos.INSERT.key =  atoi(split[2]);
		ret->argumentos.INSERT.valor =quitarEspacioFalso(split[3]);
	}
	else if(string_equals_ignore_case(keyword, "CREATE")){
		ret->_raw = split;
		ret->keyword = CREATE;
		ret->argumentos.CREATE.nombre_tabla = split[1];
		ret->argumentos.CREATE.tipo_consistencia = split[2];
		ret->argumentos.CREATE.numero_particiones = atoi(split[3]);
		ret->argumentos.CREATE.compactation_time = atoi(split[4]);
	}
	else if(string_equals_ignore_case(keyword, "DESCRIBE")){
		ret->_raw = split;
		ret->keyword = DESCRIBE;
		if(split[1]==NULL){
			ret->argumentos.DESCRIBE.nombre_tabla = "";
		}
		else {
			ret->argumentos.DESCRIBE.nombre_tabla = split[1];
		}
	}
	else if(string_equals_ignore_case(keyword, "DROP")){
		ret->_raw = split;
		ret->keyword = DROP;
		ret->argumentos.DROP.nombre_tabla = split[1];
	}
	else if(string_equals_ignore_case(keyword, "JOURNAL")){
		ret->_raw = split;
		ret->keyword = JOURNAL;
	}
	else if(string_equals_ignore_case(keyword, "ADD")){
		ret->_raw = split;
		ret->keyword = ADD;
		ret->argumentos.ADD.nro_memoria = atoi(split[2]);
		ret->argumentos.ADD.criterio = split[4];
	}
	else if(string_equals_ignore_case(keyword, "RUN")){
		ret->_raw = split;
		ret->keyword = RUN;
		ret->argumentos.RUN.path = split[1];
	}
	else if(string_equals_ignore_case(keyword, "METRICS")){
		ret->_raw = split;
		ret->keyword = METRICS;
	}
	else {
		log_error(loggerKernel, "No se encontro el keyword %s", keyword);
		return parse_error();
	}

	free(auxLine);
	return ret;
}


char* quitarEspacioFalso(char* value){
	char* operacionFinal = string_new();
	char** valuearray = string_split(value, ";");
	int tamanio = tamanioArray((void**)valuearray);
	if(tamanio== 0){
		free(operacionFinal);
		hacerFreeArray((void**)valuearray);
		free(valuearray);
		return quitarComillas(value);
	}else {
	for(int i = 0; i < tamanio; i ++){
		string_append(&operacionFinal, valuearray[i]);
		if(i!=tamanio-1){
		string_append(&operacionFinal, " ");}

	}
	hacerFreeArray((void**)valuearray);
	free(valuearray);
	char** a = string_split(operacionFinal, "\"");
	free(operacionFinal);
	char* v =malloc(strlen(a[0])+1);
	strcpy(v, a[0]);
	hacerFreeArray((void**)a);
	free(a);
	return v;
	}
}






