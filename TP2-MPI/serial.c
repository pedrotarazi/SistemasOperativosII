#include <stdio.h>
#include <sys/time.h>
#include "signal.h"
#include "sample.h"

/******** Variables Globales *********/
float y[1250500]={0};				// Declaracion y seteo de la variable Y a 0.
float z[1250499]={0};				// Declaracion y seteo de la variable Z a 0.

/******** Declaracion de Funciones ***/
void matchFilter();
void PPP();

int main(int argc, char **argv)
{
	matchFilter();					// Llamada a la funcion matchFilter.
	PPP();							// Llamada a la funcion PPP.
			
	return 0;
}

/********************************* Definicion de Funciones **************************************/

/*
 * Este metodo realiza el filtrado de dos señales mediante
 * una convolucion entre las mismas.
 * @param void
 * @return void
 */
void matchFilter(){
	int i,n;							// Variables usadas para los ciclos for.
	int cantY = 500 + 1250000 -1;		// Cantidad de datos que tendrá el vector Y.
	int cantSIGNAL = 1250000;   				// Cantidad de datos que tiene el vector Signal (x).
	int cantSAMPLE = 500;       				// Cantidad de datos que tiene el vector Sample (h).
	struct timeval ti, tf;
    double tiempo;
    
	gettimeofday(&ti, NULL);
	/* Ciclos for anidados donde se recorreran los vectores SIGNAL y SAMPLE para obtener, mediante
	 * convolucion, la señal a la salida del filtro denominada Y[n]. */
	for(n=0;n<cantSIGNAL; n++){						// FOR que recorre todo el vector SIGNAL
		for(i=0;i<cantSAMPLE;i++){					// FOR que recorre todo el vector SAMPLE
			y[n+i]=y[n+i]+signal[n]*sample[i];		// Formula para calcular la convolucion entre dos señales.
		}
	}
	gettimeofday(&tf, NULL);   // Instante final
}

/*
 * Este metodo realiza el procesamiento de par de pulsos
 * mediante la correlacion de una señal consigo misma, es decir,
 * mediante autocorrelacion.
 * @param void
 * @return void
 */
void PPP(){
	int i,n;							// Variables usadas para los ciclos for.
	int M=25000;						// Cantidad de muestras transmitidas
	int cantZ = 1250498;				// Cantidad de elementos que tendra Z
	FILE *archivo;						// Variable que declara un fichero
	struct timeval ti, tf;
    double tiempo;
    
	archivo = fopen("PPP_serial.txt", "w");	// Crea el archivo donde guardara la informacion obtenida
	if (archivo == NULL){
		printf("\nError de apertura del archivo. \n\n");
	}
	else{
		gettimeofday(&ti, NULL);
		for(n=0;n<cantZ;n++){
			for(i=0;i<M-2;i++){
				z[n]=z[n] + ( y[n+1] * y[i] ); 			// Formula de autocorrelacion
			}
		}
		gettimeofday(&tf, NULL);   // Instante final
		for(i=0;i<cantZ;i++) 
			fprintf(archivo, "%f\n", z[i]);		// Guarda los datos de Z[n] en el archivo
		fclose(archivo);					// Cierra el archivo
		tiempo= (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0;
		printf("PPP en %g milisegundos\n", tiempo);
	}	
}

