#include "memoria.h"

int main(int argc, char* argv[]) {

	if(argc == 1){
		puts("Error no ingresó ningún archivo de configuración");
		return EXIT_FAILURE;
	}
	if(argc > 2){
		puts("ERROR ingresó más de un parámetro");
		return EXIT_FAILURE;
	}
	setear_path_config(argv[1]);
	configurarLoggerMemoria();
	configMemoria =leerConfigMemoria();
	mostrarDatosArchivoConfig();
	socketLFS = 0;
	conectarmeAlLFS();
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

	pthread_create(&hacerJournal,NULL, (void*)hacerElJOURNAL, NULL);

	pthread_create(&conexionLFS, NULL, (void*)conectarmeAlLFSHILO, NULL);

	pthread_create(&config_observer,NULL,observer_config,NULL);
	pthread_detach(config_observer);

	pthread_join(consola, NULL);
	pthread_join(multiplexacionMemoria, NULL);
	pthread_join(gossip, NULL);
	pthread_join(hacerJournal, NULL);
	pthread_join(conexionLFS, NULL);
}

void setear_path_config(char* path){
	PATH_MEMORIA_CONFIG = (char*)malloc(1+strlen(path));
	strcpy(PATH_MEMORIA_CONFIG,path);
}

