//#include <ncurses.h>       // por clear, mvprintw, refresh, getch
#include <sys/ioctl.h>     // por ioctl, struct winsize, TIOCGWINSZ
#include "juego.h"         // por TABLERO_X_MINIMO, TABLERO_Y_MINIMO
#include "mostrar_error.h" // por mostrar_error

void tamanio_terminal(unsigned short *anchoPantalla, unsigned short *altoPantalla)
{
	struct winsize sz;
	if(ioctl(0, TIOCGWINSZ, &sz) == 0)
	{
		// Limpia la pantalla
//		clear();
		// Mover el cursor a las coordenadas superior derecha y mostrar coordenadas obtenidas
//		mvprintw(0, 0, "Coordenadas de la terminal: x=%d y=%d ", sz.ws_col, sz.ws_row);
		// Actualizar la pantalla
//		refresh();
//		while(getch() == ERR);
		if((sz.ws_col < TABLERO_X_MINIMO) || (sz.ws_row < TABLERO_Y_MINIMO))
		{
            mostrar_error("Error: La pantalla es demasiado pequeña.\n\rPresione una tecla y acondicione la pantalla.\n\r");
		}
		*anchoPantalla = sz.ws_col;
		*altoPantalla = sz.ws_row;
	}
	else
	{
        mostrar_error("Error: No se pueden obtener las características de la consola a utilizar.\n\r");
	}
}
