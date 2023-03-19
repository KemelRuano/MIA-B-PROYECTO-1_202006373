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

        typedef struct _Transition {
        int partition;
        int start;
        int end;
        int before;
        int after;
        } Transition;

       // para mkdisk
        void crear_disk(string size,string path,string unit,string fit);
        void eliminar_disk(string path);
        string  obtner_rutaCarpeta(string path);
        bool ver_existeCarpeta(string path);
        bool file_existe(string documento);
        void mostrar_mbr(string path);

        // para el fdisk
        void CrearParticion(DATOS_PASS::FDISK);  
        vector<Estructuras::Partition> List_partition(Estructuras::DISCO);
        Estructuras::DISCO adjust(Estructuras::DISCO , Estructuras::Partition, vector<Transition>, vector<Estructuras::Partition>, int);
        Estructuras::Partition findby(Estructuras::DISCO, string name, string path);
        void logic(Estructuras::Partition , Estructuras::Partition, string p);
        vector<Estructuras::EBR> getlogics(Estructuras::Partition partition, string p);
        void addEspace(string path ,string nombre,string unidades,string cantidad);
        void EliminarParticion(string d, string p, string n) ;

        // metodos alternos
        string LimpiarString(string);

        // mount
        void mount(string path,string name);
        string extrerNameDisk(string name);
        vector<Mount> List_mounts;
        void verVector();
        void unmount(string path);
        Estructuras::Partition EncontrarPartition(string id , string *p);

        // mkfs
        void mkfs(string id ,string type , string fs);
        void Format_ext2(Estructuras::Superblock,Estructuras::Partition,int bloques , string path);
        void Format_ext3(Estructuras::Superblock,Estructuras::Partition,int bloques , string path);
};
#endif