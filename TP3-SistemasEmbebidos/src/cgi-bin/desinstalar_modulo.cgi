#!/bin/bash
echo "Content-type: text/html"
echo
echo "<html>"
echo "<head>"
echo "<title>"
echo "Desinstala Modulo"
echo "</title>"
echo "</head>"
echo "<body bgcolor='F9F665'>"
echo "<header>"
echo "<hgroup>"
echo "<h1 style='text-align:center;'><u><b>SISTEMAS OPERATIVOS II</b></u></h1>"
echo "<h2 style='text-align:center;'><b>Desinstalacion del Modulo</b></h2>"
echo "</hgroup>"
echo "</header>"
/sbin/rmmod hello			#Desinstala el modulo "hello" si ya estaba instalado
echo "<h3><font color='red'><i>"
dmesg -k | tail -n 1			     #Muestra el ultimo mensaje del dmesg
echo "</H3></i></font>"
echo "</body>"
echo "</html>"
