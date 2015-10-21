#ifndef COLA_H_
#define COLA_H_

#include "NodoSimplementeEnlazado.h"

class Cola {
private:
    NodoSimplementeEnlazado* frente;
    NodoSimplementeEnlazado* ultimo;
    
public:
    Cola();
    ~Cola();

    /**
     * Agrega el elemento a la cola
     */
    void push(const Dato& dato);

    /**
     * Retorna el primer dato ingresado, y lo elimina de la cola
     */
    Dato pop();

    /**
     * Retorna el primer dato ingresado
     */
    Dato top();

    bool estaVacia();
};

#endif
