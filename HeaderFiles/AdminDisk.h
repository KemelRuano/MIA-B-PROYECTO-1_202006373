#ifndef ADMINDISK_H
#define ADMINDISK_H
#include <string>
#include <vector>
#include <iostream>
#include "../HeaderFiles/Tokens.h"
#include "../HeaderFiles/pass_struct.h"
#include "../HeaderFiles/Class_Estruct.h"
#include <fstream>
using namespace std;

class AdminDisk{
    public:

         
       // para mkdisk
        void crear_disk(string size,string path,string unit,string fit);
        void eliminar_disk(string path);
        string  obtner_rutaCarpeta(string path);
        bool ver_existeCarpeta(string path);
        bool file_existe(string documento);
        void mostrar_mbr(string path);

        // para el fdisk
         void CrearParticion(DATOS_PASS::FDISK);
        //
        
        Estructuras::DISCO particiones(Estructuras::DISCO,Estructuras::Partition);
        vector<Estructuras::Partition> List_partition(Estructuras::DISCO);

        void controler(vector<Estructuras::Partition>);

    


};
#endif