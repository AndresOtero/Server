#ifndef NODOSIMPLEMENTEENLAZADO_H_
#define NODOSIMPLEMENTEENLAZADO_H_

/**
 * Tipo de Informacion que esta contenida en los Nodos de la Lista.
 * En las bibliografias se suelen utilizar sinonimos como Info o Elemento. 
**/
typedef void* Dato;

/*
 * Tipo de Estructura de los Nodos de una Lista Simplemente Enlazada.
*/
class NodoSimplementeEnlazado
{
private:
    //Dato almacenado
    Dato dato;
    //Puntero al siguiente nodo
    NodoSimplementeEnlazado* siguiente;
    
public:
    /*
     * Construye un nuevo nodo
     */
    NodoSimplementeEnlazado();
    
    /*
     * Construye un nuevo nodo con un elemento
     */
    NodoSimplementeEnlazado(Dato dato);
    
    /*
     * Destruye el nodo
     */
    virtual ~NodoSimplementeEnlazado();

    /*
     * Setea el dato al nodo
     */
    void setDato(Dato dato);
    
    /*
     * Retorna el dato del nodo
     */
    Dato getDato();
    
    /*
     * Setea el siguiente nodo
     */
    void setSiguiente(NodoSimplementeEnlazado* siguiente);
    
    /*
     * Retorna el siguiente nodo
     */
    NodoSimplementeEnlazado*& getSiguiente();
    
    /*
     * Establece si el nodo tiene un siguiente
     */
    bool tieneSiguiente() const;

};

#endif
