#ifndef JUEGO_INI_H_
#define JUEGO_INI_H_

#include "juego.h"        // por Juego

Juego *juego_ini(void);   // inicialización de la estructura Juego
void juego_fin(Juego **); // destrucción de la memoria alocada para el juego

#endif // JUEGO_INI_H_
