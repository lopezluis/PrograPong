#include <stdlib.h>        // por rand
#include <curses.h>        // por mvaddch, mvaddstr
#include <time.h>          // por clock, CLOCKS_PER_SEC
#include "juego.h"         // por Juego
#include "mover_jugador.h" // por mostrar_barra_jugador, mostrar_barra_maquina, borrar_toda_barra_jugador, borrar_toda_barra_maquina

const char *linea_item_especial[] = {"ll", "rr", "bb"};

void inicializar_vector_item(Juego *juego)
{
    unsigned char i = 0;
    while (i < ITEMS_NUM)
    {
        juego->item[i].tipo = 0;
        i++;
    }
}

// La función bola_encima_item se usa para verificar que la bola no caiga sobre un ítem al inicializarla
int bola_encima_item(Juego *juego, int identificador_item)
{
    // ¿El ítem está inactivo?
    if((juego->item[identificador_item].tipo & 0x80) == 0)
    {
        return 0;
    }
    return(
        (juego->bola.x >= juego->item[identificador_item].x) &&
        (juego->bola.x <= (juego->item[identificador_item].x + 1)) &&
        (juego->bola.y >= juego->item[identificador_item].y) &&
        (juego->bola.y <= (juego->item[identificador_item].y + 1))
    );
}

// La función item_encima_otro es privada, no debe agregarse al encabezado de esta fuente
unsigned char item_encima_otro(Juego *juego, unsigned char item)
{
    unsigned char i = 0;
    while (i < ITEMS_NUM)
    {
        if(i != item)
        {
            // Si por azar juego->item[i].x
            if((((juego->item[item].x - 1) == juego->item[i].x) || (juego->item[item].x == juego->item[i].x) || ((juego->item[item].x + 1) == juego->item[i].x)) &&
               (((juego->item[item].y - 1) == juego->item[i].y) || (juego->item[item].y == juego->item[i].y) || ((juego->item[item].y + 1) == juego->item[i].y)))
            {
                return 1;
            }
        }
        i++;
    }
    return 0;
}

void item_procesar(Juego *juego)
{
    unsigned char i = 0;
    while (i < ITEMS_NUM)
    {
        // ¿El ítem está activo?
        if((juego->item[i].tipo & 0x80) != 0)
        {
            // El item está activo, verificar si la bola toca el item
            if(bola_encima_item(juego, i))
            {
                // Desactivar el item
                juego->item[i].tipo &= 3;
                // Hacer desaparecer el ítem, borraré la bola, pero no importa, reaparecerá en el próximo refresh
                mvaddch(juego->item[i].y + juego->inicio_tablero_y, juego->item[i].x + juego->inicio_tablero_x, VACIO_CHAR);
                mvaddch(juego->item[i].y + juego->inicio_tablero_y + 1, juego->item[i].x + juego->inicio_tablero_x, VACIO_CHAR);
                mvaddch(juego->item[i].y + juego->inicio_tablero_y, juego->item[i].x + juego->inicio_tablero_x + 1, VACIO_CHAR);
                mvaddch(juego->item[i].y + juego->inicio_tablero_y + 1, juego->item[i].x + juego->inicio_tablero_x + 1, VACIO_CHAR);
                // Aplico el efecto a quien corresponda,
                // no me parece del todo bien aplicar el ítem por cuadrante, porque puede que el efrecto caduque antes que la bola impacte a quien debe aplicarsele el efrecto
                // mejor sería aplicar el efecto a quien será el próximo en pegarle a la bola
                // Si a quien corresponde aplicar ya tiene otro efecto activo, se le aplica el efecto nuevo, dejando sin efecto el anterior
                if(juego->bola.x < (juego->anchoTablero / 2))
                {
                    // El efecto corresponde al jugador, tener en cuenta que ya desactivé el item
                    switch(juego->item[i].tipo)
                    {
                    case 0: // alargar-velocidad
                        // Cuando la bola golpea el item de alargar barra, puede ser que antes venga de un bloquear, entonces es necesario eliminar la barra completa
                        borrar_toda_barra_jugador(juego);
                        juego->jugador.largo_actual = BARRA_Y_LARGO;
                        // Si el jugador está cerca del muro inferior, el alargasmiento de la barra rompe el muro, evitar esto
                        if(juego->jugador.y > 0)
                        {
                            juego->jugador.y--;
                        }
                        if((juego->jugador.y + juego->jugador.largo_actual) >= juego->altoTablero)
                        {
                            juego->jugador.y = juego->altoTablero - juego->jugador.largo_actual - 1;
                        }
                        juego->jugador.ticks_mover *= 3 / 5;
                        juego->jugador.ticks_efecto = clock() + (10 * CLOCKS_PER_SEC);
                        mostrar_barra_jugador(juego);
                        break;
                    case 1: // acortar+velocidad
                        // Cuando la bola golpea el item de alargar barra, puede ser que antes venga de un bloquear, entonces es necesario eliminar la barra completa
                        borrar_toda_barra_jugador(juego);
                        juego->jugador.largo_actual = BARRA_Y_CORTO;
                        juego->jugador.y++;
                        juego->jugador.ticks_mover *= 5 / 4;
                        juego->jugador.ticks_efecto = clock() + (10 * CLOCKS_PER_SEC);
                        mostrar_barra_jugador(juego);
                        break;
                    case 2: // bloquear zona
                        // Este item no tiene sentidso si la pelota se aleja del jugador
                        juego->jugador.largo_actual = juego->altoTablero;
                        juego->jugador.y = 0;
                        juego->jugador.ticks_efecto = clock() + (5 * CLOCKS_PER_SEC);
                        mostrar_barra_jugador(juego);
                        break;
                    }
                }
                else
                {
                    // El efecto corresponde a la maquina
                    switch(juego->item[i].tipo)
                    {
                    case 0: // alargar-velocidad
                        // Cuando la bola golpea el item de alargar barra, puede ser que antes venga de un bloquear, entonces es necesario eliminar la barra completa
                        borrar_toda_barra_maquina(juego);
                        juego->maquina.largo_actual = BARRA_Y_LARGO;
                        // Si la máquina está pegada a un muro, el alargasmiento de la barra rompe el muro, evitar esto
                        if(juego->maquina.y > 0)
                        {
                            juego->maquina.y--;
                        }
                        if((juego->maquina.y + juego->maquina.largo_actual) >= juego->altoTablero)
                        {
                            juego->maquina.y = juego->altoTablero - juego->maquina.largo_actual - 1;
                        }
                        juego->maquina.ticks_mover *= 3 / 5;
                        juego->maquina.ticks_efecto = clock() + (10 * CLOCKS_PER_SEC);
                        mostrar_barra_maquina(juego);
                        break;
                    case 1: // acortar+velocidad
                        // Cuando la bola golpea el item de alargar barra, puede ser que antes venga de un bloquear, entonces es necesario eliminar la barra completa
                        borrar_toda_barra_maquina(juego);
                        juego->maquina.largo_actual = BARRA_Y_CORTO;
                        juego->maquina.y++;
                        juego->maquina.ticks_mover *= 5 / 4;
                        juego->maquina.ticks_efecto = clock() + (10 * CLOCKS_PER_SEC);
                        mostrar_barra_maquina(juego);
                        break;
                    case 2: // bloquear zona
                        // Este item no tiene sentidso si la pelota se aleja de la maquina
                        juego->maquina.largo_actual = juego->altoTablero;
                        juego->maquina.y = 0;
                        juego->maquina.ticks_efecto = clock() + (5 * CLOCKS_PER_SEC);
                        mostrar_barra_maquina(juego);
                        break;
                    }
                }
            }
            else
            {
                if(clock() > juego->item[i].instante_desaparicion)
                {
                    // Desactivar el item
                    juego->item[i].tipo &= 3;
                    // El ítem desaparece, borreré loa bola, pero no importa, reaparecerá en el próximo refresh
                    mvaddch(juego->item[i].y + juego->inicio_tablero_y, juego->item[i].x + juego->inicio_tablero_x, VACIO_CHAR);
                    mvaddch(juego->item[i].y + juego->inicio_tablero_y + 1, juego->item[i].x + juego->inicio_tablero_x, VACIO_CHAR);
                    mvaddch(juego->item[i].y + juego->inicio_tablero_y, juego->item[i].x + juego->inicio_tablero_x + 1, VACIO_CHAR);
                    mvaddch(juego->item[i].y + juego->inicio_tablero_y + 1, juego->item[i].x + juego->inicio_tablero_x + 1, VACIO_CHAR);
                }
            }
            // TODO Si el item está activo, la consigna dice "flotan en el campo"; en principio, para avanzar el la implementación de funciones más importantes, lo dejo estático
            // aplicar un cambio de (x+-1, y+-1) aleatorio, queda desagradable, lo ideal, sería aplicar una función que determine su ubicación dado el tiemplo, y aplicar esa definición
        }
        else
        {
            // El ítem está inactivo, lo activo al azar
            // Los items aparecen con frecuencia elevada, la alta frecuencia hace que el juego sea más divertido
            if((rand() % 100) < 1)
            {
                // Permito a los items aparecer al menos dejando 3 columnas de espaciado desde las barras
                juego->item[i].x = (rand() % (juego->anchoTablero - (BARRA_X * 2) - 9)) + BARRA_X + 4;
                // Resto 2, uno porque el item ocupa 2 filas y otro porque juego->altoTablero es cantidad de filas
                juego->item[i].y = rand() % (juego->altoTablero - 2);
                // El item nuevo no debe aparecer encima de otro anteriormente presente, el do while controla esto
                if(item_encima_otro(juego, i))
                {
                    juego->item[i].x += 2;
                    juego->item[i].y += 2;
                    // Si culpa del incremento, sacamos el item del tableo, lo volvemos a meter
                    if(juego->item[i].x > (juego->anchoTablero - (BARRA_X + 5)))
                    {
                        juego->item[i].x = BARRA_X + 4;
                    }
                    // Recordar que juego->altoTablero ya excluye titulo, 2 de estrado y 2 muros, entonces 2 porque el item ocupa 2 filas y otro porque juego->altoTablero es cantidad de filas
                    if(juego->item[i].y > (juego->altoTablero - 2))
                    {
                        juego->item[i].y = 0;
                    }
                }
                juego->item[i].tipo = rand() % 3 + 128; // 0: alargar-velocidad, 1: acortar+velocidad, 2: bloquear zona
                // La consigna dice que deben "desaparecer a los 5-10 segundos si no son golpeados", considero que es poco tiempo
                juego->item[i].instante_desaparicion = clock() + (rand() % (6 * CLOCKS_PER_SEC)) + (5 * CLOCKS_PER_SEC);
                // El item no debe aparecer encima de la bola, si el azar así lo provoca, quitar la situación
                if(bola_encima_item(juego, i))
                {
                    juego->item[i].x += 2 - juego->bola.x + juego->item[i].x;
                    juego->item[i].y += 2 - juego->bola.y + juego->item[i].y;
                    // Si culpa del incremento, sacamos el item del tableo, lo volvemos a meter
                    if(juego->item[i].x >= juego->anchoTablero)
                    {
                        juego->item[i].x = 0;
                    }
                    if(juego->item[i].y >= juego->altoTablero)
                    {
                        juego->item[i].y = 0;
                    }
                }
                // dibujar el ítem en pantalla
                mvaddstr(juego->item[i].y + juego->inicio_tablero_y, juego->item[i].x + juego->inicio_tablero_x, linea_item_especial[juego->item[i].tipo & 3]);
                mvaddstr(juego->item[i].y + juego->inicio_tablero_y + 1, juego->item[i].x + juego->inicio_tablero_x, linea_item_especial[juego->item[i].tipo & 3]);
            }
        }
        i++;
    }
}
