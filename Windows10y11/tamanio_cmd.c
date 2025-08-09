#include <windows.h> // por HANDLE, GetStdHandle, STD_OUTPUT_HANDLE, CONSOLE_SCREEN_BUFFER_INFO, GetConsoleScreenBufferInfo

void tamanio_cmd(unsigned short *ancho, unsigned short *alto)
{
    // Obtener el identificador del búfer de la consola
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Estructura para almacenar la información de la consola
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

    // Obtener la información de la consola
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);

    // Ancho y alto de la consola en caracteres
    *ancho = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
    *alto = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
}
