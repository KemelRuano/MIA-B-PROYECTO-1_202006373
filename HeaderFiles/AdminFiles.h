#ifndef ADMINFILES_H
#define ADMINFILES_H
#include <string>
#include <ctime>
using namespace std;
#include "../HeaderFiles/AdminDisk.h"
#include "../HeaderFiles/AdminUG.h"
#include <vector>
class AdminFiles{

    public:
        void passDate(AdminDisk,User_Group);
        vector <string> extraerCarpetas(string);
        void MKDIR(vector<string>,string r );
        bool VerExisteCarpeta(Estructuras::Folderblock,string);

        void ActualizarBitMaps(Estructuras::Superblock,string,string);
        int Inodosiguiente(Estructuras::Superblock superbloque,string path);
        void ActualizarBloque(Estructuras::Superblock superbloque,string path,string nombre,int numero);
        int Bloquesiguiente(Estructuras::Superblock superbloque,string path);




    private:
        AdminDisk admindisk;
        User_Group user_group;



};

#endif