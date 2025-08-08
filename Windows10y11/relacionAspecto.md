# Relación de aspecto

La anchura de la resolución es \(1920\) píxeles.

La altura de la resolución es \(1080\) píxeles.

## Cómo resolver

Para calcular la relación de aspecto, divide la anchura por la altura y simplifica la fracción. La relación inicial es \(\frac{1920}{1080}\). Simplificar la fracción. Dividir ambos números por su máximo común divisor. El máximo común divisor de \(1920\) y \(1080\) es \(120\).

\(\frac{1920}{120}=16\).

\(\frac{1080}{120}=9\).

La fracción simplificada es \(\frac{16}{9}\).

La relación de aspecto de \(1920\times 1080\) es \(16:9\).

Una relación de aspecto predeterminada de 16:9, se refiere a las siguientes resoluciones:

	* 4320p (8K): 7680 × 4320
	* 2160p (4K): 3840 × 2160
	* 1440p (2K): 2560 × 1440
	* 1080p (HD): 1920 × 1080
	*  720p (HD): 1280 ×  720
	*  480p (SD):  854 ×  480
	*  360p (SD):  640 ×  360
	*  240p (SD):  426 ×  240

La letra p corresponde a píxeles. En los casos de 480p y 240p se presentan casos particulares, los anchos deben ser el número par más cercano, para 480p y un espacio de color para 6 bits por color, es de 1 megabyte con un desperdicio de 126256 bytes, y en el caso de 240p y un espacio de color de 27 bits por color, también es de 1 megabyte con un desperdicio de 13396 bytes.

## Ejemplo

Si al ancho lo dividimos por el primer valor de la relación de aspecto, luego lo multiplicamos por el segundo valor de la relación de aspecto:

	/usr/bin/echo -en "scale=10; 1920/16*9\n" | /usr/bin/bc

Obtenemos al alto:

	1080

De la misma manera, si el alto en píxeles lo multiplicamos por el primer valor de la relación de aspecto, luego lo dividimos por el segundo valor de la relación de aspecto:

	/usr/bin/echo -en "scale=10; 1080*16/9\n" | /usr/bin/bc

Obtenemos el ancho:

	1920
