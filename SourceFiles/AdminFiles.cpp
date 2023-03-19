
#include "../HeaderFiles/AdminFiles.h"
#include "../HeaderFiles/AdminDisk.h"
#include "../HeaderFiles/AdminUG.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include "string"

void AdminFiles::passDate(AdminDisk disk,User_Group user){
    admindisk = disk;
    user_group = user;
}
vector <string> AdminFiles::extraerCarpetas(string carpetas){  
   vector<string> list_user;
   stringstream ss(carpetas);
   string word;
   string temp;
   while (getline(ss, word,'/')) {
        if (word.find(' ') != std::string::npos) {
                 temp = '"'+ word+'"';
                 list_user.push_back(temp);
                 temp = "";
        }else if(word != ""){
                   list_user.push_back(word);  
        }
  }
     
        
   return list_user;


}
int AdminFiles::Inodosiguiente(Estructuras::Superblock superbloque,string path){
  

    FILE *Leer_modificar = fopen(path.c_str(), "rb+");
    char bitMapInodo[superbloque.s_inodes_count];
    fseek(Leer_modificar,superbloque.s_bm_inode_start,SEEK_SET);
    fread(&bitMapInodo, sizeof(bitMapInodo), 1, Leer_modificar);
    for(int x = 0; x < superbloque.s_inodes_count; x++){
            if(bitMapInodo[x] == '0'){
                    fclose(Leer_modificar);
                    return x;
            }
    }
    return -1;
}

int AdminFiles::Bloquesiguiente(Estructuras::Superblock superbloque,string path){

    FILE *Leer_modificar = fopen(path.c_str(), "rb+");
   
    char bitMapInodo[superbloque.s_blocks_count];
    fseek(Leer_modificar, superbloque.s_bm_block_start,SEEK_SET);
    fread(&bitMapInodo, sizeof(bitMapInodo), 1, Leer_modificar);
    for(int x = 0; x < superbloque.s_blocks_count; x++){
            if(bitMapInodo[x] == '0'){
                    fclose(Leer_modificar);
                    return x;
            }
    }
    return -1;

}

void AdminFiles::ActualizarBitMaps(Estructuras::Superblock superbloque,string path, string tipo){

    FILE *Leer_modificar = fopen(path.c_str(), "rb+");
    int numero_inodo = 0;
    int numero_bloque = 0;
    char valor = '1';
    if(tipo == "I"){
        char bitMapInodo[superbloque.s_inodes_count];
        fseek(Leer_modificar,superbloque.s_bm_inode_start,SEEK_SET);
        fread(&bitMapInodo, sizeof(bitMapInodo), 1, Leer_modificar);
        for(int x = 0; x < superbloque.s_inodes_count; x++){
               if(bitMapInodo[x] == '0'){
                     numero_inodo = x;
                     break;
               }
        }
        fseek(Leer_modificar,superbloque.s_bm_inode_start + numero_inodo,SEEK_SET);
        fwrite(&valor,sizeof(valor),1,Leer_modificar);
    }else{
      cout << superbloque.s_blocks_count << endl;
      char bitMapbloque[superbloque.s_blocks_count];
      fseek(Leer_modificar,superbloque.s_bm_block_start,SEEK_SET);
      fread(&bitMapbloque, sizeof(bitMapbloque), 1, Leer_modificar);
      for(int x = 0; x < superbloque.s_blocks_count; x++){
               if(bitMapbloque[x] == '0'){
                     numero_bloque = x;
                     break;
               }
      }
      fseek(Leer_modificar,superbloque.s_bm_block_start +  numero_bloque,SEEK_SET);
      fwrite(&valor,sizeof(valor),1,Leer_modificar);
    }
   
    fclose(Leer_modificar);

}

void AdminFiles::ActualizarBloque(Estructuras::Superblock superbloque,string path,string nombre,int numero){

    Estructuras::Folderblock  bloque_de_carpetas;
    FILE *Leer_modificar = fopen(path.c_str(), "rb+");
    bloque_de_carpetas = Estructuras::Folderblock();
    fseek(Leer_modificar,superbloque.s_block_start +(sizeof(Estructuras::Folderblock)*numero),SEEK_SET);
    fread(&bloque_de_carpetas,sizeof(Estructuras::Folderblock),1,Leer_modificar);
    for(int z = 0; z < 4; z++){
        cout << "entro" << endl;
        if(bloque_de_carpetas.b_content[z].b_inodo == -1 ){
            strcpy(bloque_de_carpetas.b_content[z].b_name, nombre.c_str());
            bloque_de_carpetas.b_content[z].b_inodo = Inodosiguiente(superbloque,path);
            fseek(Leer_modificar,superbloque.s_block_start +(sizeof(Estructuras::Folderblock)*numero),SEEK_SET);
            fwrite(&bloque_de_carpetas,sizeof(bloque_de_carpetas),1,Leer_modificar);
            fclose(Leer_modificar);
            break;
        }
    }

}


// void AdminFiles::CREATEINODO(){



// }

void AdminFiles::MKDIR(vector<string> lista_carpeta,string c){
    Estructuras::Partition particion;
    Estructuras::Superblock superbloque;
    Estructuras::Folderblock bloque_de_carpetas;
    Estructuras::Inodes inodo;
    try{
           if(user_group.logeado.user == "" && user_group.logeado.password == ""){
                cout << endl << "[mkdir]--- debe de logearse antes" << endl;
                return;
            }

            string creadas = admindisk.LimpiarString(c);

            string path;
            particion = admindisk.EncontrarPartition(user_group.logeado.id,&path);


            FILE *Leer_modificar = fopen(path.c_str(), "rb+");
            // Leer el super bloque
            fseek(Leer_modificar,particion.part_start,SEEK_SET);
            fread(&superbloque,sizeof(Estructuras::Superblock),1,Leer_modificar);
            int total = superbloque.s_inodes_count;
            //Leer los bitmap de inodos
            char bitmapsInode[total];
            fseek(Leer_modificar,superbloque.s_bm_inode_start, SEEK_SET);
            fread(&bitmapsInode,sizeof(bitmapsInode),1,Leer_modificar);
            // LEO EL BITMAP DE INODOS PARA SABER LA CANTIDAD DE INODOS DISPONIBLES
            string padre = lista_carpeta.front(); 
            lista_carpeta.erase(lista_carpeta.begin());
            
           // 1. Fase validar si existe la carpeta padre 
            bool existe = false;
            int Bloqueactual = 0;
            for(int x = 0; x < total; x++){
                if(bitmapsInode[x] == '1'){
                        inodo = Estructuras::Inodes();
                        fseek(Leer_modificar,superbloque.s_inode_start + ( sizeof(Estructuras::Inodes) * x),SEEK_SET);
                        fread(&inodo,sizeof(Estructuras::Inodes),1,Leer_modificar);
                        if(inodo.i_type == '0'){
                            for(int y = 0; y < 16; y++){
                                if(y < 13){
                                    if(inodo.i_block[y] != -1){
                                        Bloqueactual  = inodo.i_block[y];
                                        bloque_de_carpetas = Estructuras::Folderblock();
                                        fseek(Leer_modificar,superbloque.s_block_start + ( sizeof(Estructuras::Folderblock) * inodo.i_block[y]),SEEK_SET);
                                        fread(&bloque_de_carpetas,sizeof(Estructuras::Folderblock),1,Leer_modificar);
                                        for(int z = 0; z < 4; z++){
                                            if(bloque_de_carpetas.b_content[z].b_name == padre){
                                                        existe = true;
                                                        break;
                                            }
                                        }

                                    }

                                }
                               
                               
                            }
                        }
                }
            }
            fclose(Leer_modificar);


            if(existe){
                MKDIR(lista_carpeta,creadas);

            }
            

            
            
            


   
 


    }catch(exception &e){
        cout << e.what() << endl;
    }
 
}


