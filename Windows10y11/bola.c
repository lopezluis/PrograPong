#include <stdlib.h>           // por rand
#include <curses.h>           // por mvaddch
#include <time.h>             // por CLOCKS_PER_SEC
#include <math.h>             // por M_PI, sin
#include "juego.h"            // por Juego
#include "reaccion_maquina.h" // por quitar_limite_reaccion_maquina
#include "buffer_teclado.h"   // por vaciar_buffer_teclado
#include "item.h"             // por bola_encima_item
#include "mover_jugador.h"    // por mostrar_barra_jugador, mostrar_barra_maquina, borrar_toda_barra_jugador, borrar_toda_barra_maquina

double delta_angulo[16384]; // 91 grados en el sistema sexagecimal 91*182.044444444444-1, desde 0 hasta 16383

// Las siguientes COORDENADA_X_PANTALLA_BOLA y COORDENADA_Y_PANTALLA_BOLA son privadas para esta fuente: "bola.c",
// por lo tanto no deben definirtse en el header de esta fuente, directamente en lugar de definirlas como funciones,
// las defino como macros para optimizar la ejecución del juego, dado que son muy sencillas,
// aunque el propio optimizador de gcc.exe lo haría por mi. Igualmente dejo comentadas las funciones para comparar

//unsigned short coordenada_x_pantalla_bola(Juego *juego)
//{
//    return juego->bola.x + juego->inicio_tablero_x;
//}

//unsigned short coordenada_y_pantalla_bola(Juego *juego)
//{
//    return juego->bola.y + juego->inicio_tablero_y;
//}

#define COORDENADA_X_PANTALLA_BOLA(juego) (juego->bola.x + juego->inicio_tablero_x)
#define COORDENADA_Y_PANTALLA_BOLA(juego) (juego->bola.y + juego->inicio_tablero_y)
// Tener en cuenta que cuando quiero conocer si la bola, por ejemplo, está dentro o fuera del tablero, debo comparar sus coordenadas con el ancho y alto del tablero, despreciando el inicio_tablero

void inicializar_delta_angulo(void)
{
    for(unsigned int i = 0; i < 16384 /* 90 grados en el sistema sexagecimal */; i++)
    {
        // Tener en cuenta que utilizo 180.0 en lugar de castear el 180 que es lo mismo ((double)180)
        delta_angulo[i] = sin((((double)i)/180.0)*(((double)M_PI)/180.0));
    }
}

void inicializar_bola(Juego *juego)
{
    // Borro la bola. Tener en cuenta que las coordenadas de la bola están desde (x, y) = (0, 0), por lo tanto dewberán sumársele el lugar donde inicia el tablero
    mvaddch(COORDENADA_Y_PANTALLA_BOLA(juego), COORDENADA_X_PANTALLA_BOLA(juego), VACIO_CHAR);
    // Inicializar la Bola
    juego->bola.x = juego->anchoTablero / 2;
    juego->bola.xFino = ((double)(juego->anchoTablero)) / 2;
    // La posición vertical de la bola, por definición es al azar, recordar que las coordenadas de la bola son con relación al tablero que va de 0 hasta juego->altoTablero, quitando barra de título, muros e información inferior
    {
        int bola_cae_sobre_item = 1;
        while(bola_cae_sobre_item)
        {
            juego->bola.y = juego->bola.yFino = rand() % juego->altoTablero;
            // Evitar que la Bola caiga sobre algún ítem
            {
                int i = 0;
                while(i < ITEMS_NUM)
                {
                    if(bola_encima_item(juego, i))
                    {
                        bola_cae_sobre_item = 1;
                        break;
                    }
                    i++;
                }
                if(i >= ITEMS_NUM)
                {
                    bola_cae_sobre_item = 0;
                }
            }
        }
    }
    // Angulo inicial es al azar entre 0 y 65535 quitando el cono superior e inferior
    juego->bola.angulo = (unsigned short)(rand() % (int)65536);
    if(((juego->bola.angulo > 8192 /* 45 grados en el sistema sexagesimal, 45*182.044444444444=8192 */) && (juego->bola.angulo < 24576 /* 90+45 grados en el sistema sexagecinal */)) ||
        ((juego->bola.angulo > 40960 /* 180+45 grados en el sistema sexagecinal */) && (juego->bola.angulo < 57344 /* 270+45 grados en el sistema sexagecinal */)))
    {
        // Si desborda por valer de 49152 a 65535, no me interesa, se autoacomodará a un valor de 0 a 16383.
        juego->bola.angulo += 16384; // 90 grados en el sistema sexagecinal
    }
    // La velocidad de la Bola no cambia cuando algún jugador pierde un punto, continúa igual. Solo se resetea cuando inicia una nueva partida.
	//juego->bola.ticks_mover = CLOCKS_PER_SEC / 10; // 50000;
	// Cuando inicia una bola, la máquina tiene reacción plena
	quitar_limite_reaccion_maquina(juego);
    // Cuando inicia una bola, ambos jugadores comienzan a jugar con su barra centrada
	juego->jugador.y = juego->maquina.y = ((juego->altoTablero - BARRA_Y_INICIAL) / 2) - (BARRA_Y_INICIAL / 2) + juego->inicio_tablero_y;
    // Cuando inicia una bola, ambos jugadores tienen una barra con alto normal
	juego->jugador.largo_actual = juego->maquina.largo_actual = BARRA_Y_INICIAL;
    // Cuando inicia una bola el jugador tienen la velocidad estándard en su barra sin efectos
    juego->jugador.ticks_mover = CLOCKS_PER_SEC / 10;
    juego->jugador.ticks_efecto = 0;
    borrar_toda_barra_jugador(juego);
    mostrar_barra_jugador(juego);
    // Cuando inicia una bola la máquina tienen la velocidad estándard en su barra sin efectos
    juego->maquina.ticks_mover = CLOCKS_PER_SEC / 10;
    juego->maquina.ticks_efecto = 0;
    borrar_toda_barra_maquina(juego);
    mostrar_barra_maquina(juego);
    // Mostrar la bola
    mvaddch(COORDENADA_Y_PANTALLA_BOLA(juego), COORDENADA_X_PANTALLA_BOLA(juego), BOLA_CHAR);
}

void mostrar_bola(Juego *juego)
{
    mvaddch(COORDENADA_Y_PANTALLA_BOLA(juego), COORDENADA_X_PANTALLA_BOLA(juego), BOLA_CHAR);
}

void mover_bola(Juego *juego)
{
    // Se puede borrar y reescribir la bola porque el refresh de NCurses/PDCurses, evita el parpadeo de la bola
    mvaddch(COORDENADA_Y_PANTALLA_BOLA(juego), COORDENADA_X_PANTALLA_BOLA(juego), VACIO_CHAR);
    // Noto que PDCurses tiene el bug de poner un punto de color en alguna situación, que no se corrije con mvaddstr, este bug con ncurses en linux, no ocurre
    //mvaddstr(COORDENADA_Y_PANTALLA_BOLA(juego), COORDENADA_X_PANTALLA_BOLA(juego) - 1, "   ");
	if(juego->bola.angulo < 16384 /* 90 grados en el sistema sexagecimal */)
	{
        // El ángulo vale desde 0 a 16383: derecha arriba
        // Derecha con índice de 16383 a 0 equivalente al coseno del ángulo, coseno = adyacente porque la diagoran es 1
		juego->bola.xFino += delta_angulo[16383 /* 90 grados en el sistema sexagecimal */ - juego->bola.angulo];
        // Arriba con índice del vector de 0 a 16383 equivalente al seno del ángulo, seno = ordenada porque la diagonal es 1
		juego->bola.yFino -= delta_angulo[juego->bola.angulo];
        // Verificar si la bola rebota en el muro superior, tener en cuenta que juego->bola.yFino es un double, por lo tanto, puede ser negativo
        if(juego->bola.yFino < 0)
        {
            // Cambiar el ángulo para que vaya derecha abajo
            juego->bola.angulo = (unsigned short)(65536.0 /* 360 grados en el sistema sexagesimal */ - ((double)(juego->bola.angulo)));
            // La definición del ejercicio solicita: la máquina tendrá un tiempo de reacción aleatorio a cambios de dirección
            // ¿A cambios de diracción de la bola o a los de la barra de la máquina? Para cambio de dirección de la bola, habría que poner un "limitar_reaccion_maquina(juego);" en este punto
            // Asumo por sentido común, que la definición del ejercicio se refiere a cambios de dirección de la barra de la máquina
        }
	}
	else
    {
        if((juego->bola.angulo >= 16384 /* 90 grados en el sistema sexagecimal */) && (juego->bola.angulo < 32768 /* 180 grados en el sistema sexagecimal */))
        {
            // El ángulo vale desde 16384 a 32767: izquierda arriba
            // Izquierda con indice de 0 a 16383 equivalente al seno del ángulo, seno = ordenada porque la diagonal es 1
            juego->bola.xFino -= delta_angulo[juego->bola.angulo - 16384 /* 90 grados en el sistema sexagecimal */];
            // Arriba con índice de 16383 a 0 equivalente al coseno del ángulo, coseno = adyacente porque la diagonal es 1
            juego->bola.yFino -= delta_angulo[16383 /* 90 grados en el sistema sexagecimal */ - juego->bola.angulo + 16384 /* 90 grados en el sistema sexagecimal */];
            // Verificar si la bola rebota en el muro superior, tener en cuenta que juego->bola.yFino es un double, por lo tanto, puede ser negativo
            if(juego->bola.yFino < 0)
            {
                // Cambiar el ángulo para que vaya a izquierda abajo
                juego->bola.angulo = (unsigned short)(65536.0 /* 360 grados en el sistema sexagesimal */ - ((double)(juego->bola.angulo)));
            }
        }
        else
        {
            if((juego->bola.angulo >= 32768 /* 180 grados en el sistema sexagecimal */) && (juego->bola.angulo < 49152 /* 270 grados en el sistema sexagecimal */))
            {
                // El ángulo vale desde 32768 a 49151: izquierda abajo
                // Izquierda con índice de 16383 a 0 equivalente al coseno del ángulo, coseno = adyacente porque la diagoran es 1
                juego->bola.xFino -= delta_angulo[16383 /* 90 grados en el sistema sexagecimal */ - juego->bola.angulo + 32768 /* 180 grados en el sistema sexagecimal */];
                // Abajo con índice de 0 a 16383 equivalente al seno del ángulo, seno = ordenada porque la diagonal es 1
                juego->bola.yFino += delta_angulo[juego->bola.angulo - 32768 /* 180 grados en el sistema sexagecimal */];
                // Verificar si la bola rebota en muro inferior, tener en cuenta que juego->bola.yFino es un double, por lo tanto, casteo altoTablero para comparar double con double
                if(juego->bola.yFino > ((double)(juego->altoTablero) - 1.0))
                {
                    // Cambiar el ángulo para que vaya a izquierda arriba
                    juego->bola.angulo = (unsigned short)(65536.0 /* 360 grados en el sistema sexagesimal */ - ((double)(juego->bola.angulo)));
                }
            }
            else
            {
                if(juego->bola.angulo >= 49152 /* 270 grados en el sistema sexagecimal */)
                {
                    // El ángulo vale desde 49152 a 65535: derecha abajo
                    // Derecha con índice de 0 a 16383 equivalente al seno del ángulo, seno = ordenada porque la diagonal es 1
                    juego->bola.xFino += delta_angulo[juego->bola.angulo - 49152 /* 270 grados en el sistema sexagecimal */];
                    // Abajo con índice de 16383 a 0 equivalente al coseno del ángulo, coseno = adyacente porque la diagoran es 1
                    juego->bola.yFino += delta_angulo[65535 /* 360 grados en el sistema sexagecimal */ - juego->bola.angulo];
                    // Verificar si la bola rebota en muro inferior, tener en cuenta que juego->bola.yFino es un double, por lo tanto, casteo altoTablero para comparar double con double
                    if(juego->bola.yFino > ((double)(juego->altoTablero) - 1.0))
                    {
                        // Cambiar el ángulo para que vaya a derecha arriba
                        juego->bola.angulo = (unsigned short)(65536.0 /* 360 grados en el sistema sexagesimal */ - ((double)(juego->bola.angulo)));
                    }
                }
            }
        }
    }
    juego->bola.x = (unsigned short)(juego->bola.xFino);
    juego->bola.y = (unsigned short)(juego->bola.yFino);
	// Verificar si la Bola rebota con la máquina
    if ((juego->bola.x >= (juego->anchoTablero - 2 - BARRA_X)) && (juego->bola.y >= juego->maquina.y) && (juego->bola.y <= (juego->maquina.y + juego->maquina.largo_actual)) &&
        ((juego->bola.angulo < 16383 /* 90 grados en el sistema sexagecimal */) || (juego->bola.angulo > 49151 /* 270 grados en el sistema sexagecimal */)))
    {
        if((juego->bola.angulo >> 14) == 0)
        {
            // La Bola se dirige hacia la esquina superior derecha (0)
            // Cambiar para que se dirija a la esquina superior izquierda (1) y agregar un poco de azar al golpe
            juego->bola.angulo = (16383 /* 90 grados en el sistema sexagecimal */ - juego->bola.angulo + 16384 /* 90 grados en el sistema sexagecimal */) +
                (rand() % 7464 /* 41 grados en el sistema sexagecimal */) - 3641 /* 20 grados en el sistema sexagecimal */;
            // Si el ángulo queda muy vertical, acomodar
            if(juego->bola.angulo < 24576 /* 90+45 grados en el sistema sexagecimal */)
            {
                juego->bola.angulo = 24576 /* 135 grados en el sistema sexagecimal */;
            }
        }
        else
        {
            // La Bola se dirige hacia la esquina inferior derecha (3)
            // Cambiar para que se dirija a la esquina inferior izquierda (2) y agregar un poco de azar al golpe
            juego->bola.angulo = (16383 /* 90 grados en el sistema sexagecimal */ - juego->bola.angulo + 81920 /* 450 grados en el sistema sexagecimal */) +
                (rand() % 7464 /* 41 grados en el sistema sexagecimal */) - 3641 /* 20 grados en el sistema sexagecimal */;
            // Si el ángulo queda muy vertical, acomodar
            if(juego->bola.angulo > 40960 /* 180+45 grados en el sistema sexagecimal */)
            {
                juego->bola.angulo = 40960 /* 225 grados en el sistema sexagecimal */;
            }
        }
    }
	// Verificar si la Bola rebota con el jugador
    if ((juego->bola.x <= (1 + BARRA_X)) && (juego->bola.y >= juego->jugador.y) && (juego->bola.y <= (juego->jugador.y + juego->jugador.largo_actual)) &&
        (juego->bola.angulo > 16383 /* 90 grados en el sistema sexagecimal */) && (juego->bola.angulo < 49151 /* 270 grados en el sistema sexagecimal */))
    {
        // Corregir BUG aqui, juego->bola.y se iguala a 0 al rabotar en el jugador y no advierto dónde
        if ((juego->bola.angulo >> 14) == 1)
        {
            // La Bola se dirige hacia la esquina superior izquierda (1)
            // Cambiar para que se dirija a la esquina superior derecha (0) y agregar un poco de azar al golpe
            juego->bola.angulo = (32767 /* 180 grados en el sistema sexagecimal */ - juego->bola.angulo) +
                (rand() % 7464 /* 41 grados en el sistema sexagecimal */) - 3641 /* 20 grados en el sistema sexagecimal */;
            // Si el ángulo queda muy vertical, acomodar
            if(juego->bola.angulo > 8192 /* 45 grados en el sistema sexagecimal */)
            {
                juego->bola.angulo = 8192 /* 45 grados en el sistema sexagecimal */;
            }
        }
        else
        {
            // La Bola se dirige hacia la esquina inferior izquierda (2)
            // Cambiar para que se dirija a la esquina inferior derecha (3) y agregar un poco de azar al golpe
            juego->bola.angulo = (49151 /* 270 grados en el sistema sexagecimal */ - juego->bola.angulo + 49152 /* 270 grados en el sistema sexagecimal */) +
                (rand() % 7464 /* 41 grados en el sistema sexagecimal */) - 3641 /* 20 grados en el sistema sexagecimal */;
            // Si el ángulo queda muy vertical, acomodar
            if(juego->bola.angulo < 57344 /* 270+45 grados en el sistema sexagecimal */)
            {
                juego->bola.angulo = 57344 /* 270+45 grados en el sistema sexagecimal */;
            }
        }
    }
    mvaddch(COORDENADA_Y_PANTALLA_BOLA(juego), COORDENADA_X_PANTALLA_BOLA(juego), BOLA_CHAR);
}
