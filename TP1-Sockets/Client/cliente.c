/*	COP		COP		COP		COP		COP		COP		COP		COP		COP		COP		COP		COP		COP		COP		COP		COP		COP		COP		COP		
 * Alumno: TARAZI PEDRO ESEQUIEL
 * Matricula: 35035736
 * Materia: Sistemas Operativos II
 * Jefe de TP: MARTINA, AGUSTIN
 * Carrera: Ingrenieria en Computacion
 * Facultad de Ciencias Exactas, Fisicas y Naturales
 * Universidad Nacional de Cordoba
 * 
 *	Trabajo Practico N°1
 * Tema: Sockets
 * 
 * 		El Servicio Meteorológico Nacional (SMN) ha adquirido una serie de 25 estaciones meteorológicas
 * automáticas (AWS por sus siglas en inglés), que serán instaladas por todo el territorio nacional. Cada AWS 
 * consta de una serie de sensores (GPS, humedad, temperatura, etc), conectado a un sistema de adquisición de
 * datos, que toma medidas de los sensores (telemetría) cada un segundo.
 * 		El SMN solicita a la Escuela de Ingeniería en Computación que diseñe, implemente y testee el software 
 * (desarrollado en C), necesario para realizar la conexión, control y transferencia de datos telemétricos 
 * entre el COP y todas las AWS, utilizando una arquitectura cliente servidor.
 * 
 * Año: 2016
 * 
 * Este codigo representa al Centro de Operaciones (COP), aqui llamado "cliente".
 */
 
/********************* LIBRERIAS ********************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

/******************** Variables globales y/o constates **********/
#define TAM 256
#define B_BLUE "\E[34;1m"
#define B_RED "\E[31;1m"
#define WHITE "\E[0;37m"
char ip[TAM];				//Contiene la direccion IP

/************************** Declaracion de Funciones ***************************/
int contarPalabras(char* array);
int primeraPalabra(char* array);
int leerPuerto(char* array);
int leerIP(char* array);
void verificarLectura(int n);
void verificarEscritura(int n);

/************************** Main *******************************/
int main( int argc, char *argv[] ) {
	
	/****************** Declaracion de variables ******************/
	int sockfd, puerto, n;
	struct sockaddr_in serv_addr;	//Estructuara que mantiene la informacion
									//de direcciones de socket.
	struct hostent *server;
	int cant, priPal, val;
	char buffer[TAM];
	
	// Bucle principal
	while( 1 ){
		/* 
		 * Bucle para imprimir el prompt cada vez que se ingresa mal el comando de conexion, 
		 * o la IP ingresada no es valida, o el puerto no es el correcto, etc.
		 */
		while(1){
			printf(B_BLUE"   >>%s$ "WHITE, getcwd(NULL,PATH_MAX)); //Lee directorio actual e imprimie el PROMPT.
			memset( buffer, '\0', TAM );							//Limpia buffer.
			fgets( buffer, TAM-1, stdin );							//Lee teclado.
			buffer[strlen(buffer)-1] = '\0';						//Completa el buffer con caracteres NULL.
			cant = contarPalabras(buffer);			//Cuenta palabras ingresadas por el CLIENTE.
			priPal = primeraPalabra(buffer);		//Controla si la primera palabra es "connect".
			val = leerIP(buffer);					//'val' indica si la IP tiene formato valido o no. 
			puerto = leerPuerto(buffer);			//Contiene el puerto ingresado.
			
			if(priPal==2){ 							//Si se ingresó "exit", se cierra el programa.
				return 0;
			}
			else{									//Si no se ingresó "exit"
				/*
				 * Verifica si se ingresaron menos de 3 palabras, o si
				 * se ingresó mal la palabra "connect", o si la IP es invalida,
				 * o si el puerto no esta en el rango correcto.
				 */
				if(cant < 3 || priPal==0 || val<2 || puerto==0){		
					if(priPal==0)
						fprintf( stderr, "   Use 'connect ip port' o 'exit'\n");	
					if(priPal==1 && cant<3)
						fprintf( stderr, "   Use 'connect ip port'\n");
					if(val==0)
						printf("   Error en el formato de IP. Una IP esta formada por 4 bytes.\n");
					if(val==1)
						printf("   Error en el formato de la IP. Una IP tiene numeros entre 0 y 255.\n");
					if(puerto==0)
						printf("   El puerto debe ser entre 6020 y 6044.\n");
				}
				else 								//Si lo anterior no se cumple, sigue la ejecucion del programa.
					break;
			}
		}
		
		/************************************* CONEXION TCP *************************************/		
		sockfd = socket( AF_INET, SOCK_STREAM, 0 );			//Crea el socket. Si la conexion fue exitosa, devuelve >0. Si no, <0.
		if ( sockfd < 0 ) {
			perror( "  ERROR apertura de socket" );
			exit( 1 );
		}
		
		server = gethostbyname(ip);				//Retorna informacion sobre el host "ip". Si falla, retorna NULL.
		if (server == NULL) {
			fprintf( stderr,"    Error, no existe el host\n" );
			exit( 0 );
		}
		
		memset( (char *) &serv_addr, '0', sizeof(serv_addr) );	//Limpia estructura serv_addr. Copia el valor 0 en la direccion de serv_addr
		serv_addr.sin_family = AF_INET;							//En la estructura serv_addr, asigna la familia de direcciones 
		bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length );	//Copia la direccion IP a la estructura del socket
		serv_addr.sin_port = htons( puerto );					//Convierte bytes de hosts a bytes de red y lo guarda como numero de puerto
			
		//La funcion connect devuelve 0 si fue exitosa, -1 en caso contrario
		if ( connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {
			perror( "   conexion" );
			exit( 1 );
		}
		
		printf("   Conectado al Server con IP %s mediante el puerto %d\n", ip, puerto );	//Informacion sobre el estado de la conexion.
		n = write( sockfd, "AWS_Quines", strlen(buffer) );		//Envia al Server el nombre de quien quiere conectarse.
		verificarEscritura(n);									//Verifica si se escribio en el socket.
		memset( buffer, '\0', TAM );							//Limpia el buffer.
		
		int termina=0;
		while(termina==0) {
			printf(B_RED"      >>%s-%d$ "WHITE,"AWSQuines",puerto);
			memset( buffer, '\0', TAM );		//Limpia el buffer
			fgets( buffer, TAM-1, stdin );		//Lee los que ingreso el cliente
			buffer[strlen(buffer)-1] = '\0';	//Completa con caracteres nulos
			
			n = write( sockfd, buffer, strlen(buffer) );	//Escribe al Server lo que se ingresó por teclado
			verificarEscritura(n);							//Verifica si se escribio correctamente en el socket
			memset(buffer,0, TAM);							//Limpia el buffer
			n = read( sockfd, buffer, TAM-1); 				//Lee la respuesta del Server
			verificarLectura(n);							//Verifica si se leyó correctamente desde el socket.
			
			/*********************** Comandos esperados cuando se escribe "disconnect" ***********************/
			/*
			 * Si recibe "disconnect", el cliente finaliza la conexion. 
			 */
			if( strcmp( "disconnect", buffer) == 0 ){ 					//Compara comando ingresado. Comando 'disconnect'	
				printf("      Hasta luego. Ejecucion finalizada\n\n" );
				termina=1;
			}
			
			/*********************** Comandos esperados cuando se escribe "list" *****************************/
			/*
			 * Si recibe "list", imprime la lista de comandos posibles que acepta el Server.
			 */
			else if( strcmp ("list", buffer) ==0 ){						//Compara comando ingresado. Comando 'list'
				n = write( sockfd, "OK", strlen(buffer) );				//Escribe "OK" en el socket para que el Server le envie el listado de comandos
				verificarEscritura(n);
				memset( buffer, '\0', TAM );
				n = read( sockfd, buffer, TAM);							//Recibe los comandos
				verificarLectura(n);
				printf("%s", buffer);									//Imprime los comandos que recibió del Server
			}
			 
			/*********************** Comandos esperados cuando se escribe "get_telemetry" ********************/
			/*
			 * Si recibe "no hay datos", imprime mensaje. El Server no pudo abrir el archivo que contenia los datos.
			 */
			else if( strcmp("no hay datos", buffer)==0 ){
				printf("      No hay datos.\n");
			}
			
			/*
			 * Si recibe "dato", el cliente recibirá el ultimo dato sensado.
			 */
			else if( strcmp("dato", buffer)==0){
				n = write( sockfd, "OK", 2);							//Escribe al Server para que éste le envie el dato.
				verificarEscritura(n);
				memset( buffer, '\0', TAM );
				n = read( sockfd, buffer, TAM );						//Recibe el dato y lo imprime en pantalla.
				printf("      Dato: %s", buffer);
			}
			
			/*
			 * Si recibe "err", imprime mensaje. Error de comunicacion. El Server no recibio el "OK" para que le pueda enviar el dato. 
			 */
			else if( strcmp("error",buffer)==0){
				printf("      Error de comunicacion con el servidor.\n");
			}
			
			/*********************** Comandos esperados cuando se escribe "erase_datta" **********************/
			/*
			 * Si recibe "borrado", se pudo borrar la memoria del AWS. 
			 */
			else if( strcmp("borrado", buffer) == 0){
					printf("      Borrado exitoso.\n");
			}
			
			/*
			 * Si recibe "no_borrado", no se pudo borrar la memoria del AWS.
			 */
			else if( strcmp("no_borrado", buffer) == 0){
					printf("      No se pudo borrar la memoria.\n");
			}
			
			/*********************** Comandos esperados cuando se escribe "get_datta" ************************/				
			/*
			 * Si recibe "serverUDP_UP", el cliente ya puede crear un cliente UDP para poder recibir el dato.
			 * Luego de recibir el dato, se cierra la conexion.
			 */
			else if(strcmp( "serverUDP_UP", buffer) == 0){
				
				/***** Declaracion de Variables *****/
				int sockfdUDP, tamano_direccion;
				struct sockaddr_in dest_addrUDP;
				struct hostent *serverUDP;
				char bufferUDP[TAM];
				int aux_puerto=puerto;									//Guardado auxiliar del puerto.
				puerto=6020;											//Puerto fijo
				serverUDP = server;										//Los datos del server UDP son los mismos que los del serverTCP
				
				/************************* INICIALIZACION DEL CLIENTE UDP **********************************/
				sockfdUDP = socket( AF_INET, SOCK_DGRAM, 0 );			//Crea el socket. Si la conexion fue exitosa, devuelve >0. Si no, <0.
				if (sockfdUDP < 0) {
					perror( "apertura de socket" );
					exit( 1 );
				}

				dest_addrUDP.sin_family = AF_INET;						//En la estructura dest_addrUDP, asigna la familia de direcciones 
				dest_addrUDP.sin_port = htons( puerto );				//Convierte bytes de hosts a bytes de red
				dest_addrUDP.sin_addr = *( (struct in_addr *)serverUDP->h_addr );
				memset( &(dest_addrUDP.sin_zero), '\0', 8 );			//Limpia estructura dest_addrUDP. Copia el valor 0 en la direccion de dest_addrUDP
				memset( bufferUDP, 0, TAM );							//Limpia buffer UDP
				tamano_direccion = sizeof( dest_addrUDP );				//Obtiene el tamaño de la direccion de destino 
				
				//Envia "comenzar" para avisar al servidor que puede empezar a enviar el archivo.
				n = sendto( sockfdUDP, (void *)"comenzar", TAM, 0, (struct sockaddr *)&dest_addrUDP, tamano_direccion );
				verificarEscritura(n);
				n = recvfrom( sockfdUDP, (void *)bufferUDP, TAM, 0, (struct sockaddr *)&dest_addrUDP, &tamano_direccion );
				verificarLectura(n);
				/***************************** RECEPCION DE ARCHIVO ******************************/
				FILE *archivo;
				remove("registros.csv");								//Elimina archivo viejo.
				archivo = fopen ( "registros.csv", "a" ); 				//Abre archivo nuevo.
				if (archivo == NULL){
					printf("\nError de apertura del archivo. \n\n");
				}
				else if(strcmp("err",bufferUDP)==0)
					printf("      No existe ningun archivo en AWS.\n");
				else{	
					while( (strcmp("fin", bufferUDP) != 0) ){			//Mientras no reciba fin, no deja de guardar datos en el archivo
						fputs(bufferUDP, archivo);						//Guarda lo recibido en el archivo.
						memset( bufferUDP, 0, sizeof( bufferUDP ) );	//Limpia bufferUDP.
						//Recibe datos.
						n = recvfrom( sockfdUDP, (void *)bufferUDP, TAM, 0, (struct sockaddr *)&dest_addrUDP, &tamano_direccion );
						verificarLectura(n);
					}
				fclose ( archivo );									//Cuando recibio "fin", cierra el archivo.
				/****************** FIN RECEPCION DE ARCHIVO *************************/
				printf( "      Archivo de datos recibido.\n" );
				puerto=aux_puerto;
				}
			}
				/****************** FIN DEL CLIENTE UDP ******************************/
			
			
			/*********************** Comandos esperados cuando se escribe una palabra invalida ************/
			/*
			 * Si recibe "commandError", significa que el servidor le esta avisando que el comando ingresado no es valido.
			 */
			else if( strcmp("commandError", buffer) ==0){
				printf("      No existe el comando. Ingrese 'list' para ver los comandos disponibles.\n");
			}
		}
	}
	return 0;
} 

/************************* Definicion de Funciones *****************************/
/*
 * Verifica que la lectura del socket fue exitosa.
 * @param n Valor entero que indica si la lectura fue exitosa.
 */
void verificarLectura(int n){
	if(n<0){
		perror("lectura de socket TCP");
		exit(1);
	}
}
/*
 * Verifica que la escritura en el socket fue exitosa.
 * @param n Valor entero que indica si la escritura fue exitosa.
 */
void verificarEscritura(int n){
	if(n<0){
		perror("escritura en socket");
		exit(1);
	}	
}
/*
 * Cuenta la cantidad de palabras ingresadas por teclado
 * @param array Contiene lo que se ingreso por teclado
 * @return Retorna la cantidad de palabras
 */
int contarPalabras(char* array){
	int i;
	int cantidad=1;
	char caracter, caracter_s;
	for(i=0;i<strlen(array)-1;i++){
		caracter = array[i];
		caracter_s = array[i+1];
		if( ( (caracter>96 && caracter<123) || (caracter>47 && caracter<58) || (caracter==46) ) && (caracter_s==32 || caracter_s=='\0') ){
			cantidad++;
		}
	}
	return cantidad;
}
/*
 * Lee el puerto ingresado por teclado
 * @param array Contiene lo que se ingreso por teclado
 * @return Retorna el valor entero del puerto
 */
int leerPuerto(char* array){
	int i, j=0;
	char aux[TAM];
	char caracter;
	memset(aux,'\0',TAM);
	for(i=0;i<strlen(array);i++){
		caracter = array[i];
		if( caracter>47 && caracter<58 ){
			aux[j]=caracter;
			j++;
		}
		if( caracter<48 || caracter>57 ){
			//si hay algun error, agregar memset y el otro ja
			j=0;	
		}
	}
	int puerto = atoi( aux );
	if (puerto<6020 || puerto>6044){
		return 0;
	}
	return puerto;
}
/*
 * Lee la IP ingresada por teclado. Verifica si es valida o no, y la guarda en la variable global "ip"
 * @param array Contiene lo que se ingreso por teclado
 * @return Retorna 0 si la IP no tiene el formato correcto, 1 si la IP tiene numeros no validos, o 2 si la IP es valida
 */
int leerIP(char* array){
	int i, j=0;
	char caracter;
	char caracter_s;
	char control[TAM];
	int empieza=0, termina=0;
	memset( ip, '\0', TAM );
	memset( control, '\0', TAM );
	for(i=0;i<strlen(array)-1;i++){
		caracter = array[i];
		caracter_s = array[i+1];
		if(caracter==32 && caracter_s!=32){
			if(empieza==1){
				termina=1;
			}
			empieza=1;
		}
		if( (caracter>47 && caracter<58) || caracter==46 ){
		//if(empieza==1 && caracter!=32){
			ip[j]=caracter;
			control[j]=caracter;
			j++;
		}
		if(termina==1){
			i=strlen(array);
		}
	}

	int puntos=0;
	for(i=0;i<strlen(ip);i++){
		if(ip[i]==46)
			puntos++;
	}
	if(puntos!=3){
		return 0;
	}
	else{
		char* str;
		int byte;
		str = strtok(control,".");
		//printf("byte: %s\n",str);
		while(str != NULL){
			byte=atoi(str);
			if(byte<0 || byte>255){
				return 1;
			}
			str = strtok( NULL, "." );		
		}
		return 2;
	}
}
/*
 * Devuelvo un entero que indica que palabra es la primera que se ingreso por teclado.
 * @param array Contiene lo que se ingreso por teclado.
 * @return Retorna 0 si la palabra no existe, 1 si la palabra fue "connect", o 2 si la palabra fue "exit".
 */
int primeraPalabra(char* array){
	int i;
	char aux[TAM];
	char caracter;
	memset(aux,'\0',TAM);
	for(i=0;i<strlen(array);i++){
		caracter=array[i];
		if(caracter==32){
			i=strlen(array);
		}
		aux[i]=caracter;
	}

	if(strcmp("connect", aux)==0)
		return 1;
	else if(strcmp("exit", aux)==0)
		return 2;
	else
		return 0;
}

