
El esquema de distribución a escoger es el estático mediante bloques.
Las ventajas que tiene esta distribución es que es fácil de implementar con las funciones de MPI, además está distribuida de manera homogénea sobre una estructura de datos como lo es el arreglo.

Todos los procesos tendrán análisis de argumentos, el listado de archivos, administrarán la estructura de datos con las parejas de archivos a comparar. El proceso 0 integrará los resultados y ordenará y presentará los resultados al usuario. 
Cada proceso divide su trabajo y calcula las distancias respectivas en su rango.
Pseudocódigo:

//Inicializar MPI
// Obtener el rank y el process count
// Analizar los argumentos
// Listar los archivos (cola)
// Crear la estructura de datos con la pareja de archivos
// Calcular mi rango
// Calcular la distancia de levenshtein con respecto a la pareja de archivos y mi rango
// Guardar la distancia en el arreglo de distancias.
// Mandar las distancias al proceso 0 con su respectivo rango.
// Si soy el 0
* // recibir el arreglo de cada uno de los procesos y su respectivo rango.
* // Poner las distancias en el arreglo de estructuras
* // ordenar el arreglo
* // imprimir la salida correspondiente


// MPI_Finalize()