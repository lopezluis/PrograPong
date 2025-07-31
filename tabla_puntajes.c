#include <stdlib.h>         // por exit, EXIT_SUCCESS, EXIT_FAILURE
#include <ncurses.h>        // por clear, refresh
#include <time.h>           // por strftime
#include <stddef.h>         // por NULL
#include <string.h>         // por strlen
#include "juego.h"          // por Juego, PROGRAMA_TITULO, PROGRAMA_LARGO
#include "buffer_teclado.h" // por vaciar_buffer_teclado

void mostrar_tabla_puntajes(Juego *juego)
{
	// Limpia la pantalla
	clear();
	mvaddstr(0, (juego->anchoTablero - PROGRAMA_LARGO) / 2, PROGRAMA_TITULO);
	mvaddstr(1, (juego->anchoTablero - DESARROLLADOR_LARGO) / 2, DESARROLLADOR_TITULO);
	if((juego->hallOfFame[0]).nombreJugador[0] == '\x00')
	{
		mvaddstr(3, (juego->anchoTablero - 40) / 2, "La tabla de mejores puntajes está vacía.");
        mvaddstr(5, (juego->anchoTablero - 45) / 2, "Presione una tecla para ir al menú principal.");
	}
	else
	{
		mvaddstr(3, 0, "Mejores puntajes:");
		// Saco la declaraciòn de i del for para que quede locar a este àmbito y utilizarla en Presione una tecla...
		unsigned int i = 0;
		while((i < 5) && ((juego->hallOfFame[i]).nombreJugador[0] != '\x00'))
		{
			if((juego->hallOfFame[i]).nombreJugador[0] == '\x00')
            {
                break;
            }
            else
			{
			    // Los instantes tendrán un ancho fijo de 19 caracteres, strAux nunca tendrá más de 35 caracteres
				char strInstante[20], strAux[36];
				struct tm *info;
				// Convertir el valor de tipo time_t a estructura tm
				info = localtime(&((juego->hallOfFame[i]).instante));
				// Esta forma de mostrar fechas, es de mi agrado (ordenado por significancia de dìgitos), màs naturalmente serìa %d/%m/%Y %H:%M:%S
				strftime(strInstante, sizeof(strInstante), "%Y-%m-%d %H:%M:%S", info);
				sprintf(strAux, "%s %s %5d", (juego->hallOfFame[i]).nombreJugador, strInstante, (juego->hallOfFame[i]).puntaje);
				mvaddstr(i + 5, 0, strAux);
			}
            i++;
		}
        mvaddstr(i + 6, (juego->anchoTablero - 45) / 2, "Presione una tecla para ir al menú principal.");
	}

	// Actualizar la pantalla
	refresh();

	// Esperar a que el usuario presione una tecla
	while(getch() == ERR);
}

void iniciar_tabla_mejores_puntajes(Juego *juego)
{
	// Asignación de memoria para la tabla de mejores puntajes
	for (unsigned int i = 0; i < CANTIDAD_HALL_OF_FAME; i++)
	{
		juego->hallOfFame[i].nombreJugador = (char *)malloc(ESPACIO_NOMBRE_JUGADOR);
		// Verificar que malloc tuvo éxito
		if (juego->hallOfFame[i].nombreJugador == NULL)
		{
			if (fputs("Error: No hay suficiente memoria para almacenar la tabla de mejores puntajes.\n", stderr) == EOF)
			{
				// La función perror mostrará el mensaje del parámetro ingresado, seguido de dos puntos, espacio, mensaje de error del sistema y salto de línea
				perror("Error interno");
			}
			exit(EXIT_FAILURE);
		}
		juego->hallOfFame[i].nombreJugador[0] = '\x00';
		juego->hallOfFame[i].instante = 0;
		juego->hallOfFame[i].puntaje = 0;
	}
}

void finalizar_tabla_mejores_puntajes(Juego *juego)
{
	// No olvidar destruir la memoria utilizada para los registros MayorPuntaje
	for (unsigned int i = 0; i < CANTIDAD_HALL_OF_FAME; i++)
	{
		free(juego->hallOfFame[i].nombreJugador);
	}
}

void leer_tabla_mejores_puntajes(Juego *juego)
{
	FILE *archivo = fopen("hallOfFame.bin", "rb"); // Abrir archivo en modo binario para lectura
	if (archivo == NULL)
	{
		// No existe el archivo, entonces la tabla de mejores puntajes está vacía, ya está inicializada como vacía, por lo tanto solo salgo
		return;
	}

	// Si la tabla de mejores puntajes no esté completa almacenada en disco, habría un error, significa que fue generado el archivo por otra aplicación o manualmente
	for (unsigned int i = 0; i < CANTIDAD_HALL_OF_FAME; i++)
	{
		// Leer la estructura del archivo
		size_t elementos_leidos = fread(juego->hallOfFame[i].nombreJugador, ESPACIO_NOMBRE_JUGADOR, 1, archivo);
		if (elementos_leidos != 1)
		{
			fclose(archivo);
			if (fputs("Error: En la lectura del archivo con el nombre del jugador.\n", stderr) == EOF)
			{
				perror("Error interno");
			}
			exit(EXIT_FAILURE);
		}
		elementos_leidos = fread(&(juego->hallOfFame[i].instante), sizeof(time_t) + sizeof(unsigned int), 1, archivo);
		if (elementos_leidos != 1)
		{
			fclose(archivo);
			if (fputs("Error: En la lectura del archivo con los mejores puntajes.\n", stderr) == EOF)
			{
				perror("Error interno");
			}
			exit(EXIT_FAILURE);
		}
	}

	fclose(archivo); // Cerrar el archivo
}

void guardar_tabla_mejores_puntajes(Juego *juego)
{
	FILE *archivo;

	archivo = fopen("hallOfFame.bin", "wb"); // Abrir archivo en modo binario para escritura
	if (archivo == NULL)
	{
		if (fputs("Error: Al crear el archivo con los mejores puntajes.\n", stderr) == EOF)
		{
			perror("Error interno");
		}
		exit(EXIT_FAILURE);
	}
	for (unsigned int i = 0; i < CANTIDAD_HALL_OF_FAME; i++)
	{
		// Escribir la estructura en el archivo
		size_t elementos_escritos = fwrite(juego->hallOfFame[i].nombreJugador, ESPACIO_NOMBRE_JUGADOR, 1, archivo);
		if (elementos_escritos != 1)
		{
			fclose(archivo);
			if (fputs("Error: En la escritura del archivo con el nombre del jugador.\n", stderr) == EOF)
			{
				perror("Error interno");
			}
			exit(EXIT_FAILURE);
		}
		elementos_escritos = fwrite(&(juego->hallOfFame[i].instante), sizeof(time_t) + sizeof(unsigned int), 1, archivo);
		if (elementos_escritos != 1)
		{
			fclose(archivo);
			if (fputs("Error: En la escritura del archivo con los mejores puntajes.\n", stderr) == EOF)
			{
				perror("Error interno");
			}
			exit(EXIT_FAILURE);
		}
	}
	fclose(archivo); // Cerrar el archivo
}

void agregar_nuevo_record(Juego *juego, unsigned int puntajeObtenido)
{
	// Es requerimiento que se llame a esta función con (puntajeObtenido > 0) && (puntajeObtenido > juego->hallOfFame[CANTIDAD_HALL_OF_FAME - 1].puntaje (último de la tabla)).
	// Por las dudas lo verificamos, si este código se reutiliza, puede ser una verificación necesaria
	if((puntajeObtenido = 0) && (puntajeObtenido <= juego->hallOfFame[CANTIDAD_HALL_OF_FAME - 1].puntaje))
	{
		return;
	}
	// Si la tabla está vacía, i = 0
	unsigned int i = 0;
	if(juego->hallOfFame[0].nombreJugador[0] != '\x00')
	{
		unsigned int j;
		i = j = CANTIDAD_HALL_OF_FAME - 1;
		while((i > 0) && (juego->hallOfFame[i].nombreJugador[0] == '\x00') && ((juego->hallOfFame[i].nombreJugador[0] != '\x00') && (juego->hallOfFame[i].puntaje < puntajeObtenido)))
		{
			i--;
		}
		if((juego->hallOfFame[i].nombreJugador[0] != '\x00') && (juego->hallOfFame[i].puntaje >= puntajeObtenido))
		{
			i++;
		}
		// i es el indice de la tabla juego->hallOfFame donde debe insertarse el puntaje obtenido
		// Generamos el espacio
		while(i < j)
		{
			strcpy(juego->hallOfFame[j].nombreJugador, juego->hallOfFame[j - 1].nombreJugador);
			juego->hallOfFame[j].instante = juego->hallOfFame[j - 1].instante;
			juego->hallOfFame[j].puntaje = juego->hallOfFame[j - 1].puntaje;
			j--;
		}
	}
	vaciar_buffer_teclado(juego);
	mvaddstr(juego->altoTablero + 3, 40, "Ha ingresado en el hall of fame.     ");
	mvaddstr(juego->altoTablero + 4, 40, "Nombre hasta 9 carácteres:           ");
	refresh();
	// Contexto para acotar la variable nombre e k
    {
        char nombre[10];
        unsigned short k = 0;
        for(; k < 9; k++)
        {
            nombre[k] = ' ';
        }
        nombre[k] = '\x00';
        k = 0;
		// Acotamos continuar_ingresando
        {
            unsigned char continuar_ingresando = 1;
            while(continuar_ingresando)
            {
				char caracter;
                switch(caracter = getch())
                {
                case '\x08': // Backspace
                    if(k > 0)
                    {
                        k--;
                        nombre[k] = ' ';
                        mvaddstr(juego->altoTablero + 4, 67, nombre);
                        refresh();
                    }
                    break;
                case '\x0A': // Enter
                    continuar_ingresando = 0;
                    break;
                default:
                    if((k < 9) && ((caracter == ' ') || ((caracter >= 'A') && (caracter <= 'Z')) || ((caracter >= 'a') && (caracter <= 'z'))))
                    {
                        nombre[k] = caracter;
                        k++;
                        mvaddstr(juego->altoTablero + 4, 67, nombre);
                        refresh();
                    }
                    break;
                }
            }
        }
        strcpy(juego->hallOfFame[i].nombreJugador, nombre);
    }
	// Obtener el instante actual
	time(&(juego->hallOfFame[i].instante));
	juego->hallOfFame[i].puntaje = puntajeObtenido;
	guardar_tabla_mejores_puntajes(juego);
	mvaddstr(juego->altoTablero + 3, 40, "                                     ");
	mvaddstr(juego->altoTablero + 4, 40, "                                     ");
}

unsigned int ultimo_mejor_puntaje(Juego *juego)
{
    // Si hay algún espacio vacío entonces el último puntaje es 0
    if(juego->hallOfFame[CANTIDAD_HALL_OF_FAME - 1].nombreJugador[0] == '\x00')
    {
        return 0;
    }
    return juego->hallOfFame[CANTIDAD_HALL_OF_FAME - 1].puntaje;
}
