#include <stdio.h>          // por stderr, fputs, perror, EOF (End-Of-File)
#include <stdlib.h>         // por system, EXIT_SUCCESS, EXIT_FAILURE
#include <stddef.h>         // por NULL
#include <curses.h>         // por initscr, noecho, keypad, stdscr, endwin
#include <string.h>         // por strcpy
#include "mostrar_menu.h"   // por mostrar_menu
#include "tabla_puntajes.h" // por mostrar_tabla_puntajes
#include "juego_ini.h"      // por juego_ini, juego_fin
#include "juego_correr.h"   // por juego_correr
#include "juego.h"          // por Juego
#include "mostrar_error.h"  // por mostrar_error
#include "bola.h"           // por inicializar_delta_angulo

int main(int argc, char *argv[])
{
    // Tener en cuenta que para Linux no hace falta establecer el c�digo de p�gina, el siguiente seteo es para windows
    system("chcp 850 > NUL");
	// Inicializar ncurses/pdcurses, el comportamiento de la librer�a tanto en Linux como en Windows es muy bueno,
	// se emplea para logtrar mayor fluidez en el juego
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
	// Desactiva el modo de l�nea (line buffering) y hace que cada car�cter ingresado por el usuario sea inmediatamente disponible para la aplicaci�n
	cbreak();
	// Desactivar el eco del teclado
	noecho();
	// Evita que el cursor se quede en la �ltima l�nea
	scrollok(stdscr, TRUE);
	// Convierte a getch en una funci�n no bloqueante
	nodelay(stdscr, TRUE);
	// Habilitar teclas especiales (como flechas)
	keypad(stdscr, TRUE);
	// Inicializar la semilla de obtenci�n de n�meros aleatorios
	srand(time(NULL));
	// Como este desarrollo no utiliza par�metros, validamos que no se pasen. En el caso de encontrar par�metros mostrar� el error
	if (argc > 1)
	{
        char *strAux0 = (char *)malloc(1024), *strAux1 = (char *)malloc(1024);
        // Verifir que malloc tuvo �xito
        if((strAux0 == NULL) || (strAux1 == NULL))
        {
			// Muestra el error y finaliza la ejecuci�n.
            mostrar_error("Error: No hay suficiente memoria para los textos auxiliares que muestran errores.\n\r");
        }
        strcpy(strAux0, "Error: No se permiten par\xA0metros por l\xA1nea de comandos.\n\r");
		int i = 1;
		while (i < argc)
		{
			if (sprintf(strAux1, "%s \xA8%s?\n\r", strAux0, argv[i]) == EOF)
			{
				perror("Error interno");
			}
            strcpy(strAux0, strAux1);
			i++;
		}
        // Destruir memoria utilizada por variable strAux0
        free(strAux0);
        // En este caso no har�a falta anular el puntero, lo hago solo por buena pr�ctica
        strAux0 = NULL;
        // Mostrar error y salir
        mostrar_error(strAux1);
        // A este punto la ejecuci�n no llegar�, pero lo dejo expl�cito igualmente
        free(strAux1);
        strAux1 = NULL;
	}
	// Inicializar el c�lculo de los deltas de los �ngulos, simplemente construyo el vector una sola vez al iniciar el programa, lo ideal deber�a ser meter esto en un include, como una constante
	inicializar_delta_angulo();
	// Inicializaci�n del juego
	Juego *juego = juego_ini();
	unsigned char noSalir = 1;
	while(noSalir)
	{
		switch(mostrar_menu(juego))
		{
		case 0: // Comenzar partida
			juego_correr(juego); // corre el juego por lapso de una partida completa
            // Destruir el juego e inicializarlo nuevamente para nuevos valores
            juego_fin(&juego);
            // Reiniciar una nueva partida
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
    juego_fin(&juego);
	// Finalizar ncurses
	endwin();
	return EXIT_SUCCESS;
}
