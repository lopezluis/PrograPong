#include <time.h>   // por clock_t, clock, CLOCKS_PER_SEC
#include <curses.h> // por getch, mvaddstr, refresh
#include "juego.h"  // por Juego

void vaciar_buffer_teclado(Juego *juego)
{
    // Hasta 80 caracteres es posible escribir, es decir el 79 si, pero el 80 no. El siguiente string es lo más largo posible:
    mvaddstr(juego->altoTablero + 3, 40, "Vaciando buffer de teclado. No teclee...");
    refresh();
    // 5 segundos para que el jugador reaccione a que debe soltar las teclas
    //clock_t fin_vaciado = clock() + (5 * CLOCKS_PER_SEC);
    // bajamos a 1 segundo para que ej juego sea más dinámico
    clock_t fin_vaciado = clock() + (1 * CLOCKS_PER_SEC);
    // Vaciar el buffer de caracteres, nótese que aquí hay necesidad de hacer una evaluación completa de la condición
    while((clock() < fin_vaciado) & getch());
    mvaddstr(juego->altoTablero + 3, 40, "                                        ");
    refresh();
}
