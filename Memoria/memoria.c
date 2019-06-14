#include "memoria.h"

int main(void) {

	pthread_t multiplexacionMemoria, clienteMemoria;

	configurarLoggerMemoria();
	configMemoria =leerConfigMemoria();
	mostrarDatosArchivoConfig();
	tablaDeSegmentos = list_create();
	definirTamanioMemoriaPrincipal(1);
	crearConsolaMemoria();
	/*iniciarEscuchaMemoria();
	t_list * listaMemoriasQueMeConecte=  list_create();
	log_info(loggerMemoria, "Esperando Conexiones");
	pthread_create(&multiplexacionMemoria, NULL, realizarMultiplexacion, NULL);
	pthread_create(&clienteMemoria, NULL, hacermeClienteDeMisServers, NULL);
	pthread_join(multiplexacionMemoria, NULL);
	pthread_join(clienteMemoria, NULL);
	pthread_exit(multiplexacionMemoria);
	pthread_exit(clienteMemoria);
*/
	borrarElementos();
	list_clean(tablaDeSegmentos);
	list_destroy(tablaDeSegmentos);

	return 0;
}
