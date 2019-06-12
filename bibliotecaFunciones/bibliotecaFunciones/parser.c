#include "parser.h"

#define returnError parserLQL ERROR={ .esValido = false }; return ERROR

void destruirOperacion(parserLQL operacion){
	if(operacion.liberar){
		string_iterate_lines(operacion.liberar, (void*) free);
		free(operacion.liberar);
	}
}


parserLQL parser (char* linea){
   	parserLQL lineaLeida;
    	if(linea == NULL || string_equals_ignore_case(linea, "")){ //Las sentencias finalizan con un salto de línea.
    		fprintf(stderr, "No puede interpretar una linea vacia\n");
    		returnError;
    	}
    	lineaLeida.esValido=true;

    	char* lineaAux = string_duplicate(linea); //copio linea que estoy leyendo
    	string_trim(&lineaAux); //le quito caracteres de derecha e izquierda que esten vacios
    	char** split = string_n_split(lineaAux, 5, " "); //hago que divida la linea en 4 palabras cada vez que encuentre un espacio
    	//de acuerdo a los structs que hice en parse.h el maximo de argumentos en un struct es 3 mas la palabra reservada entonces 4

    	char* palabraReservada = string_duplicate(split[0]);

    	if (string_starts_with(lineaAux, "#")){
    		free(lineaAux);
    		hacerFreeArray(split);
    		free(split);
    		free(palabraReservada);
    		returnError;
    	} // si la linea comienza con # significa que es un comentario
    	lineaLeida.liberar=split;

    	if(!strcasecmp(palabraReservada, "SELECT")){
    			lineaLeida.palabraReservada = SELECT;
    			lineaLeida.argumentos.SELECT.nombreTabla=string_duplicate(split[1]);
    			lineaLeida.argumentos.SELECT.key= atoi(split[2]);

    	} else if(!strcasecmp(palabraReservada, "INSERT")){
    			lineaLeida.palabraReservada = INSERT;
    			lineaLeida.argumentos.INSERT.nombreTabla= string_duplicate(split[1]);
    			lineaLeida.argumentos.INSERT.key= atoi(split[2]);
    			lineaLeida.argumentos.INSERT.value= string_duplicate(split[3]);
    			lineaLeida.argumentos.INSERT.timestamp= atoi(split[4]);
    	} else	if(!strcasecmp(palabraReservada, "CREATE")){
    			lineaLeida.palabraReservada = CREATE;
    			lineaLeida.argumentos.CREATE.nombreTabla= string_duplicate(split[1]);
    			lineaLeida.argumentos.CREATE.tipoConsistencia= string_duplicate(split[2]);
    			lineaLeida.argumentos.CREATE.numeroParticiones= atoi(split[3]);
    			lineaLeida.argumentos.CREATE.campactionTime= atoi(split[4]);
    	} else if(!strcasecmp(palabraReservada, "DESCRIBE")){
    			lineaLeida.palabraReservada = DESCRIBE;
    			lineaLeida.argumentos.DESCRIBE.nombreTabla= string_duplicate(split[1]);
    	} else if(!strcasecmp(palabraReservada, "DROP")){
    			lineaLeida.palabraReservada = DROP;
    			lineaLeida.argumentos.DROP.nombreTabla= string_duplicate(split[1]);
    	} else if(!strcasecmp(palabraReservada, "ADD")){
    			lineaLeida.palabraReservada = ADD;
    			lineaLeida.argumentos.ADD.numero= atoi(split[1]);
    			lineaLeida.argumentos.ADD.criterio= string_duplicate(split[2]);;
    	} else if(!strcasecmp(palabraReservada, "JOURNAL")){
    			lineaLeida.palabraReservada = JOURNAL;
    	} else if(!strcasecmp(palabraReservada, "METRICS")){
    			lineaLeida.palabraReservada = METRICS;
    	} else {
    		fprintf(stderr, "No se encontro la palabra reservada <%s>\n", palabraReservada);
    	}

    	free(lineaAux);
		hacerFreeArray(split);
		free(split);
		free(palabraReservada);
    	return lineaLeida;

}
