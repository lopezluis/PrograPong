#include <ncurses.h>        // por clear, mvaddstr, mvaddch(comentado), refresh, getch, mvprintw
#include <stdlib.h>         // por malloc, free
#include <time.h>           // por clock_t, clock, CLOCKS_PER_SEC(está definido en /usr/include/x86_64-linux-gnu/bits/time.h)
#include "juego.h"          // por Juego, MURO_CHAR, VACIO_CHAR, ACCION_ARRIBA, ACCION_ABAJO, ACCION_PAUSA, ACCION_QUITAR, BARRA_Y_INICIAL, SUPERVIVENCIA, AVENTURA
#include "mover_jugador.h"  // por mostrar_barra_jugador, mostrar_barra_maquina, mover_jugador_arriba, mover_jugador_abajo, mover_maquina, borrar_toda_barra_jugador, borrar_toda_barra_maquina
#include "bola.h"           // por mostrar_bola, mover_bola, inicializar_bola
#include "tabla_puntajes.h" // por ultimo_mejor_puntaje, agregar_nuevo_record
#include "item.h"           // por item_procesar
#include "buffer_teclado.h" // por vaciar_buffer_teclado

#define ABANDONO_DESCARTA 0
#define ABANDONO_CONFIRMA 1
#define ABANDONO_NO 0
#define ABANDONO_SI 1

// Este función es privada de esta fuente, no debe estar en el encabezado, ni en tabla_puntajes
unsigned char verificar_tabla_puntaje(Juego *juego, unsigned char abandono)
{
    unsigned char confirma = ABANDONO_DESCARTA;
    // Si abandona, puede haber ingresado igualmente en el hall of fame
    if(abandono)
    {
        // Recordar que cuando escribimos en juego->altoTablero + 4, debemos evitar la escritura de un caracter en la última fila y la última columna, para evitar el scroll de la pantalla completa
        mvaddstr(juego->altoTablero + 4, 40, "¿Abandona? S abandona,otra tecla cont.");
        refresh();
        char caracter;
        while((caracter = getch()) == ERR);
        mvaddstr(juego->altoTablero + 4, 40, "                                      ");
        if((caracter != 'S') && (caracter != 's'))
        {
            return confirma;
        }
        confirma = ABANDONO_CONFIRMA;
    }
    if((juego->puntos > 0) && (juego->puntos > ultimo_mejor_puntaje(juego)))
    {
        agregar_nuevo_record(juego, juego->puntos);
        guardar_tabla_mejores_puntajes(juego);
    }
    else
    {
        mvaddstr(juego->altoTablero + 4, 40, "Ha perdido. Presione una tecla.        ");
        refresh();
        while(getch() == ERR);
    }
    return confirma;
}

// función llamada desde un bucle while(1){} en main.
void juego_correr(Juego *juego)
{
	// Limpia la pantalla
	clear();
	// Nota: juego->anchoTablero es equivalente a (*juego).anchoTablero, en este desarrollo se empleará -> para mayor legibilidad del código
	// Mostrar barra de título
	mvaddstr(0, (juego->anchoTablero - PROGRAMA_LARGO) / 2, PROGRAMA_TITULO);
    // Mostrar los muros, genero un nuevo contexto para acotar el ámbito de strAux y aprovechar mejor las optimizaciones del compilador
    {
        char *strAux = (char *)malloc(juego->anchoTablero + 1);
        // Verifir que malloc tuvo éxito
        if (strAux == NULL)
        {
            if (fputs("Error: No hay suficiente memoria la cadena auxiliar para mostrar los muros.\n\r", stderr) == EOF)
            {
                // La función perror mostrará el mensaje del parámetro ingresado, seguido de dos puntos, espacio, mensaje de error del sistema y salto de línea
                perror("Error interno");
            }
            exit(EXIT_FAILURE);
        }
        // Acotamos el ámbito de i
        {
            unsigned int i = 0;
            for (; i < juego->anchoTablero; i++)
            {
                strAux[i] = MURO_CHAR;
            }
            strAux[i] = '\x00';
        }
        mvaddstr(1, 0, strAux);
        // Agrego 2 por la barra de título y el muro superior
        mvaddstr(juego->altoTablero + 2, 0, strAux);
        // Destruir memoria utilizada por variable strAux
        free(strAux);
        // En este caso no haría falta anular el puntero, lo hago solo por buena práctica
        strAux = NULL;
    }
	// Mostrar las barra
	mostrar_barra_jugador(juego);
	mostrar_barra_maquina(juego);
	// Mostrar la Bola
    mostrar_bola(juego);

	// Mostrar barra de estado (información inferior): Puntos, Vidas, Modo y Nivel
	mvprintw(juego->altoTablero + 3, 0, "Puntos: %05u", juego->puntos);
	mvprintw(juego->altoTablero + 3, 22, "| Vidas: %02hu", juego->vidas);
	mvprintw(juego->altoTablero + 4, 0, "Modo  : %s", (juego->modo == AVENTURA) ? "AVENTURA     " : ((juego->modo == SUPERVIVENCIA) ? "SUPERVIVENCIA" : "DESCONOCIDO  "));
	mvprintw(juego->altoTablero + 4, 22, "| Nivel: %02hu", juego->nivel);

    mvaddstr(juego->altoTablero + 4, 40, "Presione una tecla para iniciar.       ");
	// Actualizar la pantalla
	refresh();
    while(getch() == ERR);

    // Obtener cantidad de ticks del reloj
    clock_t ticks_reloj_actual = clock();
    // Temporizador por si se está jugando en modo SUPERVIVBNCIA
    clock_t retardoAcelBolaEnSuperv = ticks_reloj_actual + (10 * CLOCKS_PER_SEC);
    // El retardoRefrescoBola no es una característica de la Bola, sino de la implementación, por lo tanto, no es parte de la estructuira Bola
	clock_t retardoRefrescoBola = ticks_reloj_actual + juego->bola.ticks_mover;
    // Los retardoRefrescoJugador y retardoRefrescoMaquina no son una característica de las respectivas Barras, sino de la implementación, no es parte de la estructuira Barra
    // Las barras pueden tener una cantidad de ticks para su movimiento que sea diferente una de otra, mientras están bajo el efecto de algún item que afecto su velocidad
    // La barra del jugador se controla por teclado, se genera el típico problema del uso de teclados en juegos: ver cómo reconocer tecla presionada durante lapso previo a repetición, e inter-repetición
    clock_t retardoRefrescoJugador = ticks_reloj_actual + juego->jugador.ticks_mover;
    clock_t retardoRefrescoMaquina = ticks_reloj_actual + juego->maquina.ticks_mover;
	while(1)
	{
        if(ticks_reloj_actual > retardoRefrescoJugador)
        {
            // En lugar de getch() debería emplearse una técnica que devuelva si las teclas están presionadas o liberadas, sin presionar
            // Así como está, tiene el problema de toma las presiones de tecla como hayan sido configuradas en BIOS o el teclado según la marca:
            // duración de pulsación, tiempo de espera previo a repetición y frecuencia de repetición.
            // Leer entrada del jugador
            switch(getch())
            {
            case ACCION_ARRIBA: // Barra del jugador arriba
                // Tener en cuenta que la versión de ncurses no informa la liberación de una tecla, por lo tanto,
                // la frecuencia al mantenerla presionada, asumo será adecuada
                mover_jugador_arriba(juego);
                break;
            case ACCION_ABAJO:  // Barra del jugador abajo
                mover_jugador_abajo(juego);
                break;
            case ACCION_PAUSA:  // Pausar ejecución del juego
                {
                    vaciar_buffer_teclado(juego);
                    mvaddstr(juego->altoTablero + 4, 40, "P para Principal u otra continuar.     ");
                    refresh();
                    char caracter;
                    while((caracter = getch()) == ERR);
                    mvaddstr(juego->altoTablero + 4, 40, "                                       ");
                    if((caracter == 'P') || (caracter == 'p'))
                    {
                        // Se desea ir al manú principal, se chequea si ingresa en el hall of fame
                        verificar_tabla_puntaje(juego, ABANDONO_NO);
                        // No hace falta verificar nada más en el juego
                        return;
                    }
                }
                break;
            case ACCION_QUITAR:
                // Está abandonando, se pregunta para confirmar abandono y se chequea si ingresaa en el hall of fame
                if(verificar_tabla_puntaje(juego, ABANDONO_SI) == ABANDONO_CONFIRMA)
                {
                    // No hace falta verificar nada más en el juego
                    return;
                }
                break;
            }
            retardoRefrescoJugador = ticks_reloj_actual + juego->jugador.ticks_mover;
        }
        // Verificar si hay que finalizar algún efecto activo en el jugador
        if((juego->jugador.ticks_efecto != 0) && (ticks_reloj_actual > juego->jugador.ticks_efecto))
        {
            borrar_toda_barra_jugador(juego);
            // Si finaliza el efecto de bloqueo, entonces la posición y de la barra se pondrá al azar
            if(juego->jugador.largo_actual == juego->altoTablero)
            {
                juego->jugador.y = rand() % (juego->altoTablero - BARRA_Y_INICIAL);
            }
            // desactivar el efecto
            juego->jugador.largo_actual = BARRA_Y_INICIAL;
            // Si el jugador tiene un efecto de reducción y está posicionado abajo de todo, al restaurar al tamaño original, la barra del jugador romperá el muro, controlar este efecto
            if((juego->jugador.y + juego->jugador.largo_actual) > juego->altoTablero)
            {
                juego->jugador.y = juego->altoTablero - juego->jugador.largo_actual;
            }
            // juego->jugador.ticks_mover en la práctica no tiene efecto, porque la definición indica de manera incorrecta, que se debe emplear el teclado para mover la barra del jugador
            juego->jugador.ticks_mover = 50000; // CLOCKS_PER_SEC / 10;
            juego->jugador.ticks_efecto = 0;
            mostrar_barra_jugador(juego);
        }
        // Verificar si llegó el momento de actualizar la barra de la máquina
        if(ticks_reloj_actual > retardoRefrescoMaquina)
        {
            // Realizar movimiento de la barra de la máquina
            mover_maquina(juego);
            retardoRefrescoMaquina = ticks_reloj_actual + juego->maquina.ticks_mover;
        }
        // Verificar si hay que finalizar algún efecto activo en la máquina
        if((juego->maquina.ticks_efecto != 0) && (ticks_reloj_actual > juego->maquina.ticks_efecto))
        {
            borrar_toda_barra_maquina(juego);
            // Si finaliza el efecto de bloqueo, entonces la posición y de la barra se pondrá al azar
            if(juego->maquina.largo_actual == juego->altoTablero)
            {
                juego->maquina.y = rand() % (juego->altoTablero - BARRA_Y_INICIAL);
            }
            // desactivar el efecto
            juego->maquina.largo_actual = BARRA_Y_INICIAL;
            // Si la maquina tiene un efecto de reducción y está posicionada abajo de todo, al restaurar al tamaño original, la barra de la maquina romperá el muro, controlar este efecto
            if((juego->maquina.y + juego->maquina.largo_actual) > juego->altoTablero)
            {
                juego->maquina.y = juego->altoTablero - juego->maquina.largo_actual;
            }
            juego->maquina.ticks_mover = 50000; // CLOCKS_PER_SEC / 10;
            juego->maquina.ticks_efecto = 0;
            mostrar_barra_maquina(juego);
        }
		// Actualizar movimiento de la Bola y verificación de colisiones
		if(ticks_reloj_actual > retardoRefrescoBola)
		{
			mover_bola(juego);
			// TODO En la primera implementación vamos a temporizar los items igual que la Bola, es casi seguro que necesiten su frecuencia de procesamiento individual
            item_procesar(juego);
			retardoRefrescoBola = ticks_reloj_actual + juego->bola.ticks_mover;
		}
		// Si estamos jugando en modo SUPERVIVENCIA, chequear si se debe acelerar la Bola
        if((juego->modo == SUPERVIVENCIA) && (ticks_reloj_actual > retardoAcelBolaEnSuperv))
        {
            // Acelerar la Bola un 10%
            juego->bola.ticks_mover -= juego->bola.ticks_mover / 10;
            retardoAcelBolaEnSuperv = ticks_reloj_actual + (10 * CLOCKS_PER_SEC);
        }
        // Verificar si la máquina no pudo devolver la Bola y el jugador gana un punto
        if (juego->bola.x > (juego->anchoTablero - 1 - BARRA_X))
        {
            juego->puntos++;
            mvprintw(juego->altoTablero + 3, 0, "Puntos: %05u ", juego->puntos);
            if(juego->modo == AVENTURA)
            {
                unsigned short aux = juego->puntos / 5 + 1;
                if(juego->nivel != aux)
                {
                    juego->nivel = aux;
                    mvprintw(juego->altoTablero + 4, 22, "| Nivel: %02hu ", juego->nivel);
                    juego->vidas = 5;
                    mvprintw(juego->altoTablero + 3, 22, "| Vidas: %02hu", juego->vidas);
                    // Acelerar la Bola un 40%
                    juego->bola.ticks_mover -= juego->bola.ticks_mover * 4 / 10;
                    if(juego->nivel == 11)
                    {
                        // Ganó en modo AVENTURA. Recordar que el acho máximo de la pantalla, puede ser de hasta 80 caracteres, por lo tanto, los textos, nunca deben superar los 40 caracteres
                        mvaddstr(juego->altoTablero + 3, 40, "¡Felicidades! Ah ganado.                ");
                        // Recordar que cuando escribimos en juego->altoTablero + 4, debemos evitar la escritura de un caracter en la última fila y la última columna, para evitar el scroll de la pantalla completa
                        mvaddstr(juego->altoTablero + 4, 40, "Presione una tecla para iniciar.       ");
                        refresh();
                        while(getch() == ERR);
                        // Verificar si debe ingresar en el hall of fame
                        verificar_tabla_puntaje(juego, ABANDONO_NO);
                        return;
                    }
                }
            }
            inicializar_bola(juego);
            vaciar_buffer_teclado(juego);
            mvaddstr(juego->altoTablero + 4, 40, "Presione una tecla para jugar.         ");
            while(getch() == ERR);
            mvaddstr(juego->altoTablero + 4, 40, "                                       ");
        }
        // Verificar si el jugador no pudo devolver la Bola y pierde una vida
        if (juego->bola.x < 2)
        {
            // No es necesario ocultar la Bola
            //mvaddch(juego->bola.y + juego->inicio_tablero_y, juego->bola.x + juego->inicio_tablero_x, VACIO_CHAR);
            juego->vidas -= 1;
            mvprintw(juego->altoTablero + 3, 22, "| Vidas: %02hu  ", juego->vidas);
            // No hace falta el refresh, se hará a continuación
            if (juego->vidas == 0)
            {
                // Verificar si debe ingresar en el hall of fame
                verificar_tabla_puntaje(juego, ABANDONO_NO);
                return;
            }
            else
            {
                inicializar_bola(juego);
                vaciar_buffer_teclado(juego);
                mvaddstr(juego->altoTablero + 4, 40, "Presione una tecla para jugar.         ");
                refresh();
                while(getch() == ERR);
                mvaddstr(juego->altoTablero + 4, 40, "                                       ");
            }
        }

		// Actualizar e imprimir pantalla: estas dos funciones no hacen falta gracias a la utilización de la librería ncurses
//		juego_actualizar_pantalla(juego);
//		juego_imprimir(juego);

		// Verificación del final de la partida según modo de juego
		/* ... */

		// Esperar FRAME_MS unidad mínima de tiempo: esta operación no hace falta, dado que la temporización del juego se produce con su ejecución
//		Sleep(FRAME_MS);

		// Actualizar pantalla de ncurses para reflejar los cambios
		refresh();
        // Obtener cantidad de ticks del reloj para la siguiente vuelta del while
        ticks_reloj_actual = clock();
	}
}
