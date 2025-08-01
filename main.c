#include <stdio.h>          // por stderr, fputs, perror, EOF (End-Of-File)
#include <stdlib.h>         // por EXIT_SUCCESS, EXIT_FAILURE
#include <stddef.h>         // por NULL
#include <ncurses.h>        // por initscr, noecho, keypad, stdscr, endwin
#include <string.h>         // por strcpy
#include "mostrar_menu.h"   // por mostrar_menu
#include "tabla_puntajes.h" // por iniciar_tabla_mejores_puntajes, mostrar_tabla_puntajes, leer_tabla_mejores_puntajes, finalizar_tabla_mejores_puntajes
#include "juego_ini.h"      // por juego_ini
#include "juego_correr.h"   // por juego_correr
#include "juego.h"          // por Juego
#include "mostrar_error.h"  // por mostrar_error

int main(int argc, char *argv[])
{
	// Inicializar ncurses
    if (initscr() == NULL)
    {
        if (fputs("Error: Al inicializar ncurses.\n\r", stderr) == EOF)
        {
            perror("Error interno");
        }
        return EXIT_FAILURE;
    }
	// Oculta el cursor
	curs_set(0);
	// Desactiva el modo de línea (line buffering) y hace que cada carácter ingresado por el usuario sea inmediatamente disponible para la aplicación
	cbreak();
	// Desactivar el eco del teclado
	noecho();
	// Evita que el cursor se quede en la última línea
	scrollok(stdscr, TRUE);
	// Convierte a getch en una función no bloqueante
	nodelay(stdscr, TRUE);
	// Habilitar teclas especiales (como flechas)
	keypad(stdscr, TRUE);
	// Inicializar la semilla de obtención de números aleatorios
	srand(time(NULL));
	// Como este desarrollo no utiliza parámetros, validamos que no se pasen. En el caso de encontrar parámetros mostraré el error
	if (argc > 1)
	{
        char *strAux0 = (char *)malloc(1024), *strAux1 = (char *)malloc(1024);
        // Verifir que malloc tuvo éxito
        if((strAux0 == NULL) || (strAux1 == NULL))
        {
			// Muestra el error y finaliza la ejecución.
            mostrar_error("Error: No hay suficiente memoria para los textos auxiliares que muestran errores.\n\r");
        }
        strcpy(strAux0, "Error: No se permiten parámetros por línea de comandos.\n\r");
		int i = 1;
		while (i < argc)
		{
			if (sprintf(strAux1, "%s¿%s?\n\r", strAux0, argv[i]) == EOF)
			{
				perror("Error interno");
			}
            strcpy(strAux0, strAux1);
			i++;
		}
        // Destruir memoria utilizada por variable strAux0
        free(strAux0);
        // En este caso no haría falta anular el puntero, lo hago solo por buena práctica
        strAux0 = NULL;
        // Mostrar error y salir
        mostrar_error(strAux1);
        // A este punto la ejecución no llegará, pero lo dejo explícito igualmente
        free(strAux1);
        strAux1 = NULL;
	}
	// Inicialización del juego
	Juego *juego = juego_ini();
	// Inicializar tabla de mejores puntajes, recordar que es un miembro de la estructura del juego
	iniciar_tabla_mejores_puntajes(juego);
	// Como la tabla de mejores puntajes está vacía, la leo desde disco, si existe
	leer_tabla_mejores_puntajes(juego);
	unsigned char noSalir = 1;
	while(noSalir)
	{
		switch(mostrar_menu(juego))
		{
		case 0: // Comenzar partida
			juego_correr(juego);
            // Destruir el juego e inicializarlo nuevamente para nuevos valores
            juego_fin(&juego);
            juego = juego_ini();
			break;
		case 1: // Ver tabla de puntajes
			mostrar_tabla_puntajes(juego);
			break;
		case 2: // Salir
			noSalir = 0;
			break;
		}
	}
	finalizar_tabla_mejores_puntajes(juego);
    juego_fin(&juego);
	// Finalizar ncurses
	endwin();
	return EXIT_SUCCESS;
}
