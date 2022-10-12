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
    struct fileParser fileParser; ///< Estructura para almacenar los datos del archivo
    fileParser.num = 0; ///< Inicializar el numero de usuarios
    fileParser.relaciones = NULL; ///< Inicializar la matriz de relaciones

    FILE *file = fopen(path, "r"); ///< Archivo a leer
    if (file == NULL){ // Si el archivo no existe
        printf("Error al abrir el archivo");
        return fileParser;
    }

    fclose(file); ///< Cerrar el archivo

    return fileParser; ///< Retornar la estructura
}


#endif //PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_FILEPARSER_H
