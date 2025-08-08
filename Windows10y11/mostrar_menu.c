#include <curses.h> // por clear, mvaddstr, refresh
#include <stdlib.h> // por exit, EXIT_SUCCESS, EXIT_FAILURE
#include "juego.h"  // por Juego

int mostrar_menu(Juego *juego)
{
	// Limpia la pantalla
	clear();
	// Mover el cursor a coordenadas acordes y mostrar las cadenas
	mvaddstr(0, (juego->anchoTablero - PROGRAMA_LARGO) / 2, PROGRAMA_TITULO);
	mvaddstr(1, (juego->anchoTablero - DESARROLLADOR_LARGO) / 2, DESARROLLADOR_TITULO);
	mvaddstr(3, 0, "Men\xA3 principal:");
	mvaddstr(4, 1, "1 Cambiar modo: ");
	mvaddstr(5, 1, "2 Comenzar partida");
	mvaddstr(6, 1, "3 Ver tabla de puntajes");
	mvaddstr(7, 1, "4 Salir");
	if(juego->modo == AVENTURA)
	{
		mvaddstr(4, 17, "AVENTURA     ");
	}
	else
	{
		if(juego->modo == SUPERVIVENCIA)
		{
			mvaddstr(4, 17, "SUPERVIVENCIA");
		}
		else
		{
			if (fputs("Error: Inconsistencia, el modo solo puede ser Aventura o Supervivencia.\n", stderr) == EOF)
			{
				perror("Error interno");
			}
			exit(EXIT_FAILURE);
		}
	}
	// Actualizar la pantalla
	refresh();
	// El siguiente sencillo bucle, genera uso intensivo del procesador, ¿se podrá lograr menor uso del procesador, sin por ello peder eficiencia?
	while (1)
	{
		switch(getch())
		{
		case '1':
			if(juego->modo == AVENTURA)
			{
				mvaddstr(4, 17, "SUPERVIVENCIA");
				juego->modo = SUPERVIVENCIA;
                juego->vidas = 10;
				refresh();
			}
			else
			{
				if(juego->modo == SUPERVIVENCIA)
				{
					mvaddstr(4, 17, "AVENTURA     ");
					juego->modo = AVENTURA;
                    juego->vidas = 5;
					refresh();
				}
				else
				{
					if (fputs("Error: Inconsistencia, el modo solo puede ser Aventura o Supervivencia.\n", stderr) == EOF)
					{
						perror("Error interno");
					}
					exit(EXIT_FAILURE);
				}
			}
			break;
		case '2':
			return 0;
		case '3':
			return 1;
		case '4':
			return 2;
		}
	}
}
