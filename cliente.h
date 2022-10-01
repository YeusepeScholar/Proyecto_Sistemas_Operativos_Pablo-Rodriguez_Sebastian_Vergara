//
// Created by svalp on 9/29/2022.
//

#ifndef PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_CLIENTE_H
#define PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_CLIENTE_H
#define MAX_SUBSCRIPCIONES 100
// Tipo de dato para el identificador de un cliente
struct cliente {
    int id; ///< Identificador del cliente
    int pipe; ///< Pipe por el cual se va a comunicar con el cliente
    // Arreglo de clientes a los que esta suscrito
    struct cliente *suscripciones[MAX_SUBSCRIPCIONES];
    void *tweets;
};
#endif //PROYECTO_SISTEMAS_OPERATIVOS_PABLO_RODRIGUEZ_SEBASTIAN_VERGARA_CLIENTE_H
