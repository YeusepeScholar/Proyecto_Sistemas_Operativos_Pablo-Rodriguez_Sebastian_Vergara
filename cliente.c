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
int fdLectura, fdEscritura;
/// @brief Funcion para imprimir lo que se recibe por el pipe
void *imprimir(void *arg){
    printf("Imprimiendo lo que se recibe por el pipe de lectura");
    char buffer[100];
    while (1) {
        read(fdLectura, buffer, 100);
        printf("%s", buffer);
    }
}

int main(int argc, char *argv[]) {
    // Inicializar el cliente
    args = extraerDatosConsola(argc, argv);

    // Abrir el pipe como escritura
    printf("ID: %d, pipeNom: %s\n", args.id, args.pipeNom);
    int fd = open(args.pipeNom, O_WRONLY);
    // Abrir un pipe de nombre IDPIPE para la comunicación exclusiva entre el Gestor y el Cliente
    char idPipe[100];
    sprintf(idPipe, "%dPIPE", args.id);
    mkfifo(idPipe, 0666);
    // Mandar el ID al gestor
    char id[100];
    sprintf(id, "ID%d", args.id);
    write(fd, id, strlen(id));


    // Abrir el pipe de nombre IDPIPE como lectura
    unlink(idPipe); // Eliminar el pipe si existe
    fd = open(idPipe, O_RDONLY);
}