#include "usoVariado.h"

char* int_to_string(int numero){
	int length = snprintf(NULL,0,"%d",numero);
	char* string = malloc(length + 1);
	snprintf(string,length + 1,"%d",numero);
	return string;
}

int pasarMilisegundosASegundos(int milisegundos){
	return milisegundos * 0.001;
}


void printearArrayDeChars(char** arrayDeChars){
	int tamanioIpSeeds = tamanioArray(arrayDeChars);
	for (int i =0; i<tamanioIpSeeds; i++){
		printf("%s\n", arrayDeChars[i]);
	}

}
void printearArrayDeInts(int arrayDeInts[]){
	int tamanioIpSeeds = tamanioArray(arrayDeInts);
	for (int i =0; i<tamanioIpSeeds; i++){
		int a = *(arrayDeInts[i]);
		printf("%d\n", a);
	}
}
void hacerFreeArray(void** array){
	for(int i = 0; array[i]!= NULL; i++){
		free(array[i]);
	}
}
int tamanioArray(void** array){
	int i = 0;
	while(array[i]!=NULL){
		i++;
	}
	return i;
}
long int cantLugaresEnArchivo(FILE* archivo){
	 long int sz;
	    fseek(archivo, 0L, SEEK_END);
	    sz = ftell(archivo);
	return sz;
}

int longitudArrayDePunteros(char **unArrayDePunteros)
{
	int i;

	for(i=0; unArrayDePunteros[i] != NULL ;i++);

	return i;
}

unsigned long int obtenerTimeStamp(){
	return time(NULL);
}
