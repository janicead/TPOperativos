#include "planificador.h"


FILE* abrirArchivo(char* path){
	FILE* f = fopen(path,"r");
	if(f == NULL){
		log_error(loggerKernel,"Error al abrir el archivo LQL");
		exit(EXIT_FAILURE);
	}
	return f;
}

void parsear(FILE* archivo){
	char* linea = NULL;
	size_t len = 0;

	 while ((getline(&linea, &len, archivo)) != -1) {
	        t_LQL_operacion operacion = parse(linea);

	        if(operacion.valido){
	            switch(operacion.keyword){
	                case SELECT:
	                    printf("SELECT\tnombre de tabla: %s", operacion.argumentos.SELECT.nombre_tabla);
	                    printf("\tkey: %d\n",operacion.argumentos.SELECT.key);
	                    break;
	                case INSERT:
	                    printf("INSERT\tnombre de tabla: %s", operacion.argumentos.INSERT.nombre_tabla);
	                    printf("\tkey: %d",operacion.argumentos.INSERT.key);
	                    printf("\tvalor: %s\n",operacion.argumentos.INSERT.valor);
	                    break;
	                case CREATE:
	                    printf("CREATE\tnombre de tabla: %s", operacion.argumentos.CREATE.nombre_tabla);
	                    printf("\ttipo consistencia: %s",operacion.argumentos.CREATE.tipo_consistencia);
	                    printf("\tnro particiones: %d",operacion.argumentos.CREATE.numero_particiones);
	                    printf("\tcompactation time: %d\n",operacion.argumentos.CREATE.compactation_time);
	                    break;
	                case DESCRIBE:
	                    printf("DESCRIBE\tnombre de tabla: %s\n", operacion.argumentos.DESCRIBE.nombre_tabla);
	                    break;
	                case DROP:
	                    printf("CREATE\tnombre de tabla: %s\n", operacion.argumentos.DROP.nombre_tabla);
	                    break;
	                case JOURNAL:
	                    printf("JOURNAL\n");
	                    break;
	                case ADD:
	                    printf("ADD\tnumero de memoria: %d", operacion.argumentos.ADD.nro_memoria);
	                    printf("\tcriterio: %s\n",operacion.argumentos.ADD.criterio);
	                    break;
	                case RUN:
	                    printf("RUN\tpath: %s\n", operacion.argumentos.RUN.path);
	                    break;
	                case METRICS:
	                    printf("METRICS\n");
	                    break;
	                default:
	                    log_error(loggerKernel, "No pude interpretar %s", linea);
	                    exit(EXIT_FAILURE);
	            }

	            destruir_operacion(operacion);
	        } else {
	            log_error(loggerKernel, "La linea %s no es valida", linea);
	            exit(EXIT_FAILURE);
	        }
	    }

	    fclose(archivo);
	    if (linea){
	        free(linea);
	    }
}
