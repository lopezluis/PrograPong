#include <stdlib.h>           // por rand
#include <ncurses.h>          // por mvaddch
#include <time.h>             // por CLOCKS_PER_SEC
#include "juego.h"            // por Juego
#include "reaccion_maquina.h" // por quitar_limite_reaccion_maquina
#include "buffer_teclado.h"   // por vaciar_buffer_teclado
#include "item.h"             // por bola_encima_item

const double deltaAngulo[90] = {1, 0.999847695156391, 0.999390827019096, 0.998629534754574, 0.997564050259824, 0.996194698091746, 0.994521895368273, 0.992546151641322,
	0.99026806874157, 0.987688340595138, 0.984807753012208, 0.981627183447664, 0.978147600733806, 0.974370064785235, 0.970295726275996, 0.965925826289068, 0.961261695938319,
	0.956304755963035, 0.951056516295153, 0.945518575599317, 0.939692620785908, 0.933580426497202, 0.927183854566787, 0.92050485345244, 0.913545457642601, 0.90630778703665,
	0.898794046299167, 0.891006524188368, 0.882947592858927, 0.874619707139396, 0.866025403784439, 0.857167300702112, 0.848048096156426, 0.838670567945424, 0.829037572555042,
	0.819152044288992, 0.809016994374947, 0.798635510047293, 0.788010753606722, 0.777145961456971, 0.766044443118978, 0.754709580222772, 0.743144825477394, 0.73135370161917,
	0.719339800338651, 0.707106781186548, 0.694658370458997, 0.681998360062498, 0.669130606358858, 0.656059028990507, 0.642787609686539, 0.629320391049837, 0.615661475325658,
	0.601815023152048, 0.587785252292473, 0.573576436351046, 0.559192903470747, 0.544639035015027, 0.529919264233205, 0.515038074910054, 0.5, 0.484809620246337, 0.469471562785891,
	0.453990499739547, 0.438371146789077, 0.422618261740699, 0.4067366430758, 0.390731128489274, 0.374606593415912, 0.3583679495453, 0.342020143325669, 0.325568154457157,
	0.309016994374947, 0.292371704722737, 0.275637355816999, 0.258819045102521, 0.241921895599668, 0.224951054343865, 0.207911690817759, 0.190808995376545, 0.17364817766693,
	0.156434465040231, 0.139173100960065, 0.121869343405147, 0.104528463267653, 0.087155742747658, 0.069756473744125, 0.052335956242944, 0.034899496702501, 0.017452406437284};

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

void inicializar_bola(Juego *juego)
{
    // Borro la bola. Tener en cuenta que las coordenadas de la bola están desde (x, y) = (0, 0), por lo tanto dewberán sumársele el lugar donde inicia el tablero
    mvaddch(COORDENADA_Y_PANTALLA_BOLA(juego), COORDENADA_X_PANTALLA_BOLA(juego), VACIO_CHAR);
    // Inicializar la Bola
    juego->bola.x = juego->bola.xFino = juego->anchoTablero / 2;
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
    // Angulo inicial es al azar entre 0 y 359 quitando el cono superior e inferior
    juego->bola.angulo = rand() % 360;
    if(((juego->bola.angulo > 45) && (juego->bola.angulo < 135)) || ((juego->bola.angulo > 225) && (juego->bola.angulo < 315)))
    {
        juego->bola.angulo += 90;
        if(juego->bola.angulo > 359)
        {
            juego->bola.angulo -= 360;
        }
    }
    // TODO La cantidad de ticks en que debe moverse la bola cambiará a lo largo de la partida, para incrementar su velocidad, implementar este comportamiento
	juego->bola.ticks_mover = CLOCKS_PER_SEC / 10;
	// Cuando inicia una bola, la máquina tiene reacción plena
	quitar_limite_reaccion_maquina(juego);
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
	if(juego->bola.angulo < 90)
	{
		juego->bola.xFino += deltaAngulo[juego->bola.angulo];
		juego->bola.yFino -= deltaAngulo[90 - juego->bola.angulo];
        // Verificar si la bola rebota en el muro superior, tener en cuenta que juego->bola.yFino es un double, por lo tanto, puede ser negativo
        if((juego->bola.yFino) < 0)
        {
            juego->bola.angulo = 360 - juego->bola.angulo;
            // La definición del ejercicio solicita: la máquina tendrá un tiempo de reacción aleatorio a cambios de dirección
            // ¿A cambios de diracción de la bola o a los de la barra de la máquina? Para cambio de dirección de la bola, habría que poner un "limitar_reaccion_maquina(juego);" en este punto
            // Asumo por sentido común, que la definición del ejercicio se refiere a cambios de dirección de la barra de la máquina
        }
	}
	if((juego->bola.angulo >= 90) && (juego->bola.angulo < 180))
	{
		juego->bola.xFino -= deltaAngulo[90 - juego->bola.angulo + 90];
		juego->bola.yFino -= deltaAngulo[juego->bola.angulo - 90];
        // Verificar si la bola rebota en pared superior, tener en cuenta que juego->bola.yFino es un double, por lo tanto, puede ser negativo
        if((juego->bola.yFino) < 0)
        {
            juego->bola.angulo = 360 - juego->bola.angulo;
        }
	}
	if((juego->bola.angulo >= 180) && (juego->bola.angulo < 270))
	{
		juego->bola.xFino -= deltaAngulo[juego->bola.angulo - 180];
		juego->bola.yFino += deltaAngulo[90 - juego->bola.angulo + 180];
        // Verificar si la bola rebota en pared inferior, tener en cuenta que juego->bola.yFino es un double, por lo tanto, casteo altoTablero para comparar double con double
        if((juego->bola.yFino) > (double)(juego->altoTablero - 1))
        {
            juego->bola.angulo = 360 - juego->bola.angulo;
        }
	}
	if(juego->bola.angulo >= 270)
	{
        juego->bola.xFino += deltaAngulo[90 - juego->bola.angulo + 270];
        juego->bola.yFino += deltaAngulo[juego->bola.angulo - 270];
        // Verificar rebote en pared inferior, tener en cuenta que juego->bola.yFino es un double, por lo tanto, casteo altoTablero para comparar double con double
        if((juego->bola.yFino) > (double)(juego->altoTablero - 1))
        {
            juego->bola.angulo = 360 - juego->bola.angulo;
        }
    }
    juego->bola.x = (unsigned short)(juego->bola.xFino);
    juego->bola.y = (unsigned short)(juego->bola.yFino);
	// Verificar si la Bola rebota con la máquina
    if ((juego->bola.x >= (juego->anchoTablero - 2 - BARRA_X)) && (juego->bola.y >= juego->maquina.y) && (juego->bola.y <= (juego->maquina.y + juego->maquina.largo_actual)))
    {
        // ¿La Bola se dirige hacia la esquina superior derecha?
        if (juego->bola.angulo <= 90)
        {
            // Cambiar para que se dirija a la esquina superior izquierda y agregar un poco de azar al golpe
            juego->bola.angulo = (90 - juego->bola.angulo + 90) + (rand() % 41) - 20;
            // Si el ángulo queda muy vertical, acomodar
            if(juego->bola.angulo < 135)
            {
                juego->bola.angulo = 135;
            }
        }
        else
        {
            // Cambiar para que se dirija a la esquina inferior izquierda y agregar un poco de azar al golpe
            juego->bola.angulo = (90 - juego->bola.angulo + 450) + (rand() % 41) - 20;
            // Si el ángulo queda muy vertical, acomodar
            if(juego->bola.angulo > 225)
            {
                juego->bola.angulo = 225;
            }
        }
    }
	// Verificar si la Bola rebota con el jugador
    if ((juego->bola.x <= (1 + BARRA_X)) && (juego->bola.y >= juego->jugador.y) && (juego->bola.y <= (juego->jugador.y + juego->jugador.largo_actual)))
    {
        // Corregir BUG aqui, juego->bola.y se iguala a 0 al rabotar en el jugador y no advierto dónde
        // ¿La Bola se dirige hacia la esquina superior izquierda?
        if (juego->bola.angulo <= 180)
        {
            // Cambiar para que se dirija a la esquina superior derecha
            juego->bola.angulo = (180 - juego->bola.angulo) + (rand() % 41) - 20;
            // Si el ángulo queda muy vertical, acomodar
            if(juego->bola.angulo > 45)
            {
                juego->bola.angulo = 45;
            }
        }
        else
        {
            // Cambiar para que se dirija a la esquina inferior derecha
            juego->bola.angulo = (270 - juego->bola.angulo + 270) + (rand() % 41) - 20;
            // Si el ángulo queda muy vertical, acomodar
            if(juego->bola.angulo < 315)
            {
                juego->bola.angulo = 315;
            }
        }
    }
    mvaddch(COORDENADA_Y_PANTALLA_BOLA(juego), COORDENADA_X_PANTALLA_BOLA(juego), BOLA_CHAR);
}
