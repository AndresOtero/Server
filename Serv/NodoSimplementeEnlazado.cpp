#include "NodoSimplementeEnlazado.h"

#ifndef NULL
#define NULL 0
#endif

NodoSimplementeEnlazado::NodoSimplementeEnlazado()
{
    siguiente = NULL;
}

NodoSimplementeEnlazado::~NodoSimplementeEnlazado()
{}

void NodoSimplementeEnlazado::setDato(Dato dato)
{
    this->dato = dato;
}

Dato NodoSimplementeEnlazado::getDato()
{
    return this->dato;
}

NodoSimplementeEnlazado*& NodoSimplementeEnlazado::getSiguiente()
{
    return this->siguiente;
}

void NodoSimplementeEnlazado::setSiguiente(NodoSimplementeEnlazado* siguiente)
{
    this->siguiente = siguiente;
}


bool NodoSimplementeEnlazado::tieneSiguiente() const
{
    return (this->siguiente != NULL);
}
