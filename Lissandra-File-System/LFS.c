#include "LFS.h"

int main(void) {
	verificarArchivoConfig();
	mostrarDatosArchivoConfig();
	setConsole(); //AGREGAR UN HILO PARA LA CONSOLA
	exit_gracefully(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}



