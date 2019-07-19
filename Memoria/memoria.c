#include "memoria.h"

int main(void) {

	configurarLoggerMemoria();
	configMemoria =leerConfigMemoria();
	mostrarDatosArchivoConfig();
	tamanioDadoPorLFS = conectarmeAlLFS();
	log_info(loggerMemoria, "Esperando Conexiones");
	iniciarEscucha();
	crearTablaGossip();
	definirTamanioMemoriaPrincipal(tamanioDadoPorLFS);
	crearHilos();

	return 0;
}

void crearTablaGossip(){
	tablaDeGossipMemoria = list_create();
	agregarATablaDeGossip(configMemoria.puertoDeEscucha,configMemoria.ipDeEscucha, configMemoria.numeroDeMemoria,true, tablaDeGossipMemoria);
	mostrarmeMemoriasTablaGossip(tablaDeGossipMemoria);

}

void crearHilos(){

	pthread_create(&consola, NULL, (void*)crearConsolaMemoria, NULL);

	pthread_create(&multiplexacionMemoria, NULL, (void*)realizarMultiplexacion, (void*) servidorEscuchaMemoria);

	pthread_create(&gossip, NULL, (void*)realizarGossip, NULL);

	pthread_create(&hacerJournal,NULL, (void*)JOURNALMemoria, NULL);

	pthread_create(&config_observer,NULL,observer_config,NULL);

	pthread_detach(config_observer);
	pthread_join(consola, NULL);
	pthread_join(multiplexacionMemoria, NULL);
	pthread_join(gossip, NULL);
	pthread_join(hacerJournal, NULL);

}

