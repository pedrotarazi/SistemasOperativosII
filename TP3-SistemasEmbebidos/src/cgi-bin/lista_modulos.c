#include <stdio.h>                                   
#include <string.h>                                 
                                                    
void lista();                                                                                                           
int main(int argc, char **argv)                                                                              
{                                                                                                                           
        printf(                                                                                                             
                        "Context-type:text/html\n\n"                                                         
                        "<head>\n"                   
                                "<title>\n"                                                                                 
                                        "Modulos\n"                                                                       
                                "</title>\n"                                                                 
                        "</head>\n"                  
 	                       "<body bgcolor='80B2A3'>\n"                                                                                          
                                "<header>\n"                                                                                
                                        "<hgroup>\n"                                                         
                                                "<h1 style='text-align:center;'><u><b>SISTEMAS OPERATIVOS II</b></u></h1>\n"
                                                "<h2 style='text-align:center;'><b>Modulos Instalados</b></h2>\n"
                                        "</hgroup>\n"
                                "</header>\n"
                                "<H5>");
        lista();                           
        printf( "</H5>\n"    
                        "</body>\n"                           
                "</html>\n"                    
        );                                                    
        return 0;                                             
}  

void lista(){
	FILE* mod;
	char buffer [500];
	mod = fopen("/proc/modules", "r");
	while (!feof(mod)){
		fgets(buffer,500,mod);
		printf("<h4><i><font color='maroon'>%s</i></font><h4>", buffer);
	}
	fclose(mod);
}	
