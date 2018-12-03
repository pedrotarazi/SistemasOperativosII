#include <stdio.h>

#include <unistd.h>

int main(int argc, char **argv)
{
	FILE *archivoOrigen, *archivoDestino;
 	char buffer[150];
	while(1){		//lee línea por línea guardándola en la variable buffer hasta llegar a la línea especifica
		archivoOrigen = fopen ( "registro.csv", "r" ); 
		fgets(buffer,150,archivoOrigen);
		fgets(buffer,150,archivoOrigen);
		while(!feof(archivoOrigen)){
			archivoDestino = fopen ( "memoria.csv", "a" );
			printf("Dato...%s",buffer);
			fputs(buffer,archivoDestino);
			fgets(buffer,150,archivoOrigen);
			sleep(1);
			
			fclose(archivoDestino);
		}
		rewind(archivoOrigen);
	}
	fclose(archivoOrigen);
}
