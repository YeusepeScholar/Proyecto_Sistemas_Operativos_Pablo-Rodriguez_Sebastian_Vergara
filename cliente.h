//
// Created by svalp on 9/29/2022.
//

#ifndef PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_CLIENTE_H
#define PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_CLIENTE_H

#include <stdbool.h>

// Tipo de dato para el identificador de un cliente
struct cliente {
    int id; ///< Identificador del cliente
    char* pipeLectura, *pipeEscritura; ///< Pipe por el cual se va a comunicar con el cliente
    bool conectado; ///< Indica si el cliente esta conectado
    // Arreglo de clientes a los que esta suscrito
    bool *suscripciones; ///< Arreglo de suscripciones
    char **tweets; ///< Arreglo de tweets
    int numSuscripciones; ///< Numero de suscripciones
};
#endif //PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_CLIENTE_H
