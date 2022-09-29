//
// Created by svalp on 9/28/2022.
//

#ifndef PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_FILEPARSER_H
#define PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_FILEPARSER_H

#include <stdio.h>

/// @brief Struct que contiene los datos de un archivo
struct fileParser{
    /// @brief Cantidad de usuarios
    int num;
    /// @brief Matriz de relaciones
    int **relaciones;
};

/// @brief Función que se encarga de extraer los datos de un archivo
/// @param path Ruta del archivo
/// @return Estructura con los datos extraidos

struct fileParser extraerDatosArchivo(char *path){
    struct fileParser fileParser;
    fileParser.num = 0;
    fileParser.relaciones = NULL;

    FILE *file = fopen(path, "r");
    if (file == NULL){
        printf("Error al abrir el archivo");
        return fileParser;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    int i = 0;
    while ((read = getline(&line, &len, file)) != -1) {
        if (i == 0){
            fileParser.num = 1;
            fileParser.relaciones = (int **) malloc(fileParser.num * sizeof(int *));
            for (int j = 0; j < fileParser.num; ++j) {
                fileParser.relaciones[j] = (int *) malloc(fileParser.num * sizeof(int));
            }
        } else {
            char *token = strtok(line, " ");
            int j = 0;
            while (token != NULL){
                fileParser.relaciones[i - 1][j] = atoi(token);
                token = strtok(NULL, " ");
                j++;
            }
            fileParser.num++;
        }
        i++;
    }

    fclose(file);
    if (line)
        free(line);

    return fileParser;
}


#endif //PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_FILEPARSER_H
