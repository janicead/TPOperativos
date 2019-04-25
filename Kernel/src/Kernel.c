#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <prueba.h>

int main(void) {
	//char * palabra;
	bool resultado = string_is_empty("hola");
	if(resultado){
		imprimir("vacio");
	}
	else{
		imprimir("lleno");
	}
	return EXIT_SUCCESS;
}
