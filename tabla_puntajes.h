#ifndef TABLA_PUNTAJES_H_
#define TABLA_PUNTAJES_H_

#include "juego.h" // por Juego

void mostrar_tabla_puntajes(Juego *);
void iniciar_tabla_mejores_puntajes(Juego *);
void finalizar_tabla_mejores_puntajes(Juego *);
void leer_tabla_mejores_puntajes(Juego *);
void agregar_nuevo_record(Juego *, unsigned int);
void guardar_tabla_mejores_puntajes(Juego *);
unsigned int ultimo_mejor_puntaje(Juego *);

#endif // TABLA_PUNTAJES_H_
