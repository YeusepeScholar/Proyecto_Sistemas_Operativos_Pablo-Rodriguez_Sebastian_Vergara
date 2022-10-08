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
// Como entra la informacion por consola: cliente -i ID -p pipeNom
// Compilar: gcc -o cliente cliente.c -lpthread
// Ejecutar: ./cliente -i 1 -p pipeNom
struct argumentos args; ///< Estructura para almacenar los argumentos de la consola
int main(int argc, char *argv[]) {
    // Inicializar el cliente
    args = extraerDatosConsola(argc, argv);
    // Abrir el pipe como escritura
    printf("ID: %d, pipeNom: %s\n", args.id, args.pipeNom);
    int fd = open(args.pipeNom, O_WRONLY);
    // testear si se abrio correctamente
    if (fd == -1){
        perror("Error al abrir el pipe");
        exit(EXIT_FAILURE);
    }

    // Enviar el identificador del usuario al Gestor usando el pipe como ID #
    char mensaje[100];
    sprintf(mensaje, "ID%d", args.id);

    // Abrir un pipe de nombre IDPIPE para la comunicación exclusiva entre el Gestor y el Cliente
    char idPipe[100];
    sprintf(idPipe, "pipeCliente%d", args.id);
    if (mkfifo(idPipe, 0666) == -1) {
        perror("Error al crear el pipe");
        exit(EXIT_FAILURE);
    }

    // Cerrar el pipe
    // close(fd);

    // Abrir el pipe de nombre IDPIPE como lectura
    unlink(idPipe); // Eliminar el pipe si existe
    fd = open(idPipe, O_RDONLY);
}