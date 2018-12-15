li {list-style-type: none;}
El esquema de distribución a escoger es el estático mediante bloques.
Las ventajas que tiene esta distribución es que es fácil de implementar con las funciones de MPI, además está distribuida de manera homogénea sobre una estructura de datos como lo es el arreglo.

El proceso 0 hará el análisis de argumentos, el listado de archivos, administrará la estructura de datos con las parejas de archivos a comparar,integrará los resultados y ordenará y presentará los resultados al usuario. 
Cada proceso que sea diferente del cero tendrá un inicio y un final y ahí calcularán la distancia respectiva 

Pseudocódigo:

//Inicializar MPI
// Obtener el rank y el process count
// Si soy el 0
* // analizar los argumentos
* // listar los archivos (cola)
* // crear la estructura de datos con la pareja de archivos
* // mandar la estructura a los demás procesos para que puedan escribir
* // recibir el arreglo de cada uno de los procesos 
* // ordenar el arreglo
* // imprimir la salida correspondiente

// De lo contrario
* // Calcular mi rango
* // Recibir la estructura de datos
* // Calcular la distancia de levenshtein con respecto a la pareja de archivos y mi rango
* // Guardar la distancia en el arreglo.
* // Volver a mandar la estructura de datos al proceso 0

// MPI_Finalize()