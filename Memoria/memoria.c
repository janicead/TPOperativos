#include "memoria.h"

int main(void) {

	pthread_t multiplexacionMemoria, clienteMemoria, gossip;
	definirTamanioMemoriaPrincipal(20);
	crearConsolaMemoria();
	configurarLoggerMemoria();
	configMemoria =leerConfigMemoria();
	mostrarDatosArchivoConfig();
	/*iniciarEscuchaMemoria();
	t_list * listaMemoriasQueMeConecte=  list_create();
	tablaDeGossip = list_create();
	agregarATablaDeGossip(configMemoria.puertoDeEscucha,configMemoria.ipDeEscucha, configMemoria.numeroDeMemoria);
	log_info(loggerMemoria, "Esperando Conexiones");
	pthread_create(&gossip, NULL, realizarGossip, NULL);
	pthread_create(&multiplexacionMemoria, NULL, realizarMultiplexacion, NULL);
	pthread_create(&clienteMemoria, NULL, hacermeClienteDeMisServers, NULL);
	pthread_join(gossip, NULL);
	pthread_join(multiplexacionMemoria, NULL);
	pthread_join(clienteMemoria, NULL);
	pthread_join(gossip, NULL);
	pthread_exit(gossip);
	pthread_exit(multiplexacionMemoria);
	pthread_exit(clienteMemoria);
*/
	return 0;
}
