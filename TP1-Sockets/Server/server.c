/*	
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
 * automáticas (AWS por sus siglas en inglés), que serán instaladas portodo el territorio nacional. Cada AWS 
 * consta de una serie de sensores (GPS, humedad, temperatura, etc), conectado a un sistema de adquisición de
 * datos, que toma medidas de los sensores (telemetría) cada un segundo.
 * 		El SMN solicita a la Escuela de Ingeniería en Computación que diseñe, implemente y testee el software 
 * (desarrollado en C), necesario para realizar la conexión, control y transferencia de datos telemétricos 
 * entre el COP y todas las AWS, utilizando una arquitectura cliente servidor.
 * 
 * Año: 2016
 * 
 * Este codigo representa al AWS, aqui llamado "server".
 */

/********************* LIBRERIAS ********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

/******************** Variables globales y/o constates **********/
#define TAM 256
#define B_GREEN "\E[32;1m"
#define WHITE "\E[0;37m"
#define B_BLUE "\E[34;1m"
#define B_RED "\E[31;1m"

/************************** Declaracion de Funciones ***************************/
int comandoERASE_DATTA();
void verificarLectura(int n);
void verificarEscritura(int n);

/************************** Main *******************************/
int main( int argc, char *argv[] ) {
	
	/****************** Declaracion de variables ******************/
	int sockfd, newsockfd, puerto, clilen;
	char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	clock_t t_ini, t_fin;
	double secs;
	//Verifica que lo que se ingreso por teclado sea el comando de ejecucion y el puerto.
	if ( argc < 2 ) {
		fprintf( stderr, "Uso: %s <puerto>\n", argv[0] );
		exit (1);
	}
	
	/**************************************** CONEXION TCP ***********************************/
	sockfd = socket( AF_INET, SOCK_STREAM, 0);		//La funcion socket devuelve un >0 si la conexion fue exitosa, y <0 si no.
	if(sockfd<0){
		perror("Error en la apertura de socket");
		exit(1);
	}
	
	
	memset( &serv_addr, 0, sizeof(serv_addr) );		//Limpia estructura serv_addr. Copia el valor 0 en la direccion de serv_addr
	puerto = atoi ( argv[1] );						//Convierte el puerto que le pasamos a entero
	serv_addr.sin_family = AF_INET;					//En la estructura serv_addr, asigna la familia de direcciones 
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons (puerto);			//Convierte bytes de hosts a bytes de red
	
	//Si la funcion bind entrega un valor =0, la ligadura fue exitosa. Si es <0, la ligadura fue erronea.
	if( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0) {
		perror( "ligadura" );
		exit(1);
	}
	

	

	
	//Bucle Principal
	while (1){
		printf( B_GREEN"  >>$ "WHITE"Socket disponible: %d\n", ntohs(serv_addr.sin_port));
		listen (sockfd, 1);							//Escucha al socket. El segundo argumento indica el numero maximo de conexiones simultaneas
		clilen = sizeof(cli_addr);					//Guarda el tamaño de la direccion del cliente
		
		newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &clilen);		//Devuelve 0 si fue exitoso, si no devuelve -1.
		if ( newsockfd < 0){
			perror ("accept");
			exit(1);
		}
		memset(buffer, 0, TAM); 										//Limpia buffer
		n = read(newsockfd, buffer, TAM-1); 							//Lee el nombre de quien se conecto
		verificarLectura(n);
		printf( B_GREEN"  >>$ "WHITE"Cliente '%s' conectado...\n", buffer);
		int s=1;		
		while(s==1){	
			memset(buffer, 0, TAM); 					//Limpia buffer
			n = read(newsockfd, buffer, TAM-1); 		//Lee el comando enviado por el cliente
			verificarLectura(n);
			
			if(s==0)
				close(newsockfd);						//Finaliza la conexion
				
			printf(B_GREEN"  >>$ "WHITE"Recibi: %s\n", buffer);			//Imprime lo que recibio
			
			/*
			 * Si recibio "disconnect", le manda al cliente el comando aceptando su desconexion
			 */
			if( !strcmp("disconnect", buffer )){
				printf(B_BLUE"     >>$ "WHITE"Como recibi 'disconnect', termino la ejecucion.\n\n");
				n=write(newsockfd, "disconnect", 18); 
				s=0;
			}
			
			/*
			 * Si recibio "get_telemetry", abre el archivo, lee la ultima linea, y
			 * le envia el dato al cliente en caso que pueda hacerlo. Si no puede leer
			 * el dato, aviso mediante un comando en particular.
			 */
			else if( strcmp("get_telemetry", buffer )==0){
				FILE *archivo;
				char bufAux[TAM];
				int i, linea=2;
				memset(bufAux,'\0',TAM);
				archivo = fopen ( "memoria.csv", "r" ); 
				if (archivo == NULL){
					printf("  Error de apertura del archivo. \n");
					n = write(newsockfd, "no hay datos", 12);
				}
				else{
					while(!feof(archivo)){					//se asegura de que no haya llegado al final del archivo 
						fgets(bufAux,150,archivo);			//lee una linea y la guarda.
						linea++;							//cuenta la cantidad de lineas del archivo
					}
					if(linea>0)	{					
						for(i = 0; i < linea ; i++){		//lee línea por línea guardándola en la variable array hasta llegar a la línea especifica
							if(!feof(archivo)){					//se asegura de que no haya llegado al final del archivo 
								fgets(bufAux,150,archivo);		//lee una linea y la guarda
							}
						}
					}
					printf(B_BLUE"     >>$ "WHITE"%s", bufAux);			//Imprime el dato que va a enviar
					memset(buffer,0,TAM);
					n=write(newsockfd, "dato", 4);	//Envia el comando "dato" para avisarle al cliente que lo proximo a enviar va a ser el dato pedido
					verificarEscritura(n);
					memset(buffer,0,TAM);
					n=read(newsockfd, buffer, TAM-1);	//Lee el "OK" del cliente.
					verificarLectura(n);
					if( strcmp("OK",buffer)==0){
						n=write(newsockfd, bufAux, 150); //Envia el dato
						verificarEscritura(n);
						printf(B_BLUE"     >>$ "WHITE"El ultimo dato sensado fue enviado.\n");
					}
					else{
						n=write(newsockfd, "error", 5);		//Envia error si no recibe "OK"
					}
					fclose ( archivo );
				}
			}				
			
			/*
			 * Si recibio "list", envia el aviso de que va enviar la lista de comandos aceptados,
			 * verifica que el aviso fue recibido leyendo un "OK" en el socket, y luego envia la lista.
			 */
			else if( !strcmp("list", buffer) ){
				printf(B_BLUE"     >>$ "WHITE"Devuelvo el listado de comandos posibles.\n");
				n=write(newsockfd, "list",4);
				verificarEscritura(n);
				memset(buffer, 0, TAM);
				n=read(newsockfd,buffer,TAM-1);
				verificarLectura(n);
				if( strcmp("OK",buffer)==0){
					n=write(newsockfd, (
						"       Comandos posibles: \n"
						"         1- disconnect\n"
						"         2- get_telemetry\n"
						"         3- get_datta\n"
						"         4- erase_datta\n"), 150);
					verificarEscritura(n);
				}	
			}
			
			/*
			 * Si recibio "erase_datta", el server borra su memoria interna. La misma contiene los datos sensados.
			 * Le avisa al cliente si se pudieron borrar o no.
			 */
			else if( !strcmp("erase_datta", buffer) ){
				printf(B_BLUE"     >>$ "WHITE"Debo borrar datos.\n");
				int borrar;
				borrar=comandoERASE_DATTA();
				if(borrar==0)
					n=write(newsockfd, "borrado", 18);
				else
					n=write(newsockfd, "no_borrado", 18);
			}
			
			/*
			 * Si recibio "get_datta", el serverTCP crea un serverUDP, le avisa al clienteTCP para que éste
			 * cree un clienteUDP, y le envia el archivo.
			 */
			else if( !strcmp("get_datta", buffer)){
				printf(B_BLUE"     >>$ "WHITE"Enviar archivo mediante conexion no segura.\n");
				printf(B_RED"        >>$ "WHITE"Server UDP\n");
				
				/****** Declaracion de variables *******/
				int sockfdUDP, tamano_direccion;
				char bufferUDP[TAM];
				struct sockaddr_in serv_addrUDP;
				int aux_puerto;
				int iniciar=1;
				
				/*************************** INICIALIZACION DEL SERVER UDP *********************/
				sockfdUDP = socket(AF_INET, SOCK_DGRAM, 0);				
				if (sockfd < 0) { 
					perror("ERROR en apertura de socket");
					exit( 1 );
				}
				aux_puerto= puerto;
				puerto = 6020;
				memset( &serv_addrUDP, 0, sizeof(serv_addrUDP) );		//Obtiene el tamaño de la direccion de destino
				serv_addrUDP.sin_family = AF_INET;						//En la estructura dest_addrUDP, asigna la familia de direcciones 
				serv_addrUDP.sin_addr.s_addr = INADDR_ANY;
				serv_addrUDP.sin_port = htons( puerto );				//Convierte bytes de hosts a bytes de red
				memset( &(serv_addrUDP.sin_zero), '\0', 8 );			//Limpia estructura dest_addrUDP. Copia el valor 0 en la direccion de dest_addrUDP
				
				if( bind( sockfdUDP, (struct sockaddr *) &serv_addrUDP, sizeof(serv_addrUDP) ) < 0 ) {
					perror( "ERROR en binding" );
					exit( 1 );
				}

				printf( B_RED"        >>$ "WHITE"Socket UDP disponible: %d\n", ntohs(serv_addrUDP.sin_port) );

				tamano_direccion = sizeof( struct sockaddr );
				memset(buffer, 0, TAM); 								//Limpia buffer
				n = write(newsockfd, "serverUDP_UP", 12); 				//Aviso al cliente para que inicie el cliente UDP
				iniciar=1;
				while ( iniciar==1 ) {
					memset( bufferUDP, 0, TAM );
					//Lee lo que le envio el cliente (un "ok")
					n = recvfrom( sockfdUDP, bufferUDP, TAM-1, 0, (struct sockaddr *)&serv_addrUDP, &tamano_direccion );
					verificarLectura(n);
					
					/****************** ENVIO DE ARCHIVO ************************/					
					FILE *archivo;
					
		
					archivo = fopen ( "memoria.csv", "r" ); 
					t_ini = clock();
					if (archivo == NULL){
						printf("  Error de apertura del archivo. \n");
						iniciar=0;
						close(sockfdUDP);
						printf(B_RED"        >>$ "WHITE"Socket UDP cerrado.\n");
						n = sendto( sockfdUDP, (void *)"err", 3, 0, (struct sockaddr *)&serv_addrUDP, tamano_direccion  );
					}
					else{
						while(!feof(archivo)){					//se asegura de que no haya llegado al final del archivo 
							memset( bufferUDP, 0, TAM );
							fgets(bufferUDP,150,archivo);		//obtiene una linea
							//Envia la linea leida
							n = sendto( sockfdUDP, (void *)bufferUDP, 150, 0, (struct sockaddr *)&serv_addrUDP, tamano_direccion  );
							memset( bufferUDP, 0, TAM );
							verificarEscritura(n);
							//Esperar recibir "ok"
							//n = recvfrom( sockfdUDP, bufferUDP, TAM-1, 0, (struct sockaddr *)&serv_addrUDP, &tamano_direccion );
							//memset( bufferUDP, 0, TAM );
							//verificarLectura(n);
						}
						fclose ( archivo );
						t_fin = clock();
						//Envia "fin" para que el cliente cierre el archivo
						n = sendto( sockfdUDP, (void *)"fin", 3, 0, (struct sockaddr *)&serv_addrUDP, tamano_direccion  );
						verificarEscritura(n);
						secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
						/************************** FIN DE ENVIO DE ARCHIVO ********************/
						//remove("memoria.csv");							//Borra el archivo porque lo envio
						close(sockfdUDP);								//Cierra el socket
						puerto=aux_puerto;								//Retoma el puerto
						printf(B_RED"        >>$ "WHITE"Socket UDP cerrado.\n");
						printf(B_BLUE"     >>$ "WHITE"Archivo enviado en %.16g milisegundos.\n",secs * 1000.0);
						iniciar=0;
						/************************ FIN DEL SERVER UDP ****************************/
					}
				}
				memset(bufferUDP,0, strlen(buffer));
			}		
			
			/*
			 * Si recibe algun comando no aceptado, envio mensaje de error.
			 */
			else{
				n=write(newsockfd, "commandError", 18); //Devuelve la cantidad de caracteres transmitidos, o -1 si no transmitió ninguno
				verificarEscritura(n);
			}
		}
	}
	return 0;
}

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
 * Intenta borrar la memoria intermedia.
 * @return Retorna un 0 si se pudo borrar el archivo, o un 1 si no se pudo borrar
 */
int comandoERASE_DATTA(){
	int erase;
	erase=remove("memoria.csv");
	if(erase==0){
		printf(B_BLUE"     >>$ "WHITE"Borrado exitoso.\n");
	}
	else
		printf(B_BLUE"     >>$ "WHITE"Borrado fallido.\n");
	return erase;
}
