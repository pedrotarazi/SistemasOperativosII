#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

void get_datta();
int main(int argc, char **argv)
{
	printf(
			"Context-type:text/html\n\n"
			"<head>\n"
				"<title>\n"
					"get_datta\n"
				"</title>\n"
			"</head>\n"
			"<body bgcolor='FEBF67'>\n"
				"<header>\n"
					"<hgroup>\n"
						"<h1 style='text-align:center;'><u><b>SISTEMAS OPERATIVOS II</b></u></h1>\n"
						"<h2 style='text-align:center;'><b>Datos Sensados</b></h2>\n"
					"</hgroup>\n"
				"</header>\n"
				"<H5>");
	get_datta();
	printf(	"</H5>\n"
			"</body>\n"
		"</html>\n"	
	);
	return 0;
}

void get_datta(){
	FILE *archivo;
	int fin=0;
	char buf[256];
	memset(buf,'\0',256);

	key_t Llave;
	int semaforo;
	struct sembuf Operacion;

	//Obtiene la clave del semaforo. El primer parametro es el nombre y path de un 
	//fichero cualquiera que exista, y el segundo un entero cualquiera. 
	//Todas las llaves deben tener los mismos parametros.  
	Llave = ftok ("/bin/ls", 33);
	if (Llave == (key_t)-1){
		printf("No pudo conseguir llave de semaforo\n");
		return;
	}
	//Esta funcion nos permite obtener un array de semaforos. 
	//El primer parametro es la llave anterior, el segundo es el numero 
	//de semaforos que queremos, y el tercero le da permisos de acceso a los semaforos. 
	//0600|IPC_CREAT indica permiso de lectura y escritura para el propietario y 
	//que los semáforos se creen si no lo están al llamar a la funcion.
	semaforo = semget(Llave, 1, IPC_CREAT |0600);
	if( semaforo==-1)
	{
		printf("No puedo crear semaforo\n");
		return;
	}
	//Inicializacion del semaforo. El primer parametro es el semaforo a inicializar, 
	//el segundo el id del semaforo en el arreglo, el tercero avisa que inicializa y 
	//el cuarto es el valor inicial del semaforo (0 en rojo, 1 en verde) 
	semctl(semaforo,1,SETVAL,0);

	Operacion.sem_num=0;              //es el índice del array del semáforo sobre el que queremos actuar
        Operacion.sem_flg=0;              //flags que afectan a la operación                  
        Operacion.sem_op=1;               //valor en el que queremos incrementar el semáforo                      

        semop(semaforo,&Operacion,1);     //Accion del semaforo. Toma el semaforo
	archivo = fopen ( "memoria.csv", "r" ); 
	if (archivo == NULL){
		printf("  Error de apertura del archivo. \n");
	}
	else{
		while(fin==0){
			if(!feof(archivo)){				//se asegura de que no haya llegado al final del archivo 
				fgets(buf,150,archivo);			//lee una linea y la guarda.
				printf("<h3><i><font color='olive'>%s</font></i><h3>", buf);
			}
			else fin=1;			
		}
	}
	fclose(archivo);
	Operacion.sem_op=-1;               //decrementa el semaforo                                         
        semop(semaforo, &Operacion,1);    //Accion del semaforo. Libera el semaforo  
}
