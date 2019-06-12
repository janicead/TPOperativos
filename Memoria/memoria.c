#include "memoria.h"

int main(void) {

	pthread_t multiplexacionMemoria, clienteMemoria;
	//crearConsolaMemoria();
	configurarLoggerMemoria();
	configMemoria =leerConfigMemoria();
	mostrarDatosArchivoConfig();
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
	return 0;
}
