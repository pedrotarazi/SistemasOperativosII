#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	printf(
			"Context-type:text/html\n\n"
			"<head>\n"
				"<title>\n"
					"Subir Modulo\n"
				"</title>\n"
			"</head>\n"
			"<body bgcolor='FF3333'>\n"
				"<header>\n"
					"<hgroup>\n"
						"<h1 style='text-align:center;'><u><b>SISTEMAS OPERATIVOS II</b></u></h1>\n"
						"<h2 style='text-align:center;'><b>Subir Modulo a Instalar</b></h2>\n"
					"</hgroup>\n"
				"</header>\n"
				"<form action='upload_archivo.cgi' method='post' enctype='multipart/form-data'>\n"
				"<h3>Seleccione el archivo:<br /></h3>"
				"<input name='archivo' type='file'><br>\n"
				"<input type='submit' value='Enviar'>\n"
				"</form>\n"
				"<br><h4><i>Presione aqui para ver los modulos instalados en la Intel Galileo</h4></i>"                                          
                                "<a href='lista_modulos.cgi'><input type='button' value= 'Modulos Instalados'></a><br>\n"
				"<br><h4><i>Presione para desinstalar el modulo previamente instalado</h4></i>"
				"<a href='desinstalar_modulo.cgi'><input type='button' value='Desinstalar Modulo'></a><br></br>\n"
				
			"</body>\n"
		"</html>\n"	
	);
	
	return 0;
}
