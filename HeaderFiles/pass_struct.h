#ifndef PASSSTRUCT_H
#define PASSSTRUCT_H
#include <string>
using namespace std;

class DATOS_PASS{
    public:

            typedef struct _mkdisk{
                string path = "";
                string size = "";
                string fit = "";
                string unit = "";

            } MKDISK; 

            typedef struct _fdisk{
                string size = "";
                string path = "";
                string name = "";
                string unit = "";
                string type = "";
                string fit = "";
                string delit = "";
                string add = "";

            } FDISK; 

            typedef struct _rmdisk{
       
                string path = "";

            } RMDISK;
         

};

struct Transicion {
    int status;
    int inicio;
    int fin;
    int num_particion;
};

struct Nodo {
    Transicion cambio;
    Nodo* siguiente;
    Nodo* anterior;
};

class ListaDobleEnlazada {
private:
    Nodo* primero;
    Nodo* ultimo;
public:
    ListaDobleEnlazada() {
        primero = nullptr;
        ultimo = nullptr;
    }
    
    void agregarAlFinal(Transicion p) {
        Nodo* nuevoNodo = new Nodo;
        nuevoNodo->cambio = p;
        nuevoNodo->siguiente = nullptr;
        
        if (ultimo == nullptr) {
            nuevoNodo->anterior = nullptr;
            primero = nuevoNodo;
            ultimo = nuevoNodo;
        } else {
            nuevoNodo->anterior = ultimo;
            ultimo->siguiente = nuevoNodo;
            ultimo = nuevoNodo;
        }
    }
    
 
};
#endif