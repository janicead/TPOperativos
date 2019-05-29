#include "Kernel.h"

void pruebaParser(){
	FILE* script = abrirArchivo("/home/utnso/tp-2019-1c-BEFGN/prueba.lql");
	lql_run(script);
	return;
}

void iniciarValoresParaTest(){
	t_tabla* tabla = (t_tabla*) malloc(sizeof(t_tabla));
	tabla->consistencia = "SC";
	tabla->nombre_tabla = "prueba";
	agregar_tabla(tabla);
	t_tabla* tabla2 = (t_tabla*) malloc(sizeof(t_tabla));
	tabla2->consistencia = "SC";
	tabla2->nombre_tabla = "personajes";
	agregar_tabla(tabla2);
	agregar_memoria(5);
	strong_consistency = obtener_memoria_por_id(0);
	strong_consistency->valida = true;
}

int main(void) {
	iniciar();
	iniciarValoresParaTest();
	//conectarAMemoria();
	pruebaParser();
	ejecutar();
	ejecutar();
	ejecutar();
	//setConsole();
	exit_gracefully(EXIT_SUCCESS);
}

void iniciar(){
	verificarArchivoConfigKernel();
	mostrarDatosArchivoConfigKernel();
	puertoMemoria = int_to_string(configKernel.puerto_memoria);
	inicializarIds();
	crear_colas();
	crear_listas();
	return;
}

void conectarAMemoria(){
	char * buffer[1024];
	int socketMemoria = conectarAlServidor(configKernel.ip_memoria,puertoMemoria);
	send(socketMemoria,"Hola Memoria",strlen("Hola Memoria"),0);
	int bytes = recv(socketMemoria,buffer,1024,0);
	buffer[bytes] = '\0';
	printf("%s%",buffer);
}
