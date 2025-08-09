#include <curses.h>  // por clear(comentado), mvaddstr(comentado), refresh(comentado), getch(comentado), endwin
#include <stdio.h>   // por puts
#include <stdlib.h>  // por exit, EXIT_FAILURE

void mostrar_error(const char *mensaje)
{
    // Limpia la pantalla
    //clear();
    // Mover el cursor a las coordenadas superior derecha y mostrar el mensaje
    //mvaddstr(0, 0, mensaje);
    // Actualizar la pantalla
    //refresh();
    // Esperar una tecla
    //while(getch() == ERR);
    // Finalizar ncurses
    endwin();
    // Mostrar el mensaje
    puts(mensaje);
    // Salir al sistema operativo
    exit(EXIT_FAILURE);
    // TODO Ver si se puede crear una ventana similar al intento en el siguiente código
    // Crear ventana
//    WINDOW *win = newwin(10, 10, 10, 200);
    // Dibujar un borde
//    wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
    // Visualizar el mensaje
//    mvwprintw(win, 1, 1, "%s", mensaje);
    // Actualizar la ventana
//    wrefresh(win);
    // Esperar una tecla
//    while(getch() == ERR);
    // Borrar la ventana
//    delwin(win);
}
