#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>

int main(int argc, char **argv)
{
	FILE *archivoOrigen, *archivoDestino;
 	char buffer[150];
 	
	key_t Llave;
	int semaforo;
	struct sembuf Operacion;
	//Obtiene la clave del semaforo. El primer parametro es el nombre y path de un 
	//fichero cualquiera que exista, y el segundo un entero cualquiera. 
	//Todas las llaves deben tener los mismos parametros. 
	Llave = ftok ("memoria.csv", 33);
	if( Llave == (key_t)-1){
		printf("No pudo conseguir llave de semaforo\n");
		return 0;
	}
	//Esta funcion nos permite obtener un array de semaforos. 
	//El primer parametro es la llave anterior, el segundo es el numero 
	//de semaforos que queremos, y el tercero le da permisos de acceso a los semaforos. 
	//0600|IPC_CREAT indica permiso de lectura y escritura para el propietario y 
	//que los semáforos se creen si no lo están al llamar a la funcion.
	semaforo = semget(Llave,1,0600|IPC_CREAT);
	if(semaforo==-1){
		printf("No pudo crear semaforo\n");
		return 0;
	}
	//Inicializacion del semaforo. El primer parametro es el semaforo a inicializar, 
	//el segundo el id del semaforo en el arreglo, el tercero avisa que inicializa y 
	//el cuarto es el valor inicial del semaforo (0 en rojo, 1 en verde)
	semctl(semaforo,0,SETVAL,1);
	Operacion.sem_num=0;              //es el índice del array del semáforo sobre el que queremos actuar
        Operacion.sem_flg=0;              //flags que afectan a la operación  

	while(1){		//lee línea por línea guardándola en la variable buffer hasta llegar a la línea especifica
		archivoOrigen = fopen ( "registro.csv", "r" ); 
		fgets(buffer,150,archivoOrigen);
		fgets(buffer,150,archivoOrigen);
		while(!feof(archivoOrigen)){
			Operacion.sem_op=1;              //valor en el que queremos incrementar el semáforo                      
	                semop(semaforo,&Operacion,1);     //Accion del semaforo. Toma el semaforo
			archivoDestino = fopen ( "memoria.csv", "a" );
			printf("Dato...%s",buffer);
			fputs(buffer,archivoDestino);
			fgets(buffer,150,archivoOrigen);
			Operacion.sem_op=-1;               //decrementa el semaforo                                         
        		semop(semaforo, &Operacion,1);    //Accion del semaforo. Libera el semaforo 
			sleep(1);			
			fclose(archivoDestino);
		}
		rewind(archivoOrigen);
	}
	fclose(archivoOrigen);
	
}
