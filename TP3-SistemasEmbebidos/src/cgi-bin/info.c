#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void info();
void ExtraerDato();
void CPU_Info();
void Kernel();
void TiempoTranscurrido();
void Procesos();
void Info_Memoria();

int main(int argc, char **argv)
{
	printf(
			"Context-type:text/html\n\n"
			"<head>\n"
				"<title>\n"
					"Informacion del Sistema\n"
				"</title>\n"
			"</head>\n"
			"<body bgcolor='69FB2F'>\n"
				"<header>\n"
					"<hgroup>\n"
						"<h1 style='text-align:center;'><u><b>SISTEMAS OPERATIVOS II</b></u></h1>\n"
						"<h2 style='text-align:center;'><b>Informacion del Sistema</b></h2>\n"
					"</hgroup>\n"
				"</header>\n"
				);
			info();
	printf(		
			"</body>\n"
		"</html>\n"	
	);
	return 0;
}

void info(){
	printf("<div style='padding: 15px 200px; float: left; width: 23%';>");
	CPU_Info();
	printf("</div><div style='padding: 15px 50px; float: right; width: 35%;'>");
	Kernel();
	Info_Memoria();
	TiempoTranscurrido();
	printf("</div><div style='clear: both'></div>"); 
}

/*
 * Funcion para extraer el dato de un archivo
 * @param filename Direccion absoluta del archivo
 * @param cadena Puntero donde guardara el dato	
 * @param comienzo Puntero que indica el comienzo del dato requerido
 */
 
void Extraer_Dato (char* filename, char* cadena, char* comienzo)
{
	FILE* fd;					
	char *leer = NULL;
	char buffer [500];
	fd = fopen (filename, "r");
	while (feof (fd) == 0)				//Mientras no se llega al final del fichero
	{
		fgets (buffer, 500, fd);		//Obtiene una linea y la guarda en buffer
		leer = strstr (buffer, comienzo);	//Busca en buffer lo indicado por comienzo
		if (leer != NULL)			//Si lo encuentra, sale del bucle
		break;
	}
	fclose (fd);
	strcpy (cadena, leer);				//Copia leer en cadena y sale de la funcion
	return;
}
/*
 * Funcion que convierte el tiempo de segundos a horas:minutos:segundos
 * @param secs Tiempo en segundos
 * @param buf Buffer donde se guardara el resultado
 */
void Conversor_Tiempo (float secs, char* buf)
{
	unsigned int d, h, m;
	float s;
    d = (int) (secs / 86400);
    secs = secs - (long) (d * 86400);
    h = (int) (secs / 3600);
    secs = secs - (long) (h * 3600);
    m = (int) (secs / 60);
    secs = secs - (long) (m * 60);
    s = secs;
    if (d > 0)
		sprintf (buf, "%3ud %2u:%02u:%02.2f\n", d, h, m, secs);
    else
		sprintf (buf, "%2u:%02u:%02.2f\n", h, m, s);
	return;
}

/*
 * Funcion que obtiene la informacion del sistema.
 */
void CPU_Info()
{
printf("<h3><i><u><font color='red'>INFORMACION DE CPU</font></u></i></h3><h5>\n");

	FILE* fd;
	char buffer [500];
	fd = fopen("/proc/cpuinfo", "r");
	while(!feof(fd))
	{
		fgets(buffer,500, fd);
		printf("%s<br>",buffer);
	}
	fclose(fd);
	printf("</h5>");
	return;
}

/*
 * Funcion que obtiene la informacion del kernel.
 */
void Kernel()
{
	printf("<h3><i><u><font color='red'>VERSION DEL KERNEL</font></i></u></h3><h5>\n"); 
	FILE *fd; 
	char version [60];
	fd = fopen ("/proc/version", "r"); 
	fscanf (fd, "%[^(]s", version);					
	printf ("Kernel: %s \n", version); 
	fclose (fd);
	printf("</h5><br>\n");
	return;
}

/*
 * Funcion que obtiene el tiempo transcurrido desde que se inicio el sistema
 */
void TiempoTranscurrido()
{
	printf("<h3><i><u><font color='red'>INFORMACION DE TIEMPO</font></i></u></h3><h5>\n"); 
	FILE *fd; 
	float time;
	char tiempo [18];
	fd = fopen ("/proc/uptime", "r"); 
	fscanf (fd, "%f", &time);
	Conversor_Tiempo (time, tiempo);
	printf ("Tiempo transcurrido desde que se inicio el Sistema Operativo: %s", tiempo); 
	fclose (fd);
	printf("</h5><br>");
	return;
}

/*
 * Funcion que obtiene la informacion de la memoria del sistema.
 */
void Info_Memoria()
{
	printf("<h3><i><u><font color='red'>INFORMACION DE MEMORIA</font></i></u></h3><h5>\n");
	char cadena [256];
	unsigned int MemTotal, MemFree;
	Extraer_Dato ("/proc/meminfo", cadena, "MemTotal");
	sscanf (cadena, "MemTotal: %u", &MemTotal);	
	printf ("Memoria total: %u MB<br>\n", MemTotal/1024);
	Extraer_Dato ("/proc/meminfo", cadena, "MemFree");
	sscanf (cadena, "MemFree: %u", &MemFree);	
	printf ("Memoria disponible: %u MB<br>\n", MemFree/1024);
	printf("</h5><br>\n");
	return;
}

