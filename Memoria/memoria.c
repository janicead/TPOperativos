#include "memoria.h"

int main(void) {

	configurarLoggerMemoria();
	configMemoria =leerConfigMemoria();
	mostrarDatosArchivoConfig();
	definirTamanioMemoriaPrincipal(20);

	log_info(loggerMemoria, "Esperando Conexiones");
	iniciarEscucha();
	crearTablaGossip();
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

	//pthread_create(&config_observer,NULL,observer_config,NULL);
	//pthread_detach(config_observer);
	pthread_join(consola, NULL);
	pthread_join(multiplexacionMemoria, NULL);
	pthread_join(gossip, NULL);
	pthread_join(hacerJournal, NULL);

}
void test(){
	sleep(10);
	char* rta = SELECTMemoria("TABLA 1", 12, 0);
	printf("La respuesta 1 es %s\n", rta);
	sleep(20);
	char* rta1 = SELECTMemoria("TABLA 2", 2, 0);
	printf("La respuesta 2 es %s\n", rta1);
	sleep(30);
	char* rta2 = SELECTMemoria("TABLA 4", 1, 0);
	printf("La respuesta 3 es %s\n", rta2);
	sleep(40);
	char* rta3 = SELECTMemoria("TABLA 5", 3, 0);
	printf("La respuesta 4 es %s\n", rta3);
	sleep(50);
	char* rta4 = SELECTMemoria("TABLA 3",666, 0);
	printf("La respuesta 5 es %s\n", rta4);


}
