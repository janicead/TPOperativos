#include "memoria.h"

int main(void) {

	pthread_t multiplexacionMemoria, config_observer/*, clienteMemoria, gossip*/;

	configurarLoggerMemoria();
	configMemoria =leerConfigMemoria();
	mostrarDatosArchivoConfig();
	definirTamanioMemoriaPrincipal(20);
	//crearConsolaMemoria();

	iniciarEscucha();

	log_info(loggerMemoria, "Esperando Conexiones");
	pthread_create(&multiplexacionMemoria, NULL, (void*)realizarMultiplexacion, (void*) servidorEscuchaMemoria);
	//pthread_create(&config_observer,NULL,observer_config,NULL);
	//pthread_detach(config_observer);
	pthread_join(multiplexacionMemoria, NULL);
	//iniciarEscuchaMemoria();
	/*
	//t_list * listaMemoriasQueMeConecte=  list_create();
	//tablaDeGossip = list_create();
	//agregarATablaDeGossip(configMemoria.puertoDeEscucha,configMemoria.ipDeEscucha, configMemoria.numeroDeMemoria);
	//pthread_create(&gossip, NULL, realizarGossip, NULL);
	//pthread_create(&multiplexacionMemoria, NULL, (void*)realizarMultiplexacion, NULL);
	//pthread_create(&clienteMemoria, NULL, hacermeClienteDeMisServers, NULL);
	//pthread_join(gossip, NULL);
	//pthread_join(clienteMemoria, NULL);
	//pthread_join(gossip, NULL);
	//pthread_exit(gossip);
	//pthread_exit(multiplexacionMemoria);
	//pthread_exit(clienteMemoria);
*/
	return 0;
}
