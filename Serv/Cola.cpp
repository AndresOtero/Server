#include "Cola.h"

#ifndef NULL
#define NULL 0
#endif

Cola::Cola() {
    frente = NULL;
    ultimo = NULL;
}

Cola::~Cola() {
    while (!estaVacia()) {
        pop();
    }
}

void Cola::push(const Dato& dato) {
    NodoSimplementeEnlazado* aux = new NodoSimplementeEnlazado();
    aux->setDato(dato);
    
    if (frente == NULL) {
    	frente = aux;
    	ultimo = aux;
    }
    else {
    	ultimo->setSiguiente(aux);
    	ultimo =aux;
    }
}

Dato Cola::pop() {
    if (frente == NULL) {
        return NULL;
    }
    Dato aux = frente->getDato();
    NodoSimplementeEnlazado* aElminiar = frente;
    frente = frente->getSiguiente();
    delete aElminiar;
    return aux;
}

Dato Cola::top() {
    if (frente == NULL) {
        return NULL;
    }
    return frente->getDato();
}

bool Cola::estaVacia() {
    return (frente == NULL);
}
