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

    fclose(file);

    return fileParser;
}


#endif //PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_FILEPARSER_H
