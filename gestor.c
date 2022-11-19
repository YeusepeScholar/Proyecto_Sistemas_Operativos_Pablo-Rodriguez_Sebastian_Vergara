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


void imprimirRelaciones() {
    /*printf("Relaciones cargadas\n");
    for (int i = 0; i < args.num; i++){ // Recorrer usuarios
        printf("Usuario %d: ", i); // Imprimir usuario
        for (int j = 0; j < args.num; j++){ // Recorrer usuarios
            printf("%d ", clientes[i].suscripciones[j]); // Imprimir suscripcion
        }
        printf("\n"); // Salto de linea
    }*/
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
    // Si no existe el archivo, crear uno nuevo
    if (fp == NULL){
        printf("No existe el archivo de relaciones, creando uno nuevo\n");
        fp = fopen(args.relaciones, "w");
        fclose(fp);
        fp = fopen(args.relaciones, "r");
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
    imprimirRelaciones();

    return NULL; // Retornar
}

/// @brief Funcion para atención de solicitudes de los procesos Cliente
/// @param arg Argumentos de la consola
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
                    // Crear el pipe para el id
                    char pipeCliente[100]; // Pipe para el id
                    // El formato del pipe es: pipeCliente%d
                    sprintf(pipeCliente, "pipeCliente%d", id);
                    printf("Conectando al pipe: %s\n", pipeCliente);
                    // Abrir el pipe como escritura
                    clientes[id].pipeLectura = open(pipeCliente, O_WRONLY);
                    // Si no se pudo abrir el pipe
                    if (clientes[id].pipeLectura == -1) {
                        perror("Error al abrir el pipe");
                    }
                    // Enviar mensaje de conexion
                    char mensaje[100]; // Mensaje a enviar
                    // Mandar mensaje de conexion
                    sprintf(mensaje, "Conectado al servidor\n");
                    // Escribir en el pipe
                    write(clientes[id].pipeLectura, mensaje, strlen(mensaje));

                    // Imprimir el pipe
                    printf("Conectado al pipe: %s\n", pipeCliente);

                    // Borrar mensaje
                    memset(mensaje, 0, 100);
                    // Mandar mensaje de conexion
                    sprintf(mensaje, "\nTweets Archivados: \n");
                    // Escribir en el pipe
                    write(clientes[id].pipeLectura, mensaje, strlen(mensaje));

                    // Enviar los tweets a los que el usuario se suscribio
                    for (int i = 0; i < args.num; i++) { // Para cada usuario
                        if (clientes[id].suscripciones[i] == 1) { // Si el usuario se suscribio al usuario i
                            for (int j = 0; j < clientes[i].numTweets; j++) { // Para cada tweet del usuario i
                                // Enviar el tweet por el pipe
                                tweetsEnviados++;
                                write(clientes[id].pipeLectura, clientes[i].tweets[j], strlen(clientes[i].tweets[j]));
                            }
                        }
                    }

                    // Borrar mensaje
                    memset(mensaje, 0, 100);
                    // Mandar mensaje de conexion
                    sprintf(mensaje, "\nGestor listo para recibir comandos. \n");
                    // Escribir en el pipe
                    write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                } else {
                    // Enviar mensaje de conexion
                    char mensaje[100]; // Mensaje a enviar
                    // Mandar mensaje de conexion
                    sprintf(mensaje, "ERROR: Otra conexion de usuario\n");
                    // Escribir en el pipe
                    write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                }
            } else {
                // Enviar mensaje de conexion
                char mensaje[100]; // Mensaje a enviar
                // Mandar mensaje de conexion
                sprintf(mensaje, "ERROR: El ID no esta en el rango\n");
                // Escribir en el pipe
                write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
            }
        }
            // Operaciones
            // f: follow
            // u: unfollow
            // t: tweet
            // r: recuperar tweets
            // d: desconectar
            // a: ayuda
            // s: salir
            // Formato de los mensajes que se reciben por el pipe
            // operacion ID Mensaje
            // ID: identificador del cliente
            // operacion: operacion que se desea realizar
            // mensaje: mensaje que se desea enviar
            // Ejemplo: t 1 Hola mundo
            // Ejemplo: f 1 2
            // Ejemplo: u 1 2
            // Ejemplo: r 1
            // Ejemplo: d
            // Ejemplo: a f

            // Si el primer caracter es f
        else if (buffer[0] == 'f') { // Si el primer caracter es f
            printf("Operacion: follow\n");
            printf("Cantidad maxima de usuarios conectados: %d\n", args.num);
            // Extraer el id
            int id = atoi(&buffer[2]);
            // Si el id esta en el rango
            if (id >= 0 && id < args.num) {
                // Si el id esta conectado
                if (clientes[id].conectado == true) {
                    // Extraer el id del usuario a seguir
                    int idSeguir = atoi(&buffer[4]);
                    // Si el id del usuario a seguir esta en el rango
                    if (idSeguir >= 0 && idSeguir < args.num) {
                        // Si el id del usuario a seguir no es el mismo que el id
                        if (idSeguir != id) {
                            printf("Usuario %d sigue a %d\n", id, idSeguir);
                            // Si el id no sigue al id del usuario a seguir
                            if (clientes[id].suscripciones[idSeguir] == 0) {
                                imprimirRelaciones();
                                // Seguir al usuario
                                clientes[id].suscripciones[idSeguir] = 1;
                                // Incrementar el numero de suscripciones del id
                                clientes[id].numSuscripciones++;
                                imprimirRelaciones();
                                // Enviar mensaje de confirmacion
                                char mensaje[100]; // Mensaje a enviar
                                sprintf(mensaje, "Seguimiento exitoso");
                                printf("Enviando mensaje: %s\n", mensaje);
                                // Escribir en el pipe
                                write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                            } else { // Si el id ya sigue al id del usuario a seguir
                                // Enviar mensaje de error
                                char mensaje[100]; // Mensaje a enviar
                                sprintf(mensaje, "Error: Ya sigue al usuario");
                                printf("Enviando mensaje: %s\n", mensaje);
                                // Escribir en el pipe
                                write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                            }
                        } else { // Si el id del usuario a seguir es el mismo que el id
                            // Enviar mensaje de error
                            char mensaje[100]; // Mensaje a enviar
                            sprintf(mensaje, "Error: No se puede seguir a si mismo");
                            printf("Enviando mensaje: %s\n", mensaje);
                            // Escribir en el pipe
                            write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                        }
                    } else {
                        // Enviar mensaje de error
                        char mensaje[100]; // Mensaje a enviar
                        sprintf(mensaje, "Error: El ID no esta en el rango");
                        printf("Enviando mensaje: %s\n", mensaje);
                        // Escribir en el pipe
                        write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                    }
                }
            } else {
                // Enviar mensaje de error
                char mensaje[100]; // Mensaje a enviar
                sprintf(mensaje, "Error: El ID no esta en el rango");
                printf("Enviando mensaje: %s\n", mensaje);
                // Escribir en el pipe
                write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
            }
        }

        // Si el primer caracter es u
        else if (buffer[0] == 'u') { // Si el primer caracter es u
            printf("Operacion: unfollow\n");
            // Extraer el id
            int id = atoi(&buffer[2]);
            // Si el id esta en el rango
            if (id >= 0 && id < args.num) {
                // Si el id esta conectado
                if (clientes[id].conectado == true) {
                    // Extraer el id del usuario a dejar de seguir
                    int idSeguir = atoi(&buffer[4]);
                    // Si el id del usuario a dejar de seguir esta en el rango
                    if (idSeguir >= 0 && idSeguir < args.num) {
                        // Si el id del usuario a dejar de seguir no es el mismo que el id
                        if (idSeguir != id) {
                            // Si el id sigue al id del usuario a dejar de seguir
                            if (clientes[id].suscripciones[idSeguir] == 1) {
                                // Dejar de seguir al usuario
                                clientes[id].suscripciones[idSeguir] = 0;
                                // Decrementar el numero de suscripciones del id
                                clientes[id].numSuscripciones--;
                                // Enviar mensaje de confirmacion
                                char mensaje[100]; // Mensaje a enviar
                                sprintf(mensaje, "Dejado de seguir exitoso");
                                printf("Enviando mensaje: %s\n", mensaje);
                                // Escribir en el pipe
                                write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                            } else { // Si el id no sigue al id del usuario a dejar de seguir
                                // Enviar mensaje de error
                                char mensaje[100]; // Mensaje a enviar
                                sprintf(mensaje, "Error: No sigue al usuario");
                                printf("Enviando mensaje: %s\n", mensaje);
                                // Escribir en el pipe
                                write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                            }
                        } else { // Si el id del usuario a dejar de seguir es el mismo que el id
                            // Enviar mensaje de error
                            char mensaje[100]; // Mensaje a enviar
                            sprintf(mensaje, "Error: No se puede dejar de seguir a si mismo");
                            printf("Enviando mensaje: %s\n", mensaje);
                            // Escribir en el pipe
                            write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                        }
                    } else {
                        // Enviar mensaje de error
                        char mensaje[100]; // Mensaje a enviar
                        sprintf(mensaje, "Error: El ID no esta en el rango");
                        printf("Enviando mensaje: %s\n", mensaje);
                        // Escribir en el pipe
                        write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                    }
                }
            } else {
                // Enviar mensaje de error
                char mensaje[100]; // Mensaje a enviar
                sprintf(mensaje, "Error: El ID no esta en el rango");
                printf("Enviando mensaje: %s\n", mensaje);
                // Escribir en el pipe
                write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
            }
        }

            // Si la operacion es t
        else if (buffer[0] == 't') { // Si la operacion es t
            printf("Operacion: tweet\n");
            // Extraer el id
            int id = atoi(&buffer[2]);
            // Si el id esta en el rango
            if (id >= 0 && id < args.num) {
                // Si el id esta conectado
                if (clientes[id].conectado == true) {
                    // Añadir 1 al numero de tweets del gestor
                    tweetsRecibidos++;
                    // Guardar el tweet
                    char tweet[100]; // Tweet a guardar
                    strcpy(tweet, &buffer[4]);
                    // Cambiar el tamaño del arreglo
                    clientes[id].tweets = realloc(clientes[id].tweets, (clientes[id].numTweets + 1) * sizeof(char*));
                    // Guardar el tweet
                    clientes[id].tweets[clientes[id].numTweets] = malloc(strlen(tweet) * sizeof(char));
                    strcpy(clientes[id].tweets[clientes[id].numTweets], tweet);
                    // Incrementar el numero de tweets del id
                    clientes[id].numTweets++;
                    // Enviar mensaje de confirmacion
                    char mensaje[100]; // Mensaje a enviar
                    sprintf(mensaje, "Tweet exitoso");
                    printf("Enviando mensaje: %s\n", mensaje);
                    // Escribir en el pipe
                    write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                    // Enviar los mensajes del timeline
                    if(args.modo == 'a'){
                        for (int i = 0; i < args.num; i++) { // Para cada id
                            // Si el id esta conectado
                            if (clientes[i].conectado == true) {
                                tweetsEnviados++;
                                // Si el id sigue al id
                                if (clientes[i].suscripciones[id] == 1) {
                                    // Enviar el tweet
                                    char mensaje[200]; // Mensaje a enviar
                                    sprintf(mensaje, "Tweet: %s", tweet);
                                    printf("Enviando mensaje: %s\n", mensaje);
                                    // Escribir en el pipe
                                    write(clientes[i].pipeLectura, mensaje, strlen(mensaje));
                                }
                            }
                        }
                    } else {
                        printf("Guardando tweet en el buffer\n");
                    }
                }
            }
        }

            // Si la operacion es r
        else if (buffer[0] == 'r') { // Si la operacion es r
            printf("Operacion: read\n");

            // Extraer el id
            int id = atoi(&buffer[2]);
            if(args.modo == 'd'){// Si el id esta en el rango
                if (id >= 0 && id < args.num) {
                    // Si el id esta conectado
                    if (clientes[id].conectado == true) {
                        printf("Enviando tweets del buffer\n");
                        // Imprimir cantidad de tweets en el buffer
                        printf("Cantidad de tweets en el buffer: %d\n", clientes[id].numTweets);
                        // Buscar tweets de los usuarios que sigue
                        for (int i = 0; i < args.num; i++) { // Para cada id
                            // Si el id sigue al id
                            if (clientes[id].suscripciones[i] == 1) {
                                // Imprimir cantidad de tweets del id
                                printf("Cantidad de tweets del usuario %d: %d\n", i, clientes[i].numTweets);
                                // Enviar los tweets del id
                                for (int j = 0; j < clientes[i].numTweets; j++) { // Para cada tweet
                                    tweetsEnviados++;
                                    // Enviar el tweet
                                    char mensaje[200]; // Mensaje a enviar
                                    sprintf(mensaje, "Tweet: %s", clientes[i].tweets[j]);
                                    printf("Enviando mensaje: %s\n", mensaje);
                                    // Escribir en el pipe
                                    write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                                }
                            }
                        }
                    }
                }
            } else
            {
                char mensaje[100]; // Mensaje a enviar
                sprintf(mensaje, "Error: No se puede leer el timeline en modo a");
                printf("Enviando mensaje: %s\n", mensaje);
                // Escribir en el pipe
                write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
            }

        }

            // Si la operacion es d
        else if (buffer[0] == 'd') { // Si la operacion es d
            printf("Operacion: disconnect\n");
            // Extraer el id
            int id = atoi(&buffer[2]);
            // Si el id esta en el rango
            if (id >= 0 && id < args.num) {
                // Si el id esta conectado
                if (clientes[id].conectado == true) {
                    // Desconectar el id
                    clientes[id].conectado = false;
                    // Enviar mensaje de confirmacion
                    char mensaje[100]; // Mensaje a enviar
                    sprintf(mensaje, "Desconexion exitosa");
                    printf("Enviando mensaje: %s\n", mensaje);
                    // Escribir en el pipe
                    write(clientes[id].pipeLectura, mensaje, strlen(mensaje));
                }
            }
        }

        else if(n != 0){
            // Imprimir el mensaje
            printf("Mensaje: %s\n", buffer);
        }


        // Borra el buffer
        memset(buffer, 0, 100);
    }
}


/// @brief Funcion principal del proceso Gestor
int main(int argc, char *argv[]) {
    // Inicializar el gestor

    // Extraer los datos de la consola¿
    args = extraerDatosConsola(argc, argv);

    // Manejo de errores
    // Si hace falta las relaciones
    if (args.relaciones == NULL) {
        printf("Error: Falta el archivo de relaciones\n");
        exit(1);
    }


    // Todos los clientes tienen id -1
    clientes = malloc(sizeof(struct cliente) * args.num);
    for (int i = 0; i < args.num; ++i) { // Para cada cliente
        clientes[i].id = -1; // Inicializar el id en -1
        clientes[i].conectado = false; // Inicializar conectado en false
        clientes[i].suscripciones = malloc(sizeof(int) * args.num); // Inicializar suscripciones
    }

    // Borrar el contenido basura que pueda haber en el pipe
    remove(args.pipeNom);

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




