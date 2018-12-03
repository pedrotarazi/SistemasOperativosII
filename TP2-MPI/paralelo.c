/*
 * Autor: Tarazi, Pedro Esequiel
 * Matricula: 35.035.736
 * Carrera: Ingenieria en Computacion
 * Materia: Sistemas Operativos II
 * Trabajo Practico N°2
 * Tema: Paralelismo - OpenMP
 * JTP: Martina, Agustin
 * Facultad de Ciencias Exactas Fisicas y Naturales
 * Universidad Nacional de Cordoba 
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "signal.h"
#include "sample.h"

/*********************** Variables Globales *****************************/
float y[1250500]={0};				// Declaracion y seteo de la variable Y a 0.
float z[1250499]={0};				// Declaracion y seteo de la variable Z a 0.

/********************* Declaracion de Funciones *************************/
void matchFilter(int cant);
void PPP(int cant);

int main(int argc, char **argv)
{
	char lectura[10];
	int cant_threads;
	printf("   Ingrese la cantidad de hilos deseados: ");
	fgets( lectura, 10, stdin );			// El usuario ingresa la cantidad de hilos que desea ejecutar
	cant_threads = atoi(lectura);			// Convierte la cantidad de hilos a un numero entero
	
	matchFilter(cant_threads);					// Llamada a la funcion matchFilter.
	PPP(cant_threads);							// Llamada a la funcion PPP.

	return 0;
}

/********************************* Definicion de Funciones **************************************/

/*
 * Este metodo realiza el filtrado de dos señales mediante
 * una convolucion entre las mismas.
 * @param cantidad de hilos que va a usar
 * @return void
 */
void matchFilter(int cant){
	double start,finish;			// Se definen las variables de tiempo inicial y tiempo final. 
	start = omp_get_wtime();
	
	int i,n;							// Variables usadas para los ciclos for.
	int cantY = 500 + 1250000 -1;		// Cantidad de datos que tendrá el vector Y.
	int cantSIGNAL = 1250000;   		// Cantidad de datos que tiene el vector Signal (x).
	int cantSAMPLE = 500;       		// Cantidad de datos que tiene el vector Sample (h).
	int chunk;
	int cant_threads = cant;
	
	//for(i=0;i<cantY;i++) y[i]=0;
	
	/* La sentencia "#pragma omp parallel" hace que el hilo master que viene ejecutando 
	 * el programa serialmente, cree tantos hilos como la sentencia num_threads(n) le indique. 
	 * La clausula "shared(y)" indica que la variable "y" es compartida por todos los hilos.
	 * La clausula "private(i,n)" indica que cada thread tiene una copia de las variables "i" y "n".
	 */
	#pragma omp parallel shared(y) private(n,i)	num_threads(cant_threads)			
	{
		chunk = cantY / omp_get_num_threads();				// "chunk" guarda el numero de iteraciones del primer ciclo for
															// que realizará cada hilo.
															
		/* La sentencia "#pragma omp for" indica que el siguiente loop sera paralelizado. 
		 * La clausula "schedule(dynamic, chunk)" indica que cada hilo tendra asignado 
		 * una cantidad de iteraciones inicialmente, pero cuando finalice pedira una 
		 * nueva porcion de iteraciones, que se las asignara el propio SO.
		 */
		#pragma omp for schedule(dynamic,chunk)
			/* Ciclos for anidados donde se recorreran los vectores SIGNAL y SAMPLE para obtener, mediante
			 * convolucion, la señal a la salida del filtro denominada Y[n]. 
			 */
			for(n=0;n<cantSIGNAL; n++){						// FOR que recorre todo el vector SIGNAL
				for(i=0;i<cantSAMPLE;i++){					// FOR que recorre todo el vector SAMPLE
					y[n+i]=y[n+i]+signal[n]*sample[i];		// Formula para calcular la convolucion entre dos señales.	
				}
			}
	}
	finish = omp_get_wtime();
	printf("   Convolucion: %g seg\n", finish-start);
}

/*
 * Este metodo realiza el procesamiento de par de pulsos
 * mediante la correlacion de una señal consigo misma, es decir,
 * mediante autocorrelacion.
 * @param cantidad de hilos que va a usar
 * @return void
 */
void PPP(int cant){
	double start,finish;				// Se definen las variables de tiempo inicial y tiempo final. 
	start = omp_get_wtime();
	int i,n;							// Variables usadas para los ciclos for.
	int M=25000;						// Cantidad de muestras transmitidas
	int cantZ = 1250498;				// Cantidad de elementos del vector Z
	int chunk;						
	int cant_threads = cant;			// Cantidad de threads usados.

	
	//for(i=0;i<cantZ;i++) z[i]=0;
	
	
	/* La sentencia "#pragma omp parallel" hace que el hilo master que viene ejecutando 
	 * el programa serialmente, cree tantos hilos como la sentencia num_threads(n) le indique. 
	 * La clausula "shared(y)" indica que la variable "y" es compartida por todos los hilos.
	 * La clausula "private(i,n)" indica que cada thread tiene una copia de las variables "i" y "n".
	 */
	#pragma omp parallel private(n,i) shared(z) num_threads(cant_threads)
	{
		chunk = cantZ / omp_get_num_threads();		// "chunk" guarda el numero de iteraciones del primer ciclo for
														// que realizará cada hilo.
														
	/* La sentencia "#pragma omp for" indica que el siguiente loop sera paralelizado. 
	 * La clausula "schedule(dynamic, chunk)" indica que cada hilo tendra asignado 
	 * una cantidad de iteraciones inicialmente, pero cuando finalice pedira una 
	 * nueva porcion de iteraciones, que se las asignara el propio SO.
	 */
		#pragma omp for schedule(dynamic, chunk)
			/* Ciclos for anidados donde se recorreran el vector Y[n] para obtener, mediante
			 * autocorrelacion, la señal Z[n]. 
			 */
			for(n=0;n<cantZ;n++){
				for(i=0;i<M-2;i++){
					z[n]= z[n] + ( y[n+1] * y[i] ); 			// Formula de autocorrelacion
				}
			}
	}
															//Fin de bloque paralelizable.
	
	
	FILE *archivo;								// Variable que declara un fichero
	remove("PPP_paralelo.txt");					// Borra un archivo previo para guardar informacion nueva.
	archivo = fopen("PPP_paralelo.txt", "w");	// Abre el archivo donde guardara la informacion obtenida.
	if (archivo == NULL){
		printf("\nError de apertura del archivo. \n\n");
	}
	else{
		for(i=0;i<cantZ;i++) 
			fprintf(archivo, "%f\n", z[i]);		// Guarda los datos de Z[n] en el archivo "PPP_paralelo.txt"
	}
	fclose(archivo);							// Cierra el archivo
	finish = omp_get_wtime();
	printf("   Correlacion: %g seg\n", finish-start);
}
