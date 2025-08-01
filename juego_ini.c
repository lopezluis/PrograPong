#include <stdio.h>            // por stderr, fputs, perror, EOF (End-Of-File)
#include <stddef.h>           // por NULL
#include <stdlib.h>           // por rand, exit, EXIT_SUCCESS, EXIT_FAILURE, malloc (recordar que malloc.h está deprecado)
#include "juego.h"            // por Juego, Modo AVENTURA, ESPACIO_NOMBRE_JUGADOR
#include "tabla_puntajes.h"   // por leer_tabla_mejores_puntajes
#include "tamanio_terminal.h" // por tamanio_terminal
#include "bola.h"             // por inicializar_bola

// Se crea la estructura del juego en heap, para luego devolverla y que no se destruya por ser local a juego_ini
// Es conveniente devolver el puntero a la estructura para evitar la transferencia de memoria, en el caso que la cantidad de escacio necesario para la estructura crezca en el futuro

// Inicialización de la estructura Juego
Juego *juego_ini(void)
{
	// Reserva de memoria para la estructura en el heap
	Juego *juego = (Juego *)malloc(sizeof(Juego));
	// Verificar que malloc tuvo éxito
	if (juego == NULL)
	{
		if (fputs("Error: No hay suficiente memoria para almacenar la estructura del juego.\n", stderr) == EOF)
		{
			// La función perror mostrará el mensaje del parámetro ingresado, seguido de dos puntos, espacio, mensaje de error del sistema y salto de línea
			perror("Error interno");
		}
		exit(EXIT_FAILURE);
	}

    unsigned short anchoPantalla = 80, altoPantalla = 16;
    tamanio_terminal(&anchoPantalla, &altoPantalla);
    // Para verificar tamaño escaso, tener en cuenta que la tabla de puntajes tiene un alto de 12 líneas y el tablero no tiene sentido que sea < a
    // BARRA_Y_INICIAL(5) + 3 * 2(espacio superior e inferior para mover la barra) + 5 (titulo, 2 de estado y 2 muros)
    // El ancho del tablero de 80 caracteres, es aceptable
    if((anchoPantalla < 80) || (altoPantalla < 16))
    {
		if (fputs("Error: El área para desplegar el juego es muy pequeña.\nPor favor, agrande la pantalla antes de ejecutar el juego.\n", stderr) == EOF)
		{
			// La función perror mostrará el mensaje del parámetro ingresado, seguido de dos puntos, espacio, mensaje de error del sistema y salto de línea
			perror("Error interno");
		}
		exit(EXIT_FAILURE);
    }
	// Asignación de valores a los miembros de la estructura
	juego->anchoTablero = anchoPantalla;
    // El alto del tablero, es el alto de la pantalla restado la barra de título, la barra de estado y los muros
	juego->altoTablero = altoPantalla - 5;
	// La columna del inicio del tablero es en la columna 0, coincide con el inicio de la pantalla, dado que el tablero ocupa todo el ancho de la pantalla
	juego->inicio_tablero_x = 0;
	// La fila del tablero es 2, dado que hay que sumar la barra de título y la pared superior
	juego->inicio_tablero_y = 2;
	// Inicialización de la dirección de la barra de la máquina
	juego->direccion_barra_maquina = INDEFINIDA;

    // Sumo 2 por la barra de título y el muro superior
	juego->jugador.y = juego->maquina.y = ((juego->altoTablero - BARRA_Y_INICIAL) / 2) + 2;
	juego->jugador.largo_actual = juego->maquina.largo_actual = BARRA_Y_INICIAL;
    juego->jugador.ticks_mover = juego->maquina.ticks_mover = 50000;
    // La máquina inicia normalmente, sin tener limitada su reacción
    juego->ini_limite_reaccion_maquina = clock();
    juego->fin_limite_reaccion_maquina = clock();

	juego->puntos = 0;
	juego->modo = AVENTURA;
	juego->vidas = 5;
	juego->nivel = 1;

	inicializar_bola(juego);

    // Retorno del puntero a la estructura creada
	return juego;
}

void juego_fin(Juego **juego)
{
    if(*juego != NULL)
    {
        // Destruir memoria utilizada por variables del juego
        free(*juego);
        // Marcar que el juego no está inicializado
        *juego = NULL;
    }
}
