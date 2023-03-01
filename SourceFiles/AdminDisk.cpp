#include "../HeaderFiles/Tokens.h"
#include "../HeaderFiles/AdminDisk.h"
#include "../HeaderFiles/Class_Estruct.h"
#include "../HeaderFiles/pass_struct.h"
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>
#include <cstdio>
#include <algorithm>
#include <stdlib.h>
#include <cctype>
#include <cstring>

using namespace std;

int inicio = 0;
ListaDobleEnlazada list;

void AdminDisk::crear_disk(string size,string path,string unit,string fit){
        // limpiar comillas si trae espacio
        for (int i = 0; i < path.length(); i++){
              if(path[i] == '"'){
                  path.erase(i,1);
              }
        } 
        //Carpetas
        string ruta = obtner_rutaCarpeta(path);
        bool exist_carpeta = ver_existeCarpeta(ruta);
        if (exist_carpeta == false){
            if ( mkdir(ruta.c_str(), 0755) == 0) {
                     cout << "  correcto ✓✓✓ [mkdisk] ----- carpeta creada" << endl;
            }
        }

        bool estado = true;
        if(file_existe(path) == true){
                cout << "  error xxx   [mkdisk]---- ya existe el disco con ese nombre " << endl;
                 mostrar_mbr(path);
                cout << "  error xxx   [mkdisk]---- cree uno nuevo con un nombre distinto" << endl;
        }else{

                int tamano = stoi(size);
                if(stoi(size) > 0){
                    if(unit == "m"){
                        tamano = tamano * 1024 * 1024;
                    }else if(unit == "k"){
                          tamano = tamano * 1024;
                    }else if(unit == ""){
                        tamano = tamano * 1024 * 1024;
                        cout << tamano << endl;
                    }
                    estado = false;
                }else{
                    cout << "   error xxx  [mkdisk] ---- error el tamano del disco debe ser mayor a 0" << endl;
                }
                char is_fit;
                if(fit == "bf"){
                    is_fit = 'b';
                }else if(fit == "wf"){
                     is_fit = 'w';
                }else if(fit == "ff"){
                    is_fit = 'f';
                }else if(fit == ""){
                    is_fit = 'f';
                }
                // numero aleatorio para asig_disk
                srand(time(0));
                int randomNumber = rand() % 9999 + 100;
                if(estado == false){

                    Estructuras::DISCO mbr;
                    mbr.asig_disk = randomNumber;
                    mbr.fit_disk = is_fit;
                    mbr.size_disk = tamano;
                    mbr.time_disk = time(0);
                    mbr.mbr_Part_1 = Estructuras::Partition();
                    mbr.mbr_Part_2 = Estructuras::Partition();
                    mbr.mbr_Part_3 = Estructuras::Partition();
                    mbr.mbr_Part_4 = Estructuras::Partition();

                    FILE *archivo = fopen(path.c_str(), "wb");
                    fwrite("\0", 1, 1, archivo);
                    fseek(archivo, tamano-1, SEEK_SET);
                    fwrite("\0", 1, 1, archivo);
                    rewind(archivo);
                    fwrite(&mbr, sizeof(Estructuras::DISCO), 1, archivo);
                    fclose(archivo);
                     cout << "  correcto ✓✓✓ [mkdisk] ----- disco creado" << endl;
                }
                mostrar_mbr(path);
                
        }
        
}
void AdminDisk::mostrar_mbr(string path){
    Estructuras::DISCO mbr;
    FILE *imprimir = fopen(path.c_str(), "r");
    if(imprimir!=NULL){
        fseek(imprimir, 0, SEEK_SET);
        fread(&mbr,sizeof(Estructuras::DISCO), 1,imprimir);
        struct tm *now = localtime(&mbr.time_disk);
        string fecha;
        string hora;
        string FomatoFH;
        fecha = to_string(now->tm_mday) + "-" + to_string((now->tm_mon + 1)) + "-" + to_string((now->tm_year + 1900));
        hora = to_string( now->tm_hour) + ":" + to_string(now->tm_min) + ":" + to_string(now->tm_sec);
        FomatoFH = fecha + ' ' + hora;
        
        cout << "--------------------MBR--------------------" << endl;
        cout << "--------  " << "size_disk:         " << mbr.size_disk << " bytes"<< endl;
        cout << "--------  " << "fecha_creacion:    " << FomatoFH << endl;
        cout << "--------  " << "tipo de ajuste:    " << mbr.fit_disk << endl;
        cout << "--------  " << "numero_asignacion: " << mbr.asig_disk << endl;
        cout << "--------  " << "tamano MBR:        "  << sizeof(Estructuras::DISCO)<< " bytes"<< endl;
        cout << "--------  " << "tamano MBR:        "  << sizeof(mbr)<< " bytes"<< endl;
                  
    }
    fclose(imprimir);


}

void AdminDisk::CrearParticion(DATOS_PASS::FDISK fdisk){


// limpiar si trae comillas
string path = fdisk.path;
for (int i = 0; i < path.length(); i++){
        if(path[i] == '"'){
            path.erase(i,1);
        }
} 


int size_bytes = 0;
if(stoi(fdisk.size) > 0){
    
    if(fdisk.unit == "m"){
        size_bytes = stoi(fdisk.size)*1024*1024;
    }else if (fdisk.unit == "k"){
        size_bytes = stoi(fdisk.size)*1024;
    }else if (fdisk.unit == "b"){
        size_bytes = stoi(fdisk.size);
    }else{
        size_bytes = stoi(fdisk.size)*1024;
    }

    if(file_existe(path) == true){
        // leer el mbr 
        Estructuras::DISCO Disco;
        FILE *ExtraerMBR = fopen(path.c_str(), "rb+");
        rewind(ExtraerMBR );
        fread(&Disco, sizeof(Estructuras::DISCO), 1, ExtraerMBR);
        fclose(ExtraerMBR);

        vector <Estructuras::Partition> List_particion = List_partition(Disco);
        Estructuras::Partition Nueva_particion;

        Nueva_particion.part_fit = fdisk.fit[0];
        Nueva_particion.part_size = size_bytes;
        Nueva_particion.part_status = '1';
        Nueva_particion.part_type = fdisk.type[0];
        strcpy( Nueva_particion.part_name , fdisk.name.c_str());

        controler(List_particion);

        Estructuras::Partition Nueva_particion;
        Estructuras::DISCO Disk_Particion;
        Disk_Particion = particiones(Disco,Nueva_particion);

        if(fdisk.type == "e"){


        }else{

        }

         

        FILE *bfile = fopen(path.c_str(), "rb+"); 
        fseek(bfile, 0, SEEK_SET);
        fwrite(&Disk_Particion, sizeof(Estructuras::DISCO), 1, bfile);
        fclose(bfile);
        cout << "particion creada" << endl;

      

    }else{ cout << "    [fdisk] error no existe el disco cree uno con el comando mkdisk" << endl; }
    
}else{ cout << "     [fdisk] el tamano tiene que ser mayor a 0" << endl; }
// obtner el tamano de la particion 
    

}


void AdminDisk::controler(vector<Estructuras::Partition> list_new ){
    Transicion trans;
    for (int i = 0; i < list_new.size(); i++){
        if(list_new[i].part_status == '1'){
            trans.status = '1';
            trans.num_particion = inicio;
        }
    }
    


}




Estructuras::DISCO AdminDisk::particiones(Estructuras::DISCO mbr,Estructuras::Partition particion){

  if(inicio == 0){
     particion.part_start = sizeof(mbr);
     mbr.mbr_Part_1 = particion;

  }else{

   


  }
    
    return mbr;
      

}


vector<Estructuras::Partition> AdminDisk::List_partition(Estructuras::DISCO mbr){
    vector <Estructuras::Partition> List_new;
    List_new.push_back(mbr.mbr_Part_1);
    List_new.push_back(mbr.mbr_Part_2);
    List_new.push_back(mbr.mbr_Part_3);
    List_new.push_back(mbr.mbr_Part_4);

    return List_new;

}





void AdminDisk::eliminar_disk(string path){
    for (int i = 0; i < path.length(); i++){
              if(path[i] == '"'){
                  path.erase(i,1);
              }
   } 

    if (remove(path.c_str()) == 0) {
          cout << "     [mkdisk] Disco eliminado" << endl;
    }else{
          cout << "     [mkdisk] No hay disco que eliminar" << endl;
    }
}

string AdminDisk::obtner_rutaCarpeta(string path){       
        string wordToCount = "/";
        int cant_carpeta = count(path.begin(), path.end(), wordToCount[0]); 
        int cap = 0;
        int index = 0;
        string temp = "";
        while (index < path.length()){
            if(path[index] == '/'){
                 if (cap == (cant_carpeta - 1)){
                        temp += '/';
                        break;
                 }
                 cap +=  1;
            }
            temp += path[index]; 
            index += 1;
        }
    return temp;

}

bool AdminDisk::ver_existeCarpeta(string path){
    
        DIR* directorio = opendir(path.c_str());
            if (directorio) {
                closedir(directorio);
                return true;
            } else {
                return false;
            }
}

bool AdminDisk::file_existe(string documento){
    ifstream f(documento.c_str());

    if(f.good()== 0){
        return false;
    }else{
        return true;
    }
}