// Proyecto entreaga 1: Sistemas Operativos
// Sebastian Vergara y Pablo Rodriguez

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
int pipeNom; ///< Pipe nominal para lo que le entra al gestor

// cada time segundos el Gestor debe imprimir la siguiente información: Número de
// usuarios conectados, número total de tweets enviados y recibidos.
int usuariosConectados = 0; ///< Numero de usuarios conectados
int tweetsEnviados = 0; ///< Numero de tweets enviados
int tweetsRecibidos = 0; ///< Numero de tweets recibidos
/// @brief Funcion para imprimir estadisticas
void *imprimirEstadisticas(void *arg){
    while(true){ // Imprimir estadisticas cada time segundos
        sleep(args.time); // Esperar time segundos
        printf("Imprimiendo estadisticas\n"); // Imprimir estadisticas
        printf("Usuarios conectados: %d\n", usuariosConectados); // Imprimir usuarios conectados
        printf("Tweets enviados: %d\n", tweetsEnviados); // Imprimir tweets enviados
        printf("Tweets recibidos: %d\n", tweetsRecibidos); // Imprimir tweets recibidos
    }
}



/// @brief Funcion para cargar las relaciones de un archivo
void *cargarRelaciones(void *arg){
    // Cargar relaciones de un archivo
    printf("Cargando relaciones\n");
    FILE *fp; ///< Archivo de relaciones
    char *line = NULL; ///< Linea del archivo
    size_t len = 0; ///< Tamaño de la linea
    ssize_t read; ///< Tamaño de la linea leida

    fp = fopen(args.relaciones, "r"); // Abrir archivo de relaciones
    if (fp == NULL){ // Si no se pudo abrir el archivo
        printf("Error al abrir el archivo de relaciones %s \n", args.relaciones);
    }
    int i = 0; // Contador de lineas
    while ((read = getline(&line, &len, fp)) != -1) { // Leer linea por linea
        // Extraer datos de la linea, el formato es: usuario1 usuario2 usuario3 ...
        // En la linea se encuentran los usuarios a los que sigue el usuario1, representado por las columnas
        // Si lo sigue es 1, si no lo sigue es 0
        // Ejemplo: 1 0 1 0 1 0 1 0 1 0
        // El usuario1 sigue al usuario2, usuario4, usuario6, usuario8 y usuario10
        char *token; ///< Token de la linea
        token = strtok(line, " "); // Extraer el primer token
        int j = 0; ///< Contador de columnas
        while(token != NULL){ // Mientras haya tokens
            if (atoi(token) == 1){ // Si el token es 1
                clientes[i].suscripciones[j] = 1; // El usuario i sigue al usuario j
                clientes[i].numSuscripciones++; // El usuario i tiene una suscripcion mas
            }
            else{ // Si el token es 0
                clientes[i].suscripciones[j] = 0; // El usuario i no sigue al usuario j
            }
            token = strtok(NULL, " "); // Extraer el siguiente token

            j++; // Incrementar contador de columnas
        }
        i++; // Incrementar contador de lineas
    }

    return NULL; // Retornar
}

/// @brief Funcion para atención de solicitudes de los procesos Cliente
void *atenderSolicitudes(void *arg){ // Atender solicitudes de los procesos Cliente
    printf("Atendiendo solicitudes\n");

    // Abrir el pipe como lectura
    pipeNom = open(args.pipeNom, O_RDONLY); // Abrir el pipe como lectura
    if (pipeNom == -1) { // Si no se pudo abrir el pipe
        perror("Error al abrir el pipe");
        exit(EXIT_FAILURE); // Salir del programa
    }

    // Imprimir el pipe
    printf("Pipe: %s\n", args.pipeNom);

    // Leer el pipe

    while (true) { // Mientras se pueda leer del pipe
        char buffer[100]; ///< Buffer para almacenar lo que se lee del pipe
        int n = read(pipeNom, buffer, 100); ///< Numero de bytes leidos
        if (n == -1) { // Si no se pudo leer del pipe
            perror("Error al leer el pipe");
            exit(EXIT_FAILURE);
        }
        // Si el mensaje tiene id (Le entra con formato ID%d)
        if (buffer[0] == 'I' && buffer[1] == 'D') { // Si el mensaje tiene id (Le entra con formato ID%d)
            // Extraer el id
            int id = atoi(&buffer[2]);
            // Si el id esta en el rango
            if (id >= 0 && id < args.num) {
                // Si el id no esta conectado
                if (clientes[id].conectado == false) {
                    // Conectar el id
                    clientes[id].conectado = true;
                    // Incrementar el numero de usuarios conectados
                    usuariosConectados++;
                    printf("Usuario %d conectado\n", id);
                }
            }
        }
        // Borra el buffer
    }
}


/// @brief Funcion principal del proceso Gestor
int main(int argc, char *argv[]) {
    // Inicializar el gestor

    // Extraer los datos de la consola¿
    args = extraerDatosConsola(argc, argv);

    // Todos los clientes tienen id -1
    clientes = malloc(sizeof(struct cliente) * args.num);
    for (int i = 0; i < args.num; ++i) { // Para cada cliente
        clientes[i].id = -1; // Inicializar el id en -1
        clientes[i].conectado = false; // Inicializar conectado en false
        clientes[i].suscripciones = malloc(sizeof(int) * args.num); // Inicializar suscripciones
    }

    // Borrar el contenido basura que pueda haber en el pipe
    remove(args.pipeNom);
    for (int i = 0; i < args.num; ++i) { // Para cada cliente
        char pipeLectura[100]; ///< Pipe de lectura
        char pipeEscritura[100]; ///< Pipe de escritura
        sprintf(pipeLectura, "pipeLectura%d", i); // Crear el nombre del pipe de lectura
        sprintf(pipeEscritura, "pipeEscritura%d", i); // Crear el nombre del pipe de escritura
        remove(pipeLectura); // Borrar el contenido basura que pueda haber en el pipe de lectura
        remove(pipeEscritura); // Borrar el contenido basura que pueda haber en el pipe de escritura
        unlink(pipeLectura); // Borrar el pipe de lectura
        unlink(pipeEscritura); // Borrar el pipe de escritura
        clientes[i].pipeLectura = pipeLectura; // Guardar el nombre del pipe de lectura
        clientes[i].pipeEscritura = pipeEscritura; // Guardar el nombre del pipe de escritura
    }

    // Crear el primer pipe nominal de nombre args.pipeNom y abrirlo en modo escritura. Se usara para asignar un identificador y un pipe a cada usuario que se conecte al sistema.
    unlink(args.pipeNom); // Eliminar el pipe si existe
    mkfifo(args.pipeNom, 0666); // Crear el pipe

    cargarRelaciones(NULL); // Cargar las relaciones de los usuarios

    //Crear el hilo para atender solicitudes
    pthread_t hiloSolicitudes; ///< Hilo para atender solicitudes
    pthread_create(&hiloSolicitudes, NULL, atenderSolicitudes, NULL); // Crear el hilo para atender solicitudes

    // Crear el hilo para imprimir estadisticas
    pthread_t hiloEstadisticas; ///< Hilo para imprimir estadisticas
    pthread_create(&hiloEstadisticas, NULL, imprimirEstadisticas, NULL); // Crear el hilo para imprimir estadisticas

    // Esperar a que los hilos terminen
    pthread_join(hiloSolicitudes, NULL); // Esperar a que el hilo de atender solicitudes termine
    pthread_join(hiloEstadisticas, NULL); // Esperar a que el hilo de imprimir estadisticas termine

    return 0;
}




