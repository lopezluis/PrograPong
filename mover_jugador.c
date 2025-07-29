#include <ncurses.h>          // por refresh(comentado), mvaddch
#include "juego.h"            // por Juego
#include "reaccion_maquina.h" // por maquina_con_limite_reaccion

void mostrar_barra_jugador(Juego *juego)
{
	// El campo de juego con coordenada y para mvaddch, es desde la fila 2 hasta juego->altoTablero + 2, el alto de la pantalla - 3, es relativo a la política de la barra de estado
	// Recordar que las coordenadas para mvaddch, están invertidas en relación a las funciones para modo gráfico habituales, es decir, primer parámetro y, segundo parámetro x
	for(unsigned char j = 0; j < juego->jugador.largo_actual; j++)
	{
		for(unsigned char i = 0; i < BARRA_X; i++)
		{
			mvaddch(juego->jugador.y + j + juego->inicio_tablero_y, i + 1 + juego->inicio_tablero_x, BARRA_CHAR);
		}
	}
	// Borrar el excedente superior de la barra del jugador, si corresponde.
	// Recordar que la coordenada y de la barra del jugador está en relación al tablero, que se despliaga desde 0 hasta juego->altoTablero
	// Además recordar que la coordenada x de la barra del jugador es fija en 1 por el espacio 0 detrás de la barra, con relación ai inicio del tablero juego->inicio_tablero_x
	if(juego->jugador.y > 0)
	{
		for(unsigned char i = 0; i < BARRA_X; i++)
		{
			mvaddch(juego->jugador.y - 1 + juego->inicio_tablero_y, i + 1 + juego->inicio_tablero_x, VACIO_CHAR);
		}
	}
	// Borrar el excedente inferior de la barra del jugador, si corresponde
	if(juego->jugador.y < (juego->altoTablero - juego->jugador.largo_actual))
	{
		for(unsigned char i = 0; i < BARRA_X; i++)
		{
			mvaddch(juego->jugador.y + juego->jugador.largo_actual + juego->inicio_tablero_y, i + 1 + juego->inicio_tablero_x, VACIO_CHAR);
		}
	}
	// Actualizar la pantalla, no hace falta, se actualizará en juego_correr
	//refresh();
}

void mostrar_barra_maquina(Juego *juego)
{
	// Idem mostrar_barra_jugador
	for(unsigned char j = 0; j < juego->maquina.largo_actual; j++)
	{
		for(unsigned char i = 0; i < BARRA_X; i++)
		{
			mvaddch(juego->maquina.y + j + juego->inicio_tablero_y, juego->anchoTablero - 1 - BARRA_X + i + juego->inicio_tablero_x, BARRA_CHAR);
		}
	}
	// Borrar el excedente superior de la barra de la máquina
	if(juego->maquina.y > 0)
	{
		for(unsigned char i = 0; i < BARRA_X; i++)
		{
			mvaddch(juego->maquina.y - 1 + juego->inicio_tablero_y, juego->anchoTablero - 1 - BARRA_X + i + juego->inicio_tablero_x, VACIO_CHAR);
		}
	}
	// Borrar el excedente inferior de la barra de la maquina
	if(juego->maquina.y < (juego->altoTablero - juego->maquina.largo_actual))
	{
		for(unsigned char i = 0; i < BARRA_X; i++)
		{
			mvaddch(juego->maquina.y + juego->maquina.largo_actual + juego->inicio_tablero_y, juego->anchoTablero - 1 - BARRA_X + i + juego->inicio_tablero_x, VACIO_CHAR);
		}
	}
	// Actualizar la pantalla, no hace falta, se actualizará en juego_correr
	//refresh();
}

void mover_jugador_arriba(Juego *juego)
{
	if(juego->jugador.y > 0)
	{
		juego->jugador.y--;
	}
	mostrar_barra_jugador(juego);
}

void mover_jugador_abajo(Juego *juego)
{
	if(juego->jugador.y < (juego->altoTablero - juego->jugador.largo_actual))
	{
		juego->jugador.y++;
	}
	mostrar_barra_jugador(juego);
}

void mover_maquina(Juego *juego)
{
	if(maquina_con_limite_reaccion(juego))
    {
        return;
    }
    unsigned short medioMaquina = juego->maquina.y + (juego->maquina.largo_actual / 2);
	if((medioMaquina > juego->bola.y) && (juego->maquina.y > 0))
	{
		if(juego->direccion_barra_maquina != HACIA_ARRIBA)
        {
            juego->direccion_barra_maquina = HACIA_ARRIBA;
            limitar_reaccion_maquina(juego);
        }
        else
		{
            juego->maquina.y--;
		}
	}
	else
	{
		if((medioMaquina < juego->bola.y) && (juego->maquina.y < (juego->altoTablero - juego->maquina.largo_actual)))
		{
            if(juego->direccion_barra_maquina != HACIA_ABAJO)
            {
                juego->direccion_barra_maquina = HACIA_ABAJO;
                limitar_reaccion_maquina(juego);
            }
            else
            {
                juego->maquina.y++;
            }
		}
	}
	mostrar_barra_maquina(juego);
}

void borrar_toda_barra_jugador(Juego *juego)
{
	// El campo de juego es desde fila 2 hasta juego->altoTablero + 2 o el alto de la pantalla - 3
	// Mover el cursor en el lugar de la barra del jugador y vaciar
	for(unsigned char j = 0; j < juego->altoTablero; j++)
	{
		for(unsigned char i = 0; i < BARRA_X; i++)
		{
			mvaddch(j + juego->inicio_tablero_y, i + 1 + juego->inicio_tablero_x, VACIO_CHAR);
		}
	}
}

void borrar_toda_barra_maquina(Juego *juego)
{
	// Idem borrar_toda_barra_jugador
	for(unsigned char j = 0; j < juego->altoTablero; j++)
	{
		for(unsigned char i = 0; i < BARRA_X; i++)
		{
			mvaddch(j + juego->inicio_tablero_y, juego->anchoTablero - 1 - BARRA_X + i + juego->inicio_tablero_x, VACIO_CHAR);
		}
	}
}
