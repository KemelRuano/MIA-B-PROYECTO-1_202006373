#include "../HeaderFiles/AdminUG.h"
#include "../HeaderFiles/Class_Estruct.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include "string"

void User_Group::Login(string us , string pass ,string id,AdminDisk Disk){
   admindk = Disk;
   Estructuras::Superblock superbloque;
   Estructuras::Fileblock fileblock;
   Estructuras::Partition particion;
   if(estado == false){
         estado = true;
   }else{
        cout << "El usuario " << logeado.user << " esta activo espere a que cierre sesion" << endl;
        return;
   }
   try{
        string user = admindk.LimpiarString(us);
        string password = admindk.LimpiarString(pass);
        string Mount_id = admindk.LimpiarString(id);
        string path;
        particion = admindk.EncontrarPartition(id,&path);
         
        FILE *readfiles = fopen(path.c_str(), "rb");
        fseek(readfiles,particion.part_start,SEEK_SET);
        fread(&superbloque,sizeof(Estructuras::Superblock), 1 , readfiles);

        fseek(readfiles,superbloque.s_block_start + sizeof(Estructuras::Folderblock),SEEK_SET);
        fread(&fileblock,sizeof(Estructuras::Fileblock),1,readfiles);
        fclose(readfiles);

        string archivos;
        archivos += fileblock.b_content;
        vector <string> list_date = extraer(archivos,'\n');
        bool encontrado = false;
        for(int x = 0; x < list_date.size(); x++){
               if(list_date[x][2] == 'U' || list_date[x][2]  == 'u'){
                       vector<string> Users = extraer(list_date[x],',');
                       for(int i = 0; i < Users.size(); i++){
                              if(Users[3] == user && Users[4] == password){
                                 encontrado = true;
                                 logeado.id = Mount_id;
                                 logeado.password = password;
                                 logeado.user = user;
                                 logeado.uid = stoi(Users[0]);
                                 cout << "[login] login correcto" << endl;
                                 if(user == "root" && password == "123"){
                                    cout << "Bienvendio Admin ->  " << user << endl;
                                 }else{
                                     cout << "Bienvendio  ->  " << user << endl;
                                 }
                                 break;
                              }
                       }

               }
        }
        if(!encontrado){
             cout << "[login] --- El usuario no existe " << endl;
             return;
        }
   }catch(exception &e){
    cout << e.what() << endl;
   }
  

}

vector<string> User_Group::extraer(string txt,char tab){
   vector<string> list_user;
   stringstream ss(txt);
   string word;
   while (getline(ss, word, tab)) { 
        list_user.push_back(word);  
   }
   return list_user;
}

void User_Group::cerrarsesion(){
   if(logeado.user != ""){
       if(logeado.user == "root"){
        cout << "[logout] --- Nos vemos luego Admin ->  " << logeado.user << endl; 
      }else{
         cout << "[logout] --- Nos vemos luego ->  " << logeado.user << endl; 
      }

      logeado = User();
      estado = false;
   
   }else{
        cout << "[logout]--- Error inicie sesion primero " << endl;
   }
}

void User_Group::mkgrp(string name){
   Estructuras::Superblock superbloque;
   Estructuras::Fileblock fileblock;
   Estructuras::Partition particion;

   try{

      if(!(logeado.user == "root" && logeado.password == "123" && estado == true)){
          cout << "[mkgrp] --- Solo el usuario root puede realizar esta operacion" << endl;
          return; 
      }
      string name_grp = admindk.LimpiarString(name);
      string path;
      particion = admindk.EncontrarPartition(logeado.id,&path);
      
      FILE *readfiles = fopen(path.c_str(), "rb+");
      fseek(readfiles,particion.part_start,SEEK_SET);
      fread(&superbloque,sizeof(Estructuras::Superblock), 1 , readfiles);

      fseek(readfiles,superbloque.s_block_start + sizeof(Estructuras::Folderblock),SEEK_SET);
      fread(&fileblock,sizeof(Estructuras::Fileblock),1,readfiles);

      string archivos;
      archivos += fileblock.b_content;
      vector <string> list_date = extraer(archivos,'\n');
      int cont_grp = 0;
      bool encontrado = false;
      bool reanimado = false;
      string envia;
      for(int x = 0; x < list_date.size(); x++){
            if(list_date[x][2] == 'G' || list_date[x][2]  == 'g'){
                     cont_grp++;
                     vector<string> Users = extraer(list_date[x],',');
                     if(Users[0] != "0" && Users[2] == name_grp){
                           encontrado = true;
                     }else if(Users[0] == "0" && Users[2] == name_grp){
                              reanimado = true;
                              envia = list_date[x];
                              break;
                     }
            }
      }

      if(reanimado){
             cout << archivos << endl;
             string new_archivos;
            for(int x = 0; x < list_date.size(); x++){
                  if(list_date[x] == envia){
                    new_archivos += to_string(cont_grp) + ",G," + name_grp + "\n";
                  }else{
                        new_archivos += list_date[x] + '\n';
                  }

            }
            cout << "[mkgrp]---- Grupo creado exitosamente" << endl;
            strcpy(fileblock.b_content , new_archivos.c_str());
            fseek(readfiles , superbloque.s_block_start + sizeof(Estructuras::Folderblock), SEEK_SET);
            fwrite(&fileblock, sizeof(Estructuras::Fileblock), 1, readfiles);
            fclose(readfiles);

            return;
      }
      if(!encontrado){
               archivos += to_string(cont_grp+1) + ",G," + name_grp + "\n";
               strcpy(fileblock.b_content , archivos.c_str());
               fseek(readfiles , superbloque.s_block_start + sizeof(Estructuras::Folderblock), SEEK_SET);
               fwrite(&fileblock, sizeof(Estructuras::Fileblock), 1, readfiles);
               cout << "[mkgrp]---- Grupo creado exitosamente" << endl;
      }else{
         cout << "[mkgrp]--- No se creo el grupo porque ya existe" << endl;
      }

      cout << fileblock.b_content << endl;

      fclose(readfiles);



   }catch(exception &e){
      cout << e.what() << endl;
   }
}


void User_Group::rmgrp(string name){

   Estructuras::Superblock superbloque;
   Estructuras::Fileblock fileblock;
   Estructuras::Partition particion;
   try{

      if(!(logeado.user == "root" && logeado.password == "123" && estado == true)){
          cout << "[mkgrp] --- Solo el usuario root puede realizar esta operacion" << endl;
          return; 
      }
      string name_grp = admindk.LimpiarString(name);
      string path;
      particion = admindk.EncontrarPartition(logeado.id,&path);
      
      FILE *readfiles = fopen(path.c_str(), "rb+");
      fseek(readfiles,particion.part_start,SEEK_SET);
      fread(&superbloque,sizeof(Estructuras::Superblock), 1 , readfiles);

      fseek(readfiles,superbloque.s_block_start + sizeof(Estructuras::Folderblock),SEEK_SET);
      fread(&fileblock,sizeof(Estructuras::Fileblock),1,readfiles);
      
      string archivos;
      archivos += fileblock.b_content;

      vector <string> list_date = extraer(archivos,'\n');
      archivos = "";
      bool encontrado = false;
      for(int x = 0; x < list_date.size(); x++){
            if(list_date[x][0] != '0' && (list_date[x][2] == 'G' || list_date[x][2]  == 'g')){
                     vector<string> Users = extraer(list_date[x],',');
                     for (int i = 0; i < Users.size(); i++){
                           if(Users[2] == name_grp){
                               encontrado = true;
                               list_date[x][0] = '0';
                               break;
                           }
                     }
            }
            archivos += list_date[x] + "\n";
      }

      if(encontrado){
                  strcpy(fileblock.b_content , archivos.c_str());
                  fseek(readfiles , superbloque.s_block_start + sizeof(Estructuras::Folderblock), SEEK_SET);
                  fwrite(&fileblock, sizeof(Estructuras::Fileblock), 1, readfiles);
                  cout << "[rmgrp]--- Grupo eliminado correctamente" << endl;
      }else{
         cout << "[rmgrp]--- El grupo no existe" << endl;
      }
      fclose(readfiles);
     
   }catch(exception &e){
      cout << e.what() << endl;
   }
}

void User_Group::mkuser(string userss,string password,string grupo){
   Estructuras::Superblock superbloque;
   Estructuras::Fileblock fileblock;
   Estructuras::Partition particion;
   try{
      if(!(logeado.user == "root" && logeado.password == "123" && estado == true)){
          cout << "[mkgrp] --- Solo el usuario root puede realizar esta operacion" << endl;
          return; 
      }
      string user = admindk.LimpiarString(userss);
      string pass = admindk.LimpiarString(password);
      string grup = admindk.LimpiarString(grupo);

      string path;
      particion = admindk.EncontrarPartition(logeado.id,&path);
      FILE *readfiles = fopen(path.c_str(), "rb+");
      fseek(readfiles,particion.part_start,SEEK_SET);
      fread(&superbloque,sizeof(Estructuras::Superblock), 1 , readfiles);

      fseek(readfiles,superbloque.s_block_start + sizeof(Estructuras::Folderblock),SEEK_SET);
      fread(&fileblock,sizeof(Estructuras::Fileblock),1,readfiles);
    
      string archivos;
      archivos += fileblock.b_content;
      cout << archivos << endl;
      bool validacion = false;
      vector <string> list_date = extraer(archivos,'\n');
      int cont = 0;
      for(int x = 0; x < list_date.size(); x++){
            if(list_date[x][2] == 'g' || list_date[x][2] == 'G'){
                  vector<string> Users = extraer(list_date[x],',');
                  if(Users[2] == grup){
                     validacion = true;
                  }
            }
            if(list_date[x][0] != '0' && (list_date[x][2] == 'u' || list_date[x][2]  == 'U')){
                     cont++;
                     vector<string> Userss = extraer(list_date[x],',');
                     if(Userss[3] == user){
                              cout << "[mkuser] --- usuario: " << user << " ya existe" << endl;
                              return;  
                     }  
            }
                     
      }

      if(validacion == false){
         cout << "[mkusr] --- El grupo: " << grup <<" no existe." << endl;
         return;
      }
       
      archivos += to_string(cont+1) + ",U," + grup +","+ user + "," + pass+ "\n";

      strcpy(fileblock.b_content , archivos.c_str());
      fseek(readfiles , superbloque.s_block_start + sizeof(Estructuras::Folderblock), SEEK_SET);
      fwrite(&fileblock, sizeof(Estructuras::Fileblock), 1, readfiles);
      cout << fileblock.b_content << endl;
      fclose(readfiles);
      cout << "[mkusr]---- Usuario nuevo creado" << endl;


   }catch(exception &e){
      cout << e.what() << endl;
   }



}


void User_Group::rmuser(string usr){
   Estructuras::Superblock superbloque;
   Estructuras::Fileblock fileblock;
   Estructuras::Partition particion;

   try{

      

      if(!(logeado.user == "root" && logeado.password == "123" && estado == true)){
          cout << "[mkgrp] --- Solo el usuario root puede realizar esta operacion" << endl;
          return; 
      }
      string user = admindk.LimpiarString(usr);
      string path;
      particion = admindk.EncontrarPartition(logeado.id,&path);
      
      FILE *readfiles = fopen(path.c_str(), "rb+");
      fseek(readfiles,particion.part_start,SEEK_SET);
      fread(&superbloque,sizeof(Estructuras::Superblock), 1 , readfiles);

      fseek(readfiles,superbloque.s_block_start + sizeof(Estructuras::Folderblock),SEEK_SET);
      fread(&fileblock,sizeof(Estructuras::Fileblock),1,readfiles);
      
      string archivos;
      archivos += fileblock.b_content;

      vector <string> list_date = extraer(archivos,'\n');
      archivos = "";
      bool encontrado = false;
      for(int x = 0; x < list_date.size(); x++){
            if(list_date[x][0] != '0' && (list_date[x][2] == 'U' || list_date[x][2]  == 'u')){
                     vector<string> Users = extraer(list_date[x],',');
                     if(Users[3] == user){
                           encontrado = true;
                           list_date[x][0] = '0';
                     }
               
            }
            archivos += list_date[x] + "\n";
      }

      cout << archivos << endl;
      if(encontrado){
                  strcpy(fileblock.b_content , archivos.c_str());
                  fseek(readfiles , superbloque.s_block_start + sizeof(Estructuras::Folderblock), SEEK_SET);
                  fwrite(&fileblock, sizeof(Estructuras::Fileblock), 1, readfiles);
                  cout << "[rmgrp]--- Grupo eliminado correctamente" << endl;
      }else{
         cout << "[rmgrp]--- El grupo no existe" << endl;
      }
      fclose(readfiles);
   }catch(exception &e){
      cout << e.what() << endl;
   }



}