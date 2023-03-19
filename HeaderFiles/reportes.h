#ifndef REPORTES_H
#define REPORTES_H
#include "../HeaderFiles/AdminDisk.h"

class reportes{
    private:
    AdminDisk admindisk;
    public:
        void passDate(AdminDisk);
        void generarReportes(string name,string path,string id,string ruta);
        string  minusculas(string);

        /// graficas
        void MBR(string path,string ruta_diso);
        void DISK(string path,string ruta_disco);
        void bms(string path,string ruta_disco,Estructuras::Partition,bool);
        void INODE(string path,string ruta_disco,Estructuras::Partition);
        void Block(string path,string ruta_disco,Estructuras::Partition);
        string Fecha(time_t valor);

};
#endif