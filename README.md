# PrograPong

## Introducción

El desarrollo presente en este directorio corresponde al antiguo juego electrónico de las casas de videojuegos muy populares en las peatonales, llamado Pong. Este es el desarrollo en modo texto del popular videojuego.

Desarrollado por mi anteriormente, en modo gráfico:

	find . -depth -type f | grep -E "[Pp]ong"

Resultados:

	SFML/examples/pong/CMakeLists.txt
	SFML/examples/pong/Pong.cpp
	SFML/examples/pong/resources/ball.wav
	SFML/examples/pong/resources/sansation.ttf
	SFML/examples/pong/Makefile
	SFML/examples/pong/Pong
	SDL2/pong/main.cpp
	SDL2/pong/main.h
	SDL2/pong/Makefile
	SDL2/pong/pelota.cpp
	SDL2/pong/pelota.h
	SDL2/pong/pelota.png
	SDL2/pong/tablero.cpp
	SDL2/pong/tablero.h
	SDL2/pong/entidad.cpp
	SDL2/pong/entidad.h
	SDL2/pong/juego.cpp
	SDL2/pong/juego.h
	SDL2/pong/paleta.cpp
	SDL2/pong/paleta.png
	SDL2/pong/paleta.h
	SDL2/pong/lado.png

Lo he desarrollado tanto usando SFML, como SDL2, con éste último logré la versión más acabada o mejor, puede ejecutarse a 40k cuadros por segundo, extremádamente exagerado, teniendo en cuenta que los monitores convencionales refrescan a 60 cuadros por segundo.

## Desarrollo

La primera definición es la relación de aspecto del tablero. La estableceremos arbitrariamente en 16:9, ver esta toma de decisión en el documento [Relación de aspecto](relacionAspecto.html).

Para poder obtener la cantidad de columnas y filas de la terminal en Debian, podemos utilizar el comando:

	/usr/bin/tput clear # Limpia la pantalla
	/usr/bin/tput cols # Responderá 270 en el panel terminal de Dolphin y 271 en la ventana Konsole
	/usr/bin/tput lines # Responderá 17 en el panel terminal de Dolphin y 67 en la ventana Konsole

### Para depurar

Para visualizar valores de variables, puedo utilizar el siguinte código:

    // TODO: borrar visualizaciones para depuración
	{
        // Limpia la pantalla
        //clear();
        // Reservar memoria para la cadena
        char *cadena = (char *)malloc(200);
        // Verificación de que malloc tuvo éxito
        if (cadena == NULL)
        {
            if (fputs("Error: No hay suficiente memoria para almacenar el string para depuración.\n", stderr) == EOF)
            {
                // La función perror mostrará el mensaje del parámetro ingresado, seguido de dos puntos, espacio, mensaje de error del sistema y salto de línea
                perror("Error interno");
            }
            // Regresar al sistema operativo con error
            exit(EXIT_FAILURE);
        }
        mvaddstr(juego->altoTablero - 2, juego->anchoTablero - 80, "                                                             ");
        sprintf(cadena, "[%hu, %hu, %hu].", juego->bola.x, juego->bola.y, juego->bola.angulo);
        mvaddstr(juego->altoTablero - 2, juego->anchoTablero - 80, cadena);
        // Actualizar la pantalla
        refresh();
        // Esperar a que el usuario presione una tecla
        while(getch() == ERR);
        // Liberar la memoria
        free(cadena);
	}

No olvidar incluir los siguientes encabezaqdos:

    #include <ncurses.h> // TODO: quitar esta inclusión que solo es para depurar por clear, mvaddstr, refresh, getch
    #include <stdlib.h>  // TODO: quitar esta inclusión que solo es para depurar por exit, EXIT_SUCCESS, EXIT_FAILURE, malloc (recordar que malloc.h está deprecado)

Otros ejemplos:

    // TODO: borrar visualizaciones para depuración
	{
        // Reservar memoria para la cadena
        char *cadena = (char *)malloc(200);
        // Verificación de que malloc tuvo éxito
        if (cadena == NULL)
        {
            if (fputs("Error: No hay suficiente memoria para almacenar el string para depuración.\n", stderr) == EOF)
            {
                // La función perror mostrará el mensaje del parámetro ingresado, seguido de dos puntos, espacio, mensaje de error del sistema y salto de línea
                perror("Error interno");
            }
            // Regresar al sistema operativo con error
            exit(EXIT_FAILURE);
        }
        mvaddstr(juego->altoTablero - 2, juego->anchoTablero - 80, "                                                             ");
        sprintf(cadena, "[%hu, %hu, %hu].", juego->bola.x, juego->bola.y, juego->bola.angulo);
        mvaddstr(juego->altoTablero - 2, juego->anchoTablero - 80, cadena);
        // Actualizar la pantalla
        refresh();
        // Liberar la memoria
        free(cadena);
	}

    // TODO: borrar visualizaciones para depuración
	{
        // Limpia la pantalla
        //clear();
        // Reservar memoria para la cadena
        char *cadena = (char *)malloc(200);
        // Verificación de que malloc tuvo éxito
        if (cadena == NULL)
        {
            if (fputs("Error: No hay suficiente memoria para almacenar el string para depuración.\n", stderr) == EOF)
            {
                // La función perror mostrará el mensaje del parámetro ingresado, seguido de dos puntos, espacio, mensaje de error del sistema y salto de línea
                perror("Error interno");
            }
            // Regresar al sistema operativo con error
            exit(EXIT_FAILURE);
        }
        mvaddstr(juego->altoTablero - 1, juego->anchoTablero - 80, "                                                             ");
        sprintf(cadena, "[%hu, %hu], [%hu, %hu, %hu].", juego->anchoTablero, juego->altoTablero, juego->bola.x, juego->bola.y, juego->bola.angulo);
        mvaddstr(juego->altoTablero - 1, juego->anchoTablero - 80, cadena);
        // Actualizar la pantalla
        refresh();
        // Esperar a que el usuario presione una tecla
        while(getch() == ERR);
        // Liberar la memoria
        free(cadena);
	}

### Puntos a resolver

El primer detalle es que el lenguaje ansi c no dispone de una función que se le pueda pasar como parámetro un lapso y una callback, para que la función llame al callback en intervalos con el lapso establecido. Para una solución posible respecto a esto ver el código en **Projects/c/pthread** y **Projects/c/callback**.

Tablero: tendrá ancho y alto. La mímina expresión para pensar el tablero será el cuadrante del jugador y la computadora. La relación de aspecto de los cuadrantes será igual tanto para el jugador, como la computadora.

Trayectoria de objetos en el tablero de juego. Por ejemplo la Bola sobre el campo. Tendrá 2 variables enteras sin signo, que será su velocidad en metros por segundo y su ángulo en grados de 0 a 359 inclusive ambos extremos. Al rebotar en la pared se presentan 4 alternativas, según el valor en grados del cuadrantre del ángulo que lleva la Bola:

	* < 90, deberá sumársele 270 grados. Significa que la Bola va de izquierda a derecha y está rebotando en la pared superior.
	* > 90 y < 180, deberá sumarsele 90 grados. Significa que la Bola va de derecha a izquierda y está rebotando en la pared superior.
	* > 180 y < 270, deberá restarsele 90 grados. Significa que la Bola va de derecha a izquierda y está rebotando en la pared inferior.
	* > 270, deberá restarsele 90 grados. Significa que la Bola va de derecha a izquierda y está rebotando en la pared inferior.

Ángulo: Primero pensé, para definir el ángulo de la Bola, la relación entre los miembros dx y dy de la estructura Bola. Es incorrecto dado que si ambos miembros valieran 0, se generaría una inconsistencia. Significaría pelota detenida y en el juego pong, nunca puede estar la pelota detenida. Por ejemplo, si el par ordenado (dx,dy) es (1,1) significa 45 grados.

## Preguntas

¿Es necesario hacerlo en modo texto? Naturalmente es mejor escribir este desarrollo en modo gráfico, este proyecto expresa que igualmente se puede lograr desarrollar un juego en modo texto y que resulte entretenido.

SFML (Simple and Fast Multimedia Library o biblioteca multimedia sencilla y rápida) está escrito en C++, se destaca por su orientación a objetos y facilidad de uso, mientras que SDL2 (Simple DirectMedia Layer versión 2 o capa directa de medios simple versión 2) está escrito en C, ofrece un mayor control a bajo nivel y una amplia gama de funciones multimedia. La ventaja de utilizar ANSI C es el absoluto control del hardware, motivo por el cual existe una amplia gama de microcontroladores en el mercado, que se pueden programar en lenguaje C.

### SFML

Ventajas:

Fácil de usar: SFML es conocido por su sintaxis clara y concisa, lo que facilita el aprendizaje y la creación rápida de proyectos, está diseñado para ser intuitivo, con una API más abstracta que facilita el inicio y el desarrollo rápido de aplicaciones.

Orientado a objetos: Su diseño en C++ permite una estructura de código más organizada y modular, ideal para proyectos grandes.

Rendimiento: SFML está optimizado para gráficos 2D y ofrece un buen rendimiento.

SFML tiene una curva de aprendizaje más sencilla; se suele sugerir empezar a aprender de él. No se trata de cuál es mejor, ya que ambos son APIs gráficas/de E/S de bajo nivel para un lenguaje de nivel igualmente inferior. El framework XNA 4.0 en VS o Unity en C#.

Dispone de amplias funciones, incluye compatibilidad integrada con gráficos, audio y redes, lo que simplifica el desarrollo.

Tiene buena documentación, cuenta con documentación y tutoriales bien organizados, lo que lo hace accesible para principiantes.

Desventajas:

Limitaciones en 3D, se centra principalmente en gráficos 2D, por lo que para 3D necesitarás directamente a OpenGL.

Menos control a bajo nivel, en comparación con SDL2, SFML ofrece menos control directo sobre el hardware y las funcionalidades del sistema.

No es compatible con DirectX, solo con OpenGL.

### SDL2

Ventajas:

Control a bajo nivel: SDL2 proporciona acceso directo a funcionalidades del sistema y hardware, lo que permite una mayor flexibilidad en el desarrollo.

Amplia gama de funciones multimedia: SDL2 ofrece soporte para audio, entrada, gráficos (incluyendo OpenGL), y más.

Multiplataforma: SDL2 es compatible con una amplia gama de sistemas operativos, incluyendo Windows, macOS, Linux, Android e iOS.

Desventajas:

Curva de aprendizaje: SDL2, al ser un wrapper de C, puede resultar menos intuitivo para desarrolladores que prefieren un enfoque orientado a objetos en C++.

Más código: La naturaleza de bajo nivel de SDL2 a menudo requiere más código para lograr las mismas tareas que con SFML.

Casi no hay documentación en libros físicos y es favorito de los desarrolladores AAA de la industria durante bastante tiempo.

### Modo texto

Ventajas:

Permite absoluta libertad, lo cual indica que si no sos autodiciplinado al extremo, puede que obtengas un código muy alejado al ideal.

Puede ser ejecutado en Moodle, ya que no admite el modo gráfico.

Desventajas:

Codificar un juego es trabajoso. Tomarse todo el trabajo para que quede feo, no se justifica.

### ¿Cuál elegir?

La elección entre SFML y SDL2 depende de necesidades y preferencias:

Si sos nuevo en desarrollo de juegos y preferís una biblioteca fácil de aprender y usar, con un enfoque orientado a objetos en C++, SFML es una excelente opción.

Si necesitas un mayor control sobre el hardware y funciones multimedia, y estás dispuesto a lidiar con un poco más de complejidad, SDL2 es mejor opción.

Ambos entornos son herramientas poderosas que pueden ayudarte a crear juegos y aplicaciones multimedia. Sería bueno probar ambas para ver cuál se adapta mejor a tu estilo de programación y a los requisitos de tu proyecto.

## Subir a GitHub

Asegurarse que existe el archivo **README.md** y que en la primera línea contiene **# PrograPong**, es decir, el nombre del repositorio de GitHub como título principal del documento markdown. Luego ejecutar los suguientes comandos, no sé cual **git remote** es el correcto:

    git init
    git add README.md
    git add .
    git commit -m "Primer commit"
    git branch -M master
    git remote set-url origin https://github.com/lopezluis/PrograPong.git
    git remote set-url origin lopezluis@github.com:lopezluis/PrograPong.git
    git remote set-url origin git@github.com:lopezluis/PrograPong.git
    git push -u origin master
