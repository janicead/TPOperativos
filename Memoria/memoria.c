#include "memoria.h"

int main(void) {

	pthread_t multiplexacionMemoria, clienteMemoria, gossip;

	configurarLoggerMemoria();
	configMemoria =leerConfigMemoria();
	mostrarDatosArchivoConfig();
	definirTamanioMemoriaPrincipal(20);
	//crearConsolaMemoria();
	iniciarEscuchaMemoria();
	//t_list * listaMemoriasQueMeConecte=  list_create();
	tablaDeGossip = list_create();
	agregarATablaDeGossip(configMemoria.puertoDeEscucha,configMemoria.ipDeEscucha, configMemoria.numeroDeMemoria);
	log_info(loggerMemoria, "Esperando Conexiones");
	pthread_create(&gossip, NULL, (void*)realizarGossip, NULL);
	pthread_create(&multiplexacionMemoria, NULL,(void*) realizarMultiplexacion, NULL);
	pthread_create(&clienteMemoria, NULL,(void*) hacermeClienteDeMisServers, NULL);
	pthread_join(gossip, NULL);
	pthread_join(multiplexacionMemoria, NULL);
	pthread_join(clienteMemoria, NULL);
	pthread_join(gossip, NULL);
	/*pthread_exit(gossip);
	pthread_exit(multiplexacionMemoria);
	pthread_exit(clienteMemoria);*/

	return 0;
}
