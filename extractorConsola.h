//
// Created by svalp on 9/22/2022.
//

#ifndef UNTITLED_EXTRACTORCONSOLA_H
#define UNTITLED_EXTRACTORCONSOLA_H

// Clase que se encarga de extraer los datos de la consola
// Ejemplo entrada por consola cliente $ cliente -i ID -p pipeNom
// Ejemplo entrada por consola gestor $ gestor -n Num -r Relaciones -m modo -t time -p pipeNom

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct argumentos {
    int id; ///< Identificador del cliente.
    char *pipeNom; ///< Nombre del pipe nominal.
    int num; ///< Es el número máximo de usuarios que se pueden registrar en el sistema. No todos estarán conectados al mismo tiempo. Es un número entero <= 100.
    char* relaciones; ///< Es un archivo de texto que describe las relaciones actuales entre los usuarios del sistema. Las relaciones se describirán en un archivo de texto con una matriz de NumxNum. En cada línea del archivo se almacenará información sobre las relaciones de un usuario en particular. Dichas relaciones se expresan con números binarios, es decir, un cero (0) en una determinada posición significa que no se sigue a un determinado usuario y un uno (1) significa que sí sé es seguidor. (Estaestructura corresponde a la de una matriz de adyacencia de un grafo dirigido)

    // Ejemplo de archivo de texto de N = 10
    // 1 0 1 1 0 0 0 0 0 0 0
    // 2 1 0 0 0 0 0 0 0 0 0
    // 3 1 0 0 0 1 1 1 0 0 0
    // 4 1 0 0 0 0 0 0 0 0 0
    // 5 0 0 0 0 0 0 0 0 0 0
    // 6 0 0 0 0 0 0 1 0 0 0
    // 7 1 0 1 0 0 0 0 0 0 0
    // 8 0 0 0 0 0 0 0 0 0 0
    // 9 0 0 0 0 0 0 0 0 0 0
    // 10 0 0 0 0 0 0 0 0 0 0
    // La primera columna no es parte del archivo, se coloca para dejar claro que la primera fila
    // corresponde a las relaciones del usuario 1, la segunda al usuario 2, etc. Este y otros archivos de
    // ejemplo se colocarán en BS en la misma carpeta donde se coloque el proyecto.
    // La gráfica de la figura 2 ilustra las relaciones de la matriz de la figura 1. Una flecha del nodo
    // i al nodo j, indica que i es seguidor de j. Por ejemplo, en la gráfica, el usuario 4 es seguidor del
    // usuario 1 (observar que hay un 1 en fila 4, primera columna). El usuario 2 sigue al usuario 1, y el
    // usuario 1 sigue al usuario 2 y al usuario 3.

    char modo; ///< indica si el modo en el que trabajará el Gestor es acoplado o desacoplado. Se usará el carácter A para el modo acoplado y el carácter D para el desacoplado (Ambos es mayúscula o minúscula)
    int time; ///< Es un valor en segundos, que utilizará el Gestor para imprimir la siguiente información de forma periódica: Número de usuarios conectados, número total de tweets enviados y recibidos. Por ejemplo, si time = 2, cada 2 segundos el proceso Gestor imprimirá la información mencionada.
};

/// @brief Función que se encarga de extraer los datos de la consola
/// @param argv Argumentos
/// @param argc Cantidad de argumentos
/// @return Estructura con los datos extraidos
struct argumentos extraerDatosConsola(int argc, char *argv[])
{
    struct argumentos argumentos; ///< Estructura con los datos extraidos
    argumentos.id = 0; // Inicializar id en 0
    argumentos.pipeNom = NULL; // Inicializar pipeNom en NULL
    argumentos.num = 0; // Inicializar num en 0
    argumentos.relaciones = NULL; // Inicializar relaciones en NULL
    argumentos.modo = ' '; // Inicializar modo en ' '
    argumentos.time = 0; // Inicializar time en 0

    int i; // Variable para iterar
    for (i = 1; i < argc; i++) { // Iterar por los argumentos
        if (strcmp(argv[i], "-i") == 0) { // Si el argumento es -i
            argumentos.id = atoi(argv[i + 1]); // Extraer el id
        } else if (strcmp(argv[i], "-p") == 0) { // Si el argumento es -p
            argumentos.pipeNom = argv[i + 1]; // Extraer el pipeNom
        } else if (strcmp(argv[i], "-n") == 0) { // Si el argumento es -n
            argumentos.num = atoi(argv[i + 1]); // Extraer el num
        } else if (strcmp(argv[i], "-r") == 0) { // Si el argumento es -r
            argumentos.relaciones = argv[i + 1]; // Extraer el relaciones
        } else if (strcmp(argv[i], "-m") == 0) { // Si el argumento es -m
            argumentos.modo = argv[i + 1][0]; // Extraer el modo
        } else if (strcmp(argv[i], "-t") == 0) { // Si el argumento es -t
            argumentos.time = atoi(argv[i + 1]); // Extraer el time
        }
    }

    return argumentos; // Retornar los argumentos
}



#endif //UNTITLED_EXTRACTORCONSOLA_H
