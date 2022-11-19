//
// Created by svalp on 9/29/2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include "extractorConsola.h" // Funciones para extraer datos de la consola
#include "fileParser.h" // Funciones para extraer datos de un archivo
#include "cliente.h" // Estructura para almacenar los datos de un cliente
// Como entra la informacion por consola: cliente -i ID -p pipeNom
// Compilar: gcc -o cliente cliente.c -lpthread
// Ejecutar: ./cliente -i 1 -p pipeNom
struct argumentos args; ///< Estructura para almacenar los argumentos de la consola
int fdLectura, fdEscritura, fd; ///< Descriptores de archivo para el pipe
char pipeCliente[100]; // Pipe para el id

/// @brief Funcion para imprimir lo que se recibe por el pipe
void *imprimir(void *arg){
    printf("Hilo de lectura creado\n\n");

    fdLectura = open(pipeCliente, O_RDONLY); // Abrir el pipe para lectura

    char buffer[250]; ///< Buffer para leer del pipe
    while (true) { // Leer del pipe
        if (read(fdLectura, buffer, 100) > 0) { // Leer del pipe
            // Si el buffer tiene "Tweet: " significa que es un tweet
            if (strstr(buffer, "Tweet: ") != NULL) {
                // Buscar el \n para saber donde termina el tweet, y imprimir hasta ahi
                for (int i = 0; i < strlen(buffer); ++i) {
                    if (buffer[i] == '\0') {
                        buffer[i] = '\0';
                        break;
                    } else {
                        printf("%c", buffer[i]);
                    }
                }
            } else if (strstr(buffer, "ERROR: ") != NULL) { // Si el buffer tiene "ERROR: " significa que es un error
                printf("%s", buffer);
                exit(1);
            }
            else { // Si no es un tweet, es un mensaje de error
                printf("%s\n", buffer); // Imprimir lo que se recibe por el pipe
            }

            // Borra el contenido del buffer
            memset(buffer, 0, 250);
        }
        // Borra el buffer
        memset(buffer, 0, 250);
    }
}

/**
 * @brief Funcion para ayuda
 */
void ayuda(){
    printf("Ayuda\n");
    printf("cliente -i ID -p pipeNom\n");
    printf("ID: Identificador del cliente\n");
    printf("pipeNom: Nombre del pipe por el cual se va a comunicar con el cliente\n");
    printf("----------------------------------------\n");
    printf("Operaciones:\n");
    printf("f seguir a un usuario\n Uso: f usuario\n");
    printf("u dejar de seguir a un usuario\n Uso: u usuario\n");
    printf("t enviar un tweet\n Uso: t tweet\n");
    printf("r recibir los tweets de los usuarios a los que esta suscrito en modo desacoplado\n Uso: r\n");
    printf("d desconectarse\n Uso: d\n");
    printf("a ayuda\n Uso: a\n");

}

/**
 * @brief Funcion para leer lo que se escribe en la consola
 */
void *leerConsola(void *arg) {
    printf("Hilo de escritura creado\n\n");

    fdEscritura = open(args.pipeNom, O_WRONLY); // Abrir el pipe para escritura

    char buffer[200]; ///< Buffer para escribir en el pipe
    while (true) { // Leer de la consola
        fgets(buffer, 100, stdin); // Leer de la consola

        // Formato de los mensajes que se envian por el pipe
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

        // Operaciones
        // f: follow
        // u: unfollow
        // t: tweet
        // r: recuperar tweets
        // d: desconectar
        // a: ayuda

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

        // Si es f, o u entonces se debe enviar el id del cliente al que se desea seguir o dejar de seguir
        // Entrada por consola: f 2  // Seguir al cliente 2
        // Entrada por consola: u 2  // Dejar de seguir al cliente 2
        // Salida por pipe: f 1 2  // El cliente 1 sigue al cliente 2
        // Salida por pipe: u 1 2  // El cliente 1 deja de seguir al cliente 2
        if (buffer[0] == 'f' || buffer[0] == 'u') {
            int id = atoi(&buffer[2]); // Extraer el id del cliente al que se desea seguir o dejar de seguir
            sprintf(buffer, "%c %d %d", buffer[0], args.id, id); // Formatear el mensaje
            write(fdEscritura, buffer, 100); // Escribir en el pipe
        }

            // Si es t entonces se debe enviar el mensaje que se desea enviar
            // Entrada por consola: t Hola mundo  // Enviar el mensaje Hola mundo
            // Salida por pipe: t 1 Hola mundo  // El cliente 1 envia el mensaje Hola mundo
        else if (buffer[0] == 't') {
            char mensaje[100]; // Buffer para el mensaje
            // Extraer el mensaje
            for (int i = 2; i < strlen(buffer); ++i) {
                mensaje[i-2] = buffer[i];
            }
            sprintf(buffer, "%c %d %s", buffer[0], args.id, mensaje); // Formatear el mensaje
            write(fdEscritura, buffer, 100); // Escribir en el pipe
            // Borra el contenido del buffer
            memset(mensaje, 0, 100);
        }

            // Si es r y el servidor esta corriendo en modo desacoplado entonces se recupera el historial de tweets
            // Entrada por consola: r  // Recuperar el historial de tweets
            // Salida por pipe: r 1  // El cliente 1 recupera el historial de tweets
        else if (buffer[0] == 'r') {
            sprintf(buffer, "%c %d", buffer[0], args.id); // Formatear el mensaje
            write(fdEscritura, buffer, 100); // Escribir en el pipe
        }

            // Si es d entonces se desconecta del servidor
            // Entrada por consola: d  // Desconectar del servidor
            // Salida por pipe: d 1  // El cliente 1 se desconecta del servidor
        else if (buffer[0] == 'd') {
            sprintf(buffer, "%c %d", buffer[0], args.id); // Formatear el mensaje
            write(fdEscritura, buffer, 100); // Escribir en el pipe
            exit(0); // Salir del programa
        }

            // Si es a entonces se accede a la funcion de ayuda
            // Entrada por consola: a  // Acceder a la funcion de ayuda
        else if (buffer[0] == 'a') {
            ayuda(); // Acceder a la funcion de ayuda
        }

            // Si no es ninguna de las anteriores entonces se imprime un mensaje de error
        else {
            printf("Comando no reconocido\n");
        }

        // Borra el buffer
        memset(buffer, 0, 100);
    }
}

int main(int argc, char *argv[]) {
    // Inicializar el cliente
    args = extraerDatosConsola(argc, argv); // Extraer los datos de la consola
    // Si hace falta el id en la consola entonces se retorna un error
    if (args.id == -1) {
        printf("Error: El id es obligatorio\n");
        return 1;
    }

    // El formato del pipe es: pipeCliente%d
    sprintf(pipeCliente, "pipeCliente%d", args.id); // Crear el nombre del pipe
    unlink(pipeCliente); // Eliminar el pipe si existe
    if (mkfifo(pipeCliente, 0666) == -1) { // Crear el pipe
        perror("Error al crear el pipe");
        exit(1);
    }

    pthread_t hilo; ///< Hilo para imprimir lo que se recibe por el pipe
    pthread_create(&hilo, NULL, imprimir, NULL); // Crear el hilo


    // Abrir el pipe como escritura
    printf("ID: %d, pipeNom: %s\n", args.id, args.pipeNom);
    fd = open(args.pipeNom, O_WRONLY); ///< Descriptor de archivo para el pipe

    pthread_t hilo2; ///< Hilo para leer lo que se escribe en la consola
    pthread_create(&hilo2, NULL, leerConsola, NULL); // Crear el hilo

    // Mandar el ID al gestor
    char id[100]; ///< Buffer para almacenar el id
    sprintf(id, "ID%d", args.id); // Crear el mensaje con el id
    write(fd, id, strlen(id)); // Mandar el id al gestor

    pthread_join(hilo, NULL); // Esperar a que el hilo termine
    pthread_join(hilo2, NULL); // Esperar a que el hilo termine
}