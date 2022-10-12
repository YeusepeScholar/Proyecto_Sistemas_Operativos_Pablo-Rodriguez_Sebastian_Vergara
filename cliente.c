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
int fdLectura, fdEscritura; ///< Descriptores de archivo para el pipe
/// @brief Funcion para imprimir lo que se recibe por el pipe
void *imprimir(void *arg){
    printf("Imprimiendo lo que se recibe por el pipe de lectura");
    char buffer[100]; ///< Buffer para leer del pipe
    while (1) { // Leer del pipe
        read(fdLectura, buffer, 100); // Leer del pipe
        printf("%s", buffer); // Imprimir lo que se recibe por el pipe
    }
}

int main(int argc, char *argv[]) {
    // Inicializar el cliente
    args = extraerDatosConsola(argc, argv); // Extraer los datos de la consola

    // Abrir el pipe como escritura
    printf("ID: %d, pipeNom: %s\n", args.id, args.pipeNom);
    int fd = open(args.pipeNom, O_WRONLY); ///< Descriptor de archivo para el pipe
    // Abrir un pipe de nombre IDPIPE para la comunicación exclusiva entre el Gestor y el Cliente
    char idPipe[100]; ///< Nombre del pipe de comunicacion exclusiva entre el Gestor y el Cliente
    sprintf(idPipe, "%dPIPE", args.id);
    mkfifo(idPipe, 0666); // Crear el pipe
    // Mandar el ID al gestor
    char id[100]; ///< Buffer para almacenar el id
    sprintf(id, "ID%d", args.id); // Crear el mensaje con el id
    write(fd, id, strlen(id)); // Mandar el id al gestor


    // Abrir el pipe de nombre IDPIPE como lectura
    unlink(idPipe); // Eliminar el pipe si existe
    fd = open(idPipe, O_RDONLY); // Abrir el pipe como lectura
}