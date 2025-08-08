#ifndef JUEGO_H_
#define JUEGO_H_

#include <time.h> // por time_t, clock_t

// Zona de macros
#define PROGRAMA_TITULO "PrograPong PAyED Ingenier\xA1\x61 UNLP"
#define PROGRAMA_LARGO 32
#define DESARROLLADOR_TITULO "Desarrollado por Sim\xA2n e Ignacio"
#define DESARROLLADOR_LARGO 32
#define CANTIDAD_HALL_OF_FAME 5   // Cantidad de mejores puntajes que serán persistidos
#define ESPACIO_NOMBRE_JUGADOR 10 // Cantidad de caracteres máximo que puede tener un jugador en el hall of fame, recordar que cambiar este ancho invalida el archivo de mejores puntajes
#define BARRA_X 2                 // tamaño de la barra en el eje X
#define BARRA_Y_INICIAL 5         // tamaño inicial de la barra
#define BARRA_Y_LARGO 7           // tamaño largo
#define BARRA_Y_CORTO 3           // tamaño corto

#define ITEM_TAM_X 2              // tamaño en eje x de un item
#define ITEM_TAM_Y 2              // tamaño en eje y de un item
#define ITEMS_NUM 3               // cantidad de items

// Para adaptar más el juego al hardware que se utilice para ejecutarlo, quizás sea mejor que el tablero tenga las dimenciones de la consola que se utilice, sin fijarla
// Tamaño del tablero en el caso que se desee fijar
// #define TABLERO_X 50
// #define TABLERO_Y 25
// En su lugar definimos las dimenciones mínmas que debe tener el tablero, caso que no se cumplan, detiene la ejecución mostrando el mensaje de error correspondiente
#define TABLERO_X_MINIMO 80
#define TABLERO_Y_MINIMO 16

// Caracteres graficables
#define BOLA_CHAR 'O'
#define BARRA_CHAR '|'
#define VACIO_CHAR ' '
#define MURO_CHAR '#'

// La siguiente variable no hace falta, a excepción que sirva para no exigir tanto el nucleo del procesador usado para el juego
// #define FRAME_MS 100

// Para obtener mayor legibilidad en el código en lugar de especificar teclas con enumeraciones, dado que se utilizarán en switch, se definen macros
// enum acciones {ARRIBA='w', ABAJO='s', PAUSA='p'};
#define ACCION_ARRIBA 'w'
#define ACCION_ABAJO 's'
#define ACCION_PAUSA 'p'
#define ACCION_QUITAR 'q'

// Se declara con typedef para mayor legibilidad en el código, aunque quizás es mejor definir macros MODO_AVENTURA y MODO_SUPERVIVENCIA
typedef enum {AVENTURA=0, SUPERVIVENCIA=1} Modo;

// Estados que puede tomar el campo direccion_barra_maquina dentro de la estructura Juego
typedef enum {INDEFINIDA=0, HACIA_ARRIBA=1, HACIA_ABAJO=2} DireccionBarra;

// Estructura de la barra de enemigo y jugador
// El dibujo está establecido por definición en el pdf por repetición del caracter BARRA_CHAR, por lo tanto, la matriz dibujo, no es necesaria
// La coordenada y nunca puede ser negativa, por lo que se define con un tipo de datos positivo
// El largo actual solo puede tomar los valores BARRA_Y_INICIAL, BARRA_Y_LARGO y BARRA_Y_CORTO, por lo tanto nunca es negativo
// ticks_mover nunca serán negativos pese a que clock_t los admite
// ticks_mover es la cantidad de ticks que faltan para el próximo movimiento de la Barra de jugador o máquina
// efecto_temporal no hace falta en la implementación actual
// ticks_efecto es la cantidad de ticks en que debe desaparecer la acción del efecto obtenido por impacto de la Bola en un ítem
// Las estructuras que tengan un tamaño impar o inadecuado para optimizaciones de acceso a datos, el compilador les agregará espacio ficticio automáticamente para adecuarlas
typedef struct
{
//	char dibujo[BARRA_X][BARRA_Y_LARGO];
//	int y;                       // posición vertical (la X está fija para cada barra)
	unsigned short y;            // posición vertical (la X está fija para cada barra)
//	int largo_actual;            // largo
	unsigned char largo_actual;  // largo
//	int ticks_mover;             // tiempo restante en frames/ticks para efectuar movimiento
	clock_t ticks_mover;         // cantidad de ticks en el cual la barra debe realizar el siguiente movimiento
//	int efecto_temporal;         // si se desea almacenar el tipo de item
//	int ticks_efecto;            // tiempo restante en frames/ticks del efecto del ítem modificador obtenido
	clock_t ticks_efecto;        // cantidad de ticks en el cual debe desaparecer el efecto del ítem modificador obtenido
} Barra;

// Estructura Items
// El dibujo es redundante, el tipo me indica cuál es su dibujo
// Las coordenadas nunca pueden ser negativas
// El tipo son solo 3, no hace falta que su tipo de datos sea un entero
// Activo es un bit, lo puedo agragar al tipo
// ticks_restantes nunca puede ser negativo, además se redefine por el momento en que debe desaparecer
typedef struct
{
//	char dibujo[ITEM_TAM_X][ITEM_TAM_Y];
//	int x;                         // posición horizontal
//	int y;                         // posición vertical
	unsigned short x;              // posición horizontal
	unsigned short y;              // posición vertical
//	int tipo;                      // 0: alargar-velocidad, 1: acortar+velocidad, 2: bloquear zona
    unsigned char tipo;            // 0: alargar-velocidad, 1: acortar+velocidad, 2: bloquear zona, 128: alargar-velocidad activo, 129: acortar+velocidad activo, 130: bloquear zona activo
//	int activo;                    // si está activo en pantalla
//	int ticks_restantes;           // tiempo activo restante en frames o ticks
	clock_t instante_desaparicion; // momento que el item debe desaparecer por no haber sido alcanzado por la Bola
} Item;

// Estructura Bola
// La variable simbolo no es necesaria, dado que se define la macro BOLA_CHAR
// Las coordenadas de la bola x e y nunca tendrán como valor, un número negativo, por lo que se define un tipo que solo acepta valores positivos
// Se reemplaza dx y dy por el ángulo que lleva la Bola es de tipo unsigned short, por lo que el ángulo tandrá valores desde 0 a 65535.
// Los pimeros 2 bits indican con un poco de imaginación, la esquina de la pantalla, hacia donde se dirige la Bola.
// Entonces int(angulo/16384) devolverá un número de 0 a 3, correspondiente al cuadrante donde se dirige la Bola, 16384 es 2 elevado (a 16 bits - 2).
// Definiendo angulo para que tenga estos valores, me ahorro tener que verificar si ocurre un desbordamiento del ángulo, es decir, el desbordamiento se autoacomoda.
// ticks_mover nunca será negativo y se asume que no tendrá un valor mayor a 65535. Igualmente lo ponemos de tipo clock_t para que quede consecuente.
typedef struct
{
//	char simbolo;
//	int x;                 // posición horizontal
//	int y;                 // posición vertical
	unsigned short x;      // posición horizontal
	unsigned short y;      // posición vertical
	double xFino;          // posición horizontal con decimales
	double yFino;          // posición vertical con decimales
//	int dx;                // movimiento horizontal
//	int dy;                // movimiento vertical
	unsigned short angulo; // angulo que lleva la bola
	clock_t ticks_mover;   // cantidad de ticks en la que se debe mover la Bola
} Bola;

// Estructura para uno de los 5 mejores puntajes
// Así es como se debe definir un tipo de datos cuyo formato es una estructura con diferentes campos
typedef struct
{
	//char nombreJugador[ESPACIO_NOMBRE_JUGADOR];
	char *nombreJugador;  // puntero a 10 caracteres, los nueve correspondientes alo nombre del jugador más 1 del 0 como fin de string
	time_t instante;      // 8 bytes
	unsigned int puntaje; // 4 bytes
} MayorPuntaje;           // Total de la estructura 22 bytes 0x16, por ejemplo:
                          // 00  50 65 64 72 6F 20 20 20 20 00  7A 1F 85 68 00 00 00 00  03 00 00 00 | Pedro    .z..h........
                          // 16  44 69 65 67 6F 20 20 20 20 00  8D 20 85 68 00 00 00 00  03 00 00 00 | Diego    .. .h........
                          // 2C  4A 61 63 69 6E 74 6F 20 20 00  B5 1F 85 68 00 00 00 00  02 00 00 00 | Jacinto  .µ..h........
                          // 42  4D 61 72 74 69 6E 20 20 20 00  20 20 85 68 00 00 00 00  02 00 00 00 | Martin   .  .h........
                          // 58  4C 75 69 73 20 20 20 20 20 00  FD 1C 85 68 00 00 00 00  01 00 00 00 | Luis     .ý..h........

// Estructura que almacena el tablero, ambas barras, la bola, los ítems y otros indicadores respecto al modo de juego
// la pantalla no es necesario mantenerla gracias al empleo de la librería ncurses que hace la experiencia de usuario más grata
// Es una mala práctica especificar los campos de una estructura en plural, el nombre del vector items, se pasa a singular
typedef struct
{
    // No necesariamente el tablero inicia en las coordsenadas de la pantalla (0, 0), por lo tanto se declaran los siguientes 2 miembros:
	unsigned short inicio_tablero_x;
	unsigned short inicio_tablero_y;
	unsigned short anchoTablero;     // Es el ancho del campo de juego, por definición en esta implementación es igual al ancho de la pantalla
	unsigned short altoTablero;      // Es el alto del campo de juego, excluídos la barra de título. ambos muros y la barra de estado

//	char pantalla[TABLERO_Y][TABLERO_X+1];

	Barra jugador;
	Barra maquina;
	DireccionBarra direccion_barra_maquina;
	Bola bola;
//	Item items[ITEMS_NUM];
	Item item[ITEMS_NUM];
	MayorPuntaje hallOfFame[CANTIDAD_HALL_OF_FAME];

	// Informaciones visibles en la parte inferior del tablero del juego
	unsigned int puntos; // puntos obtenidos
	unsigned short vidas;  // vidas restantes
	Modo modo;  // 0: aventura, 1: supervivencia
	unsigned short nivel;  // nivel para modo 0

	//unsigned int ticks_cambio_vel; // contador para cambio de velocidad en modo SUPERVIVENCIA
    clock_t ini_limite_reaccion_maquina;
    clock_t fin_limite_reaccion_maquina;
} Juego;

#endif // JUEGO_H_
