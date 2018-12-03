#!/usr/bin/perl -w

use strict;
use CGI::Carp qw(fatalsToBrowser);
use CGI;

my %Input;

my $query = new CGI;
my @pairs = $query->param;

foreach my $pair(@pairs){
	$Input{$pair} = $query->param($pair);
}

#Directorio donde queremos guardar los archivos
my $dir = "/var/www/upload/";

#Array con extensiones de archivos que podemos recibir
my @extensiones = ('ko');
print "Context-type:text/html\n\n";
print "<title>Upload</title>";
print "<h1 style='text-align:center;'><u><b>SISTEMAS OPERATIVOS II</b></u></h1>\n";

recepcion_de_archivo(); #Iniciar la recepcion del archivo
print "<body bgcolor='00C9FF'>";
print "<h2><font color='green'>El archivo fue recibido correctamente</font></h2><br>\n";
print "<h4>Presione para instalar el modulo subido</h4>\n";
print "<form action='/cgi-bin/instalar_modulo.cgi'>\n";
print "<input type='submit' name='Submit' value='Instalar' />";
print "</form>";
print "</body>";
#TODO SALIO BIEN

exit(1);

sub recepcion_de_archivo{

	my $nombre_en_servidor = $Input{'archivo'};
	$nombre_en_servidor =~ s/ /_/gi;
	$nombre_en_servidor =~ s!^.*(\\|\/)!!;

	my $extension_correcta = 0;

	foreach (@extensiones){
		if($nombre_en_servidor =~ /\.$_$/i){
			$extension_correcta = 1;
		last;
		}
	}

	if($extension_correcta){

		#Abrimos el nuevo archivo
		open (OUTFILE, ">$dir/$nombre_en_servidor") || die "No se puedo crear el archivo";
		binmode(OUTFILE); #Para no tener problemas en Windows

		#Transferimos byte por byte el archivo
		while (my $bytesread = read($Input{'archivo'}, my $buffer, 1024)) {
			print OUTFILE $buffer;
		}

		#Cerramos el archivo creado
		close (OUTFILE);

	}
	else{
		print "Content-type: text/html\n\n";
		print "<h1>Extension incorrecta</h1>";
		print "Solo se reciben archivo con extension: ";
		print join(",", @extensiones);
		exit(0);
	}

} #sub recepcion_de_archivo
