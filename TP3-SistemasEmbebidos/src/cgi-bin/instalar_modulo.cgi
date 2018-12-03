#!/bin/bash
echo "Content-type: text/html"
echo
echo "<html>"
echo "<head>"
echo "<title>"
echo "Instala Modulo"
echo "</title>"
echo "</head>"
echo "<body bgcolor='00FFAF'>"
echo "<header>"
echo "<hgroup>"
echo "<h1 style='text-align:center;'><u><b>SISTEMAS OPERATIVOS II</b></u></h1>"
echo "<h2 style='text-align:center;'><b>Instalacion del Modulo</b></h2>"
echo "</hgroup>"
echo "</header>"

/sbin/insmod /var/www/upload/hello.ko
echo "<h3><font color='red'><i>"
dmesg -k | tail -n 1

echo "</h3></font></i>"
echo "<h4>Presione aqui si quiere desinstalar el modulo recientemente instalado:</h4>"
echo "<p><a href='desinstalar_modulo.cgi'><input type='button' value='Desinstalar Modulo'></a></p>"
echo "</body>"
echo "</html>"



