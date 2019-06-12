#include "types.h"

char* int_to_string(int numero){
	int length = snprintf(NULL,0,"%d",numero);
	char* string = malloc(length + 1);
	snprintf(string,length + 1,"%d",numero);
	return string;
}
