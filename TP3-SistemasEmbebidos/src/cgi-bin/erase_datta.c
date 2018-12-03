#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

void erase_datta();
int main(int argc, char **argv)
{
	//Esto imprime la estructura html, y el navegador lo como como si fuera un html
	printf(
		"Context-type:text/html\n\n"
		"<head>\n"
		"<title>\n"
		"erase_datta\n"
		"</title>\n"
		"</head>\n"
		"<body bgcolor='D7FCC2'>\n"
		"<header>\n"
		"<hgroup>\n"
		"<h1 style='text-align:center;'><u><b>SISTEMAS OPERATIVOS II</b></u></h1>\n"
		"<h2 style='text-align:center;'><b>Borrar Datos</b></h2>\n"
		"</hgroup>\n"
		"</header>\n"
		"<H2>"
		);
	erase_datta();
	printf(
		"</H2>\n"
		"</body>\n"
		"</html>\n"	
	);
	return 0;
}

void erase_datta(){
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

	int borrar;
	borrar = remove("memoria.csv");		
	if(borrar==0) printf("<h3><font color='teal'>El archivo fue borrado con exito</font></h3>");
	else printf("<h3><font color='teal'>No se pudo borrar el archivo</font></h3>");

        Operacion.sem_op=-1;               //decrementa el semaforo                                         
        semop(semaforo, &Operacion,1);    //Accion del semaforo. Libera el semaforo                                          
}
