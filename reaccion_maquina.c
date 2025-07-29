#include <stdlib.h> // por rand
#include <time.h>   // por clock_t, clock, CLOCKS_PER_SEC
#include "juego.h"  // por Juego

// Establecer período en que la máquina se encuentra baja de reacción
// Como está indicado en el pdf "Problema especial 1er semestre 2025 PrograPong"
// al indicar "la máquina tendrá un tiempo de reacción aleatorio a cambios de dirección"
void limitar_reaccion_maquina(Juego *juego)
{
    juego->ini_limite_reaccion_maquina = clock();
    // Agregar tiempo aleatorio entre 1 y 10 segundos, quizás valores cercanos a los 10 segundos es mucho, porque la máquina queda muy "tonta"
    //juego->fin_limite_reaccion_maquina = juego->ini_limite_reaccion_maquina + ((rand() % (CLOCKS_PER_SEC * 10)) + CLOCKS_PER_SEC);
    // Agregar tiempo aleatorio entre 2 y 5 segundos
    juego->fin_limite_reaccion_maquina = juego->ini_limite_reaccion_maquina + ((rand() % (4 * CLOCKS_PER_SEC)) + (2 * CLOCKS_PER_SEC));
}

// ¿La maquina está dentro del período de límite de reacciónj? != 0 = si, 0 = no
int maquina_con_limite_reaccion(Juego *juego)
{
    // Obtener cantidad de ticks del reloj
    clock_t ticks_reloj_actual = clock();
    return ((juego->ini_limite_reaccion_maquina < ticks_reloj_actual) && (juego->fin_limite_reaccion_maquina > ticks_reloj_actual));
}

// Quitar límite de reacción, se llamará cuando jugador o máquina pierdfa un turno
void quitar_limite_reaccion_maquina(Juego *juego)
{
    juego->ini_limite_reaccion_maquina = juego->fin_limite_reaccion_maquina = clock();
}
