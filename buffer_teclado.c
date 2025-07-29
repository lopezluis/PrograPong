#include <time.h>    // por clock_t, clock, CLOCKS_PER_SEC
#include <ncurses.h> // por getch

void vaciar_buffer_teclado(void)
{
    clock_t fin_vaciado = clock() + (5 * CLOCKS_PER_SEC);
    // Vaciar el buffer de caracteres, nótese que aquí hay necesidad de hacer una evaluación completa de la condición
    while((clock() < fin_vaciado) & getch());
}
