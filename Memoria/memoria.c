#include "memoria.h"

int main(int argc, char* argv[]) {

	if(argc == 1){
		puts("ERROR: No definio cual prueba es");
		return EXIT_FAILURE;
	}
	if (argc ==2){
		puts("ERROR: No definio cual memoria es");
	}
	if(argc > 3){
		puts("ERROR ingresó más de un parámetro");
		return EXIT_FAILURE;
	}
	setear_path_config(atoi(argv[1]), argv[2]);
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

void setear_path_config(int nroPrueba, char* nroMemoria){
	char* path = definirPrueba(nroPrueba, nroMemoria);
	PATH_MEMORIA_CONFIG = malloc(strlen(path)+1);
	strcpy(PATH_MEMORIA_CONFIG, path);

}

char* definirPrueba(int nroPrueba, char* nroMemoria){
	if (nroPrueba ==1){
		return string_from_format("/home/utnso/workspace/tp-2019-1c-BEFGN/PruebaBase/memoria%s.conf",nroMemoria);
	} else if (nroPrueba == 2){
		return string_from_format("/home/utnso/workspace/tp-2019-1c-BEFGN/PruebaKernel/memoria%s.conf",nroMemoria);
	} else if (nroPrueba == 3){
		return string_from_format("/home/utnso/workspace/tp-2019-1c-BEFGN/PruebaLFS/memoria%s.conf",nroMemoria);
	} else if (nroPrueba == 4) {
		return string_from_format("/home/utnso/workspace/tp-2019-1c-BEFGN/PruebaMemoria/memoria%s.conf",nroMemoria);
	} else {
		return string_from_format("/home/utnso/workspace/tp-2019-1c-BEFGN/PruebaStress/memoria%s.conf",nroMemoria);
	}
}

