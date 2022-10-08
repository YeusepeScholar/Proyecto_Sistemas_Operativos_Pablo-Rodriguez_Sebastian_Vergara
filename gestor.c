// Proyecto entreaga 1: Sistemas Operativos
// Sebastian Vergara
// TODO: Inicialización del proceso Gestor con los parámetros correspondientes.
//  El gestor crea las estructuras de datos necesarias y muestra los usuarios registrados.

// PROGRAMA DEL PROCESO GESTOR

// El Gestor es el primer programa que se debe invocar. Éste crea e inicializa las estructuras de datos
// que permiten almacenar a los usuarios y sus relaciones. También crea el primer pipe nominal por
// donde se recibirá la información y solicitudes de los usuarios que se van conectando (Ver figura 3,
// Izquierda). El Gestor nunca deja de funcionar a menos que se elimine de forma explícita desde el
// terminal.

// El Gestor tiene dos tareas principales:
// a) Atender las solicitudes de los procesos Cliente: registro de usuarios, follow, unfollow, enviar un tweet y desconexión y
// b) Imprimir estadísticas

// Es el proceso encargado de gestionar el envío de tweets entre usuarios y, por lo tanto, de almacenar
// los datos correspondientes; también almacena las relaciones existentes entre ellos. El Gestor va a
// tener dos modos de operación: acoplado y desacoplado. En el modo desacoplado, los usuarios
// reciben los tweets que les han enviado, únicamente al conectarse al sistema y cuando
// explícitamente solicitan una actualización de sus tweets. En el modo acoplado,se reciben los tweets
// luego del momento de conexión y en el mismo instante en el que son enviados por otros usuarios
// (a quienes ellos siguen), es decir, en este modo, si un usuario A envía un tweet en un momento
// determinado, sus seguidores conectados deben recibirlo inmediatamente.

// Como compilar: gcc -o Gestor gestor.c -lpthread
// Como ejecutar: ./Gestor -n 10 -m 10 -r relaciones.txt -t 10 -p pipeNom

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdbool.h>
#include "extractorConsola.h" // Funciones para extraer datos de la consola
#include "fileParser.h" // Funciones para extraer datos de un archivo
#include "cliente.h" // Estructura para almacenar los datos de un cliente

// Registro: Al momento de conectarse un usuario, el proceso Cliente enviará el identificador
// del usuario al Gestor usando el pipe. El Gestor le devolverá al Cliente el resultado de la
// operación (si fue exitosa o no), lo cual dependerá fundamentalmente de si un usuario con
// el mismo identificador ya se encuentra conectado en el sistema o si el número esta fuera
// del rango especificado por Num. También, si es el caso, le enviará los tweets que tiene
// almacenado para ese usuario. Si Ud. desea crear otro pipe para la comunicación exclusiva
// entre el Gestor y el Cliente (Figura 3, derecha pipes rojos), puede aprovechar la operación
// de registro para crear e intercambiar la información para este nuevo pipe.

struct argumentos args; ///< Estructura para almacenar los argumentos de la consola
struct cliente *clientes; ///< Estructura para almacenar los datos de los clientes

/// @brief Funcion para imprimir estadisticas
void *imprimirEstadisticas(void *arg){
    printf("Imprimiendo estadisticas\n");
    return NULL;
}

/// @brief Funcion para atención de solicitudes de los procesos Cliente
void *atenderSolicitudes(void *arg){
    printf("Atendiendo solicitudes\n");
    // Abrir el pipe como lectura
    int fd = open(args.pipeNom, O_RDONLY); // Abrir el pipe como lectura
    // testear si se abrio correctamente
    if (fd == -1){
        perror("Error al abrir el pipe");
        exit(EXIT_FAILURE);
    }
    // Imprimir el pipe
    printf("Pipe: %s\n", args.pipeNom);
    // Leer el pipe
    while (true) {
        char buffer[100];
        int n = read(fd, buffer, 100);
        if (n == -1) {
            perror("Error al leer el pipe");
            exit(EXIT_FAILURE);
        }
        if(n == 0){
            printf("El pipe se cerro");
            break;
        }
        // Si el mensaje tiene id (Le entra con formato ID%d)
        if (buffer[0] == 'I' && buffer[1] == 'D') {
            // Extraer el id
            int id = atoi(&buffer[2]);
            printf("ID: %d", id);
        }
    }
}

/// @brief Funcion para cargar las relaciones de un archivo
void *cargarRelaciones(void *arg){

    struct fileParser fp = extraerDatosArchivo(args.relaciones);
    printf("Cargando relaciones\n");
    for(int i = 0; i < fp.num; i++) {
        for (int j = 0; j < fp.num; j++) {
            printf("%d ", fp.relaciones[i][j]);
        }
    }
    return NULL;
}

/// @brief Funcion principal del proceso Gestor
int main(int argc, char *argv[]) {
    // Inicializar el gestor

    // Extraer los datos de la consola¿
    args = extraerDatosConsola(argc, argv);
    // Borrar el contenido basura que pueda haber en el pipe
    remove(args.pipeNom);
    // Crear el primer pipe nominal de nombre args.pipeNom y abrirlo en modo escritura. Se usara para asignar un identificador y un pipe a cada usuario que se conecte al sistema.
    unlink(args.pipeNom); // Eliminar el pipe si existe
    mkfifo(args.pipeNom, 0666);

    cargarRelaciones(NULL);

    // Crear el hilo para atender solicitudes
    pthread_t hiloSolicitudes;
    pthread_create(&hiloSolicitudes, NULL, atenderSolicitudes, NULL);

    // Crear el hilo para imprimir estadisticas
    pthread_t hiloEstadisticas;
    pthread_create(&hiloEstadisticas, NULL, imprimirEstadisticas, NULL);

    // Esperar a que los hilos terminen
    pthread_join(hiloSolicitudes, NULL);
    pthread_join(hiloEstadisticas, NULL);

    return 0;
}




