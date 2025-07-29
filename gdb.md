# GDB

## Introducción

GDB, el depurador de GNU, es una herramienta esencial para depurar programas en lenguajes como C, C++ y otros. A continuación comandos útiles para empezar:

### Comandos de control de ejecución

	* r o run: Inicia la ejecución del programa.
	* n o next: Ejecuta la siguiente línea de código, sin entrar en funciones llamadas.
	* s o step: Ejecuta la siguiente línea de código, entrando en funciones llamadas.
	* c o continue: Continúa la ejecución hasta el siguiente punto de interrupción o hasta que el programa termine.
	* finish: Ejecuta hasta el final de la función actual.
	* until: Ejecuta hasta la línea actual, o hasta que se alcance una línea especificada.
	* break <line_number> o b <line_number>: Establece un punto de interrupción en la línea especificada.
	* break <function_name> o b <function_name>: Establece un punto de interrupción en la función especificada.
	* info breakpoints: Muestra todos los puntos de interrupción establecidos.
	* delete <breakpoint_number>: Elimina el punto de interrupción con el número especificado.
	* disable <breakpoint_number>: Desactiva el punto de interrupción con el número especificado.
	* enable <breakpoint_number>: Activa el punto de interrupción con el número especificado.
	* quit o q: Sale de GDB.

### Comandos de inspección de variables y memoria

	* print <variable_name>: Imprime el valor de la variable especificada.
	* x/<n/f/u> <address>: Examina la memoria en la dirección especificada.
	* n: número de unidades a mostrar (por defecto, 1).
	* f: formato de salida (por ejemplo, x para hexadecimal, d para decimal, f para flotante, c para carácter).
	* u: unidad de tamaño (por ejemplo, b para bytes, h para medias palabras, w para palabras).
	* info locals: Muestra todas las variables locales de la función actual.
	* info args: Muestra los argumentos de la función actual.
	* whatis <variable_name>: Muestra el tipo de la variable especificada.
	* p *(<type>*)<address>: Imprime el valor de una estructura en una dirección dada, especificando el tipo de la estructura.
	* display <variable_name>: Muestra el valor de la variable cada vez que se detiene el programa.
	* undisplay <display_number>: Elimina una visualización con el número especificado.

### Comandos de información y ayuda

	* help: Muestra la ayuda general.
	* help <command>: Muestra la ayuda específica para el comando especificado.
	* backtrace o bt: Muestra el seguimiento de la pila (la secuencia de llamadas a funciones).
	* frame <frame_number>: Cambia el contexto de ejecución al marco especificado en el seguimiento de la pila.
	* info sources: Muestra los archivos fuente que GDB está utilizando.
	* info functions: Muestra la lista de funciones.
	* info variables: Muestra la lista de variables.

### Comandos adicionales

	* source <file_name>: Carga comandos desde el archivo especificado.
	* save breakpoints <file_name>: Guarda los puntos de interrupción en el archivo especificado.
	* set logging on: Inicia el registro de la sesión de GDB en un archivo.
	* set logging off: Detiene el registro de la sesión de GDB.

## Para prograPongPAyED

Ejecuto:

	gdb prograPongPAyED

Mostrar la versión utilizada del gdb:

	show version

Me informa:

	GNU gdb (Debian 13.1-3) 13.1

Para iniciar la ejecución del programa y detenerse en la primera instrucción de la función principal (main):

	start

Me informa:

	Temporary breakpoint 1 at 0x142c: file main.c, line 14.
	Starting program: unlp/PAyED2025/trabajosEspeciales/2025semestre1/prograPongPAyED
	Temporary breakpoint 1, main (argc=1, argv=0x7fffffffdd08) at main.c:14
	14              if (argc > 1)

Ejecutar siguiente instrucción:

	n

Mostrar valores de variables dentro de una estructura, p es abreviatura del comando print, solo se pueden ver valores de una sola variable, no de varias:

	p sz.ws_col
	p sz.ws_row

Ejecutar hasta determinada línea, debo establecer un punto de interrupción con **break** o su abreviatura **b**, la línea debe corresponder a la fuente ejecutandose actualmente y naturalmente debe pasar la ejecución por la línea que se desea detener la ejecución:

	b 39

En el caso de querer especificar un archivo fuente determinado que forma parte de la solución:

    b reaccion_maquina.c:13

Luego ejecuto el comando **continue** o su abreviatura **c**:

	c

Muestro los breakpoints establecidos:

	info breakpoints

Elimino alguno, con el comando **delete** que creo no tiene abreviatura, es necesario indicarle el breakpoint que se desea eliminar, dato que obtengo de **info breakpoints**:

	delete 2

## Comparar fuentes

Para comparar dos archivos fuentes en lenguaje c:

	/usr/bin/diff --report-identical-files --side-by-side --tabsize=4 --expand-tabs --ignore-trailing-space --ignore-space-change --text /home/invitado/Projects/simon/unlp/PAyED2025/trabajosEspeciales/2025semestre1/mover_jugador.c /home/invitado/Projects/simon/unlp/PAyED2025/trabajosEspeciales/2025semestre1windows/mover_jugador.c
