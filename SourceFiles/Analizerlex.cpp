#include "../HeaderFiles/Analizerlex.h"
#include "../HeaderFiles/Tokens.h"
#include "../HeaderFiles/AdminDisk.h"
#include "../HeaderFiles/pass_struct.h"
#include "../HeaderFiles/AdminUG.h"
#include "../HeaderFiles/reportes.h"
#include "../HeaderFiles/AdminFiles.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

vector<Tokens> Analizerlex::AnalizarLex(string cadena){

    int estado = 0;
    int index = 0;
    vector <Tokens> list_Tokens;
    string lexema = "";
    vector <string> List_reservada = {"mkdisk","rmdisk","fdisk","mount","unmount","mkfs","login","logout","mkgrp","rmgrp","mkusr","rmusr","rep","mkdir","pause"};
    vector <string> List_atrib = {">size=",">path=",">fit=",">unit=",">name=",">type=",">delete=",">add=",">id=",">fs=",">user=",">pass=",">grp=",">ruta=",">r"};
   
   
   
    while(index < cadena.length()){
           char temp = cadena[index];
           if(estado == 0){
                    if(temp == '#'){
                        estado = 1;          
                    }else if(temp >= char(97) && temp <= char(122)){
                         estado = 2;
                    }else if (temp == '>'){
                        estado = 3;
                        index += 1;
                        lexema += temp;
                    }else if(temp >= char(48) && temp <= char(57) || temp == char(45)){
                        estado = 4;
                    }else if(temp == '/'){
                        estado = 5;
                        index += 1;
                        lexema += temp;
                    }else if(temp == '"'){
                        estado = 6;
                        index += 1;
                        lexema += temp;
                    
                    }else if(temp == char(32)){
                        index += 1;
                    }else{
                        cout << "error de sintaxis" << endl;
                    }

           }else if(estado == 1){
                  if(temp != '\n'){
                    index += 1;
                  }
                  if(temp == '\n'){
                        index += 1;
                        estado = 0;
                        index += 1;
                  }
           }else if(estado == 2){
                    if(temp >= char(97) && temp <= char(122) || temp >= char(48) && temp <= char(57)){
                        index += 1;
                        lexema += temp;
                    }else{
                        if(find(List_reservada.begin(), List_reservada.end(), lexema) != List_reservada.end()){
                                    list_Tokens.push_back(Tokens("RESERVADA",lexema));

                        }else if(lexema == "bf" || lexema == "wf" || lexema == "ff"){

                                list_Tokens.push_back(Tokens("TAJUSTE",lexema));

                        }else if(lexema == "m" || lexema == "k" || lexema == "b"){

                            list_Tokens.push_back(Tokens("UNIDAD",lexema));      

                        }else if(lexema == "p" || lexema == "e" || lexema == "l"){
                             list_Tokens.push_back(Tokens("TYPE",lexema));
                        }else{
                            list_Tokens.push_back(Tokens("ID",lexema));
                        }
                        
                        lexema = "";
                        estado = 0;
                    }
                    
           }else if(estado == 3){
                    if(temp >= char(97) && temp <= char(122) || temp == char(61)){
                        lexema += temp;
                        index += 1;
                        if(find(List_atrib.begin(), List_atrib.end(), lexema) != List_atrib.end()){  
                                
                                    list_Tokens.push_back(Tokens("ATRIBUTO",lexema));
                                    lexema = "";
                                    estado = 0;
                        }
                    }


           }else if(estado == 4){
                  if(temp >= char(48) && temp <= char(57) || temp == char(45)){
                       lexema += temp;
                       index += 1;
                  }else{
                        
                        list_Tokens.push_back(Tokens("NUMERO",lexema));
                        lexema = "";
                        estado = 0;

                  }

                  

           }else if(estado == 5){
                    if(temp >= char(46) && temp <= char(57) || temp >= char(97) && temp <= char(122) || temp == char(95) || temp >= char(65) && temp <= char(90)){
                            index += 1;
                            lexema += temp;
                    }else{
                            list_Tokens.push_back(Tokens("RUTASINESP",lexema));
                            lexema = "";
                            estado = 0;
                    }


           }else if(estado == 6){
                if(temp != '"'){
                    index += 1;
                    lexema += temp;
                }
                if(temp == '"'){
                        index += 1;
                        lexema += temp;
                        list_Tokens.push_back(Tokens("STRING",lexema));
                        lexema = "";
                        estado = 0;

                }
           }
    }


    
    return list_Tokens;
}

AdminDisk Disk;
User_Group usergroup;
reportes report;
AdminFiles adminfiles;
void Analizerlex::ejecutar_comannd(vector<Tokens> List_token,string cadena){
    // indica que tipo de comando se esta ejecutando
    bool mkdisk = false;
    bool rmdisk = false;
    bool fdisk = false;
    bool mount = false;
    bool unmount = false;
    bool mkfs = false;
    bool login = false;
    bool logoutt = false;
    bool mkgrp = false;
    bool rmgrp = false;
    bool mkusr = false;
    bool rmusr = false;
    bool rep = false;
    bool mkdir = false;
    DATOS_PASS::MKDISK Mkdisk;
    DATOS_PASS::FDISK Fdisk;
    DATOS_PASS::RMDISK Rmdisk;
    DATOS_PASS::MKFS Mkfs;
    DATOS_PASS::Login Log;
    DATOS_PASS::MKGRP Mkgrp;
    DATOS_PASS::RMGRP Rmgrp;
    DATOS_PASS::MKUSR Mkusr;
    DATOS_PASS::REP  reporte;

    string mkpath = "";
    string mkr = "";
    
          //ver que tipo de comando es 

            for(int i = 0; i < List_token.size(); i++){
                    if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "mkdisk"){
                            mkdisk = true;

                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "rmdisk"){
                            rmdisk = true;

                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "fdisk"){
                            fdisk = true;
                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "mount"){
                            mount = true;
                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "mkfs"){
                            mkfs = true;
                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "unmount"){
                            unmount = true;
                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "login"){
                            login = true;
                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "logout"){
                            logoutt = true;
                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "mkgrp"){
                            mkgrp  = true;
                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "rmgrp"){
                             rmgrp   = true;
                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "mkusr"){
                             mkusr   = true;
                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "rmusr"){
                             rmusr = true;
                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "rep"){
                             rep = true;

                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "mkdir"){
                           mkdir = true;
                    }else if (List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "pause"){
                            system("clear");
                            cout << endl;
                            cout << "████████   Presiona  tecla ENTER para continuar..." << endl;
                            cout << endl;
                            cin.get();
                    }else if (List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "execute"){
                             
                    }
  
   
                   
            }
            // asignar valor
            for (int i = 0; i < List_token.size(); i++){
                if(mkdisk == true){
                                if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">path=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "RUTASINESP" || List_token[i+1].getToken() == "STRING")){
                                                    Mkdisk.path = List_token[i+1].getLexema();
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">unit=" && i+1 < List_token.size() && List_token[i+1].getToken() == "UNIDAD"){
                                                    Mkdisk.unit = List_token[i+1].getLexema();    
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">size=" && i+1 < List_token.size() && List_token[i+1].getToken() == "NUMERO"){
                                                    Mkdisk.size =  List_token[i+1].getLexema();
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">fit=" && i+1 < List_token.size() && List_token[i+1].getToken() == "TAJUSTE"){
                                                    Mkdisk.fit =  List_token[i+1].getLexema();                   
                                }
                }else if(rmdisk == true){
                            if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">path=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "RUTASINESP" || List_token[i+1].getToken() == "STRING")){
                                                    Rmdisk.path = List_token[i+1].getLexema();
                            }

                }else if(fdisk == true){
                                if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">path=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "RUTASINESP" || List_token[i+1].getToken() == "STRING")){
                                                    Fdisk.path = List_token[i+1].getLexema();
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">unit=" && i+1 < List_token.size() && List_token[i+1].getToken() == "UNIDAD"){
                                                    Fdisk.unit = List_token[i+1].getLexema();    
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">size=" && i+1 < List_token.size() && List_token[i+1].getToken() == "NUMERO"){
                                                    Fdisk.size =  List_token[i+1].getLexema();
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">fit=" && i+1 < List_token.size() && List_token[i+1].getToken() == "TAJUSTE"){
                                                    Fdisk.fit =  List_token[i+1].getLexema();                   
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">name=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Fdisk.name =  List_token[i+1].getLexema();                   
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">type=" && i+1 < List_token.size() && List_token[i+1].getToken() == "TYPE"){
                                                    Fdisk.type =  List_token[i+1].getLexema();                   
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">add=" && i+1 < List_token.size() && List_token[i+1].getToken() == "NUMERO"){
                                                    Fdisk.add =  List_token[i+1].getLexema();                  
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">delete=" && i+1 < List_token.size() && List_token[i+1].getToken() == "ID"){
                                                    Fdisk.delit =  List_token[i+1].getLexema();                 
                                }

                }else if(mount == true){
                    if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">path=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "RUTASINESP" || List_token[i+1].getToken() == "STRING")){
                                                    Fdisk.path = List_token[i+1].getLexema();
                    }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">name=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Fdisk.name =  List_token[i+1].getLexema();                   
                    }

                }else if(unmount == true){
                    if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">id=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Fdisk.name = List_token[i+1].getLexema();
                    }

                }else if(mkfs == true){
                    if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">type=" && i+1 < List_token.size() && List_token[i+1].getToken() == "ID"){
                                                    Mkfs.type =  List_token[i+1].getLexema();                   
                    }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">id=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Mkfs.id = List_token[i+1].getLexema();

                    }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">fs=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Mkfs.fs = List_token[i+1].getLexema();
                    }
                     

                }else if(login == true){
                         
                    if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">user=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Log.user =  List_token[i+1].getLexema();                   
                    }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">pass=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Log.pass = List_token[i+1].getLexema();

                    }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">id=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Log.id = List_token[i+1].getLexema();
                    }

                }else if (mkgrp == true){
                    if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">name=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                             Mkgrp.reservada =  List_token[i+1].getLexema();                   
                    }


                }else if(rmgrp == true){
                    if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">name=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                             Rmgrp.reservada =  List_token[i+1].getLexema();                   
                    }

                }else if(mkusr == true){

                    if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">user=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Mkusr.user =  List_token[i+1].getLexema();                   
                    }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">pass=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Mkusr.pass = List_token[i+1].getLexema();

                    }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">grp=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Mkusr.grupo = List_token[i+1].getLexema();
                    }

                }else if( rmusr == true){
                      
                    if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">user=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                    Mkusr.user =  List_token[i+1].getLexema();                   
                    }
                }else if(rep == true){
                    if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">name=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                             reporte.name=  List_token[i+1].getLexema();                   
                    }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">id=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "ID" || List_token[i+1].getToken() == "STRING")){
                                                reporte.id = List_token[i+1].getLexema();
                    }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">path=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "RUTASINESP" || List_token[i+1].getToken() == "STRING")){
                                                    reporte.path = List_token[i+1].getLexema();
                    }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">ruta=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "RUTASINESP" || List_token[i+1].getToken() == "STRING")){
                                                    reporte.ruta = List_token[i+1].getLexema();
                    }


                }else if(mkdir == true){
                         if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">path=" && i+1 < List_token.size() && (List_token[i+1].getToken() == "RUTASINESP" || List_token[i+1].getToken() == "STRING")){
                                                    mkpath = List_token[i+1].getLexema();
                         }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">r"){
                                 mkr = List_token[i].getLexema();
                         }
                }


            }

    bool unit = false;
    bool fit =  false;
    if(mkdisk == true){


       if (cadena.find(">path=") != string::npos && cadena.find(">size=") != string::npos) {
            if(Mkdisk.path != "" && Mkdisk.size !=""){
                 
                if(cadena.find(">fit=") != string::npos && Mkdisk.fit != ""){
                                fit = true;
                }
                
                if(cadena.find(">unit=") != string::npos && Mkdisk.unit != ""){ 
                               unit = true;   
                }

                if(unit == true && fit == true){     
                        Disk.crear_disk(Mkdisk.size,Mkdisk.path,Mkdisk.unit,Mkdisk.fit);
                }else if(unit == true && fit == false){
                        Disk.crear_disk(Mkdisk.size,Mkdisk.path,Mkdisk.unit,Mkdisk.fit);
                }else if(unit == false && fit == true){
                        Disk.crear_disk(Mkdisk.size,Mkdisk.path,Mkdisk.unit,Mkdisk.fit);
                }else if(unit == false && fit == false){
                        Disk.crear_disk(Mkdisk.size,Mkdisk.path,Mkdisk.unit,Mkdisk.fit);
                }

            }else{
               if(Mkdisk.path  == ""){cout << "     [mkdisk] >path= falta valor" << endl;}
               if(Mkdisk.size  == ""){cout << "     [mkdisk] >size= falta valor" << endl;}
            }           
       }else{
           if(cadena.find(">path=") == string::npos) cout <<"     [mkdisk] falto parametro ---- >path=" << endl; 
           if(cadena.find(">size=") == string::npos) cout <<"     [mkdisk] falto parametro ---- >size=" << endl;  
       }  

    }else if(rmdisk == true){

        if( cadena.substr(0, 6) == "rmdisk" && cadena.substr(7, 6) == ">path=" && cadena.substr(13,cadena.length()) != Rmdisk.path){
              Disk.eliminar_disk(Rmdisk.path);
        }else{
             if(cadena.find(">path=") == string::npos) cout <<"     [rmdisk] falto parametro ---- >path=" << endl; 
              if(Rmdisk.path == ""){cout << "     [rmkdisk] >path= falta valor" << endl;}
             cout << "comando rmdisk invalido" << endl;
        }

    }else if(fdisk == true){

        if(Fdisk.delit == "" && Fdisk.add == ""){
                if (cadena.find(">path=") != string::npos && Fdisk.path != "" && \
                    cadena.find(">size=") != string::npos && Fdisk.size != "" && \
                    cadena.find(">name=") != string::npos && Fdisk.name != "") {
                              Disk.CrearParticion(Fdisk);
            
                }else{
                    if(cadena.find(">path=") == string::npos) cout <<"     [fdisk] falto parametro ---- >path  " << endl; 
                    if(cadena.find(">size=") == string::npos) cout <<"     [fkdisk] falto parametro ---- >size " << endl;  
                    if(cadena.find(">name=") == string::npos) cout <<"     [fkdisk] falto parametro ---- >name "   << endl;
                    if(Fdisk.path == ""){ cout << "     [fdisk] >path falto valor" << endl; }
                    if(Fdisk.size == ""){ cout << "     [fdisk] >size falto valor" << endl; }
                    if(Fdisk.name == ""){ cout << "     [fdisk] >name falto valor" << endl; }

                }
        }else{
           if(Fdisk.delit != ""){
            if( cadena.find(">name=") != string::npos && Fdisk.name != "" && \
                cadena.find(">path=") != string::npos && Fdisk.path != "" && \
                 cadena.find(">delete=") != string::npos && Fdisk.delit != ""){
                       
                        Disk.EliminarParticion(Fdisk.delit,Fdisk.path,Fdisk.name);
                        
            }else{
                   if(cadena.find(">name=") == string::npos) cout <<"     [fdisk] falto parametro ---- >name  " << endl; 
                    if(cadena.find(">path=") == string::npos) cout <<"     [fkdisk] falto parametro ---- >path " << endl;  
                    if(cadena.find(">delete=") == string::npos) cout <<"     [fkdisk] falto parametro ---- >delete "   << endl;
                    if(Fdisk.path == ""){ cout << "     [fdisk] >path falto valor" << endl; }
                    if(Fdisk.delit == ""){ cout << "    [fdisk] >delete falto valor" << endl; }
                    if(Fdisk.name == ""){ cout << "     [fdisk] >name falto valor" << endl; }
                
            }

           }
            

        }

        if(Fdisk.add != ""){
            if( cadena.find(">name=") != string::npos && Fdisk.name != "" && \
                cadena.find(">path=") != string::npos && Fdisk.path != "" && \
                cadena.find(">add=") != string::npos && Fdisk.add != "" &&
                cadena.find(">unit=") != string::npos && Fdisk.unit != ""){
                       
                                Disk.addEspace(Fdisk.path,Fdisk.name,Fdisk.unit,Fdisk.add);
                        
            }else{

                    if(cadena.find(">name=") == string::npos) cout <<"     [fdisk] falto parametro ---- >name  " << endl; 
                    if(cadena.find(">path=") == string::npos) cout <<"     [fkdisk] falto parametro ---- >path " << endl;  
                    if( cadena.find(">unit=") == string::npos) cout <<"     [fkdisk] falto parametro ---- >unit"   << endl;
                    if(Fdisk.path == ""){ cout << "     [fdisk] >path falto valor" << endl; }
                    if(Fdisk.unit == ""){ cout << "     [fdisk] >unit falto valor" << endl; }
                    if(Fdisk.name == ""){ cout << "     [fdisk] >name falto valor" << endl; }


            }
        }
        
    }else if(mount == true){
            if(cadena.find(">name=") != string::npos && Fdisk.name != "" && cadena.find(">path=") != string::npos && Fdisk.path != ""){

                        Disk.mount(Fdisk.name,Fdisk.path);

            }else{
                   
                   if(cadena.find(">name=") == string::npos) cout <<"     [mount] falto parametro ---- >name  " << endl; 
                   if(cadena.find(">path=") == string::npos) cout <<"     [fkdisk] falto parametro ---- >path " << endl;  
                   if(Fdisk.path == ""){ cout << "     [mount >path falto valor" << endl; }
                   if(Fdisk.name == ""){ cout << "     [mount] >name falto valor" << endl; }
            } 

    }else if(unmount == true){
           if(cadena.find(">id=") != string::npos && Fdisk.name != ""){
                                Disk.unmount(Fdisk.name);
           }else{
               if(cadena.find(">id=") == string::npos) cout <<"     [mount] falto parametro ---- >id " << endl;  
               if(Fdisk.name == ""){ cout << "     [mount] >id falto valor" << endl; }
            }


    }else if(mkfs == true){

        if(cadena.find(">id=") != string::npos && Mkfs.id != ""){
                                Disk.mkfs(Mkfs.id,Mkfs.type,Mkfs.fs);
        }else{
               if(cadena.find(">id=") == string::npos) cout <<"    [mkfs] falto parametro ---- >id " << endl;  
               if(Mkfs.id == ""){ cout << "     [mkfs] >id falto valor" << endl; }
        }

    }else if(login == true){
        bool encon_id   = cadena.find(">id=") != string::npos;
        bool encon_user = cadena.find(">user=") != string::npos;
        bool encon_pass = cadena.find(">pass=") != string::npos;
        bool log_con    = cadena.find("login") != string::npos;
        if(log_con && encon_id && encon_user && encon_pass){
            if(!(Log.id == "" && Log.pass == "" && Log.user == "")){
                            usergroup.Login(Log.user,Log.pass,Log.id,Disk);
            }else{
                if(Log.id == ""){   cout << endl <<"[login] --- Falta valor para >id= "<< endl;}
                if(Log.user == ""){ cout << endl <<"[login] --- Falta valor para >user= "<< endl;}
                if(Log.pass == ""){   cout << endl <<"[login] --- Falta valor para >pass= "<< endl;}
            }
        }else if(encon_id && encon_user ){
                cout << endl <<"[login] --- Falta parametro >pass= " << endl;
                if(Log.id == ""){   cout << endl <<"[login] --- Falta valor para >id= "<< endl;}
                if(Log.user == ""){ cout << endl <<"[login] --- Falta valor para >user= "<< endl;}
        }else if(encon_user && encon_pass){
                cout << endl << "[login] --- Falta parametro >id= " << endl << endl;
                if(Log.pass == ""){   cout << endl <<"[login] --- Falta valor para >pass= "<< endl;}
                if(Log.user == ""){   cout << endl <<"[login] --- Falta valor para >user= "<< endl;}
        }else if(encon_id && encon_pass){
                cout<< endl << "[login] --- Falta parametro >user= " << endl << endl;
                if(Log.id == ""){   cout << endl <<"[login] --- Falta valor para >id= "<< endl;}
                if(Log.pass == ""){ cout << endl <<"[login] --- Falta valor para >pass= "<< endl;}
        }else if(log_con){
              cout << endl << "[login] --- Falta parametro >user= " << endl; 
              cout << endl << "[login] --- Falta parametro >pass= " << endl;
              cout << endl << "[login] --- Falta parametro >id=   " << endl;
              cout << endl;
        }
    }else if( logoutt == true){

             usergroup.cerrarsesion();
              
    }else if (mkgrp == true){

            bool encon_grp   = cadena.find(">name=") != string::npos;
            if(encon_grp){
                if(Mkgrp.reservada != ""){
                         usergroup.mkgrp(Mkgrp.reservada);
                }else{ cout << endl << "[mkgrp] --- Falta valor para >name= " << endl; }
            }else{  cout << endl << "[mkgrp] --- Falta parametro >name= " << endl; }

    }else if(rmgrp == true){
            bool encon_rmgrp = cadena.find(">name=") != string::npos;
            bool rmgrp1      = cadena.find("rmgrp") != string::npos;
            if(encon_rmgrp && rmgrp1){
                if(Rmgrp.reservada != ""){
                         usergroup.rmgrp(Rmgrp.reservada);
                }else{cout << endl << "[mkgrp] --- Falta valor para >name= " << endl; }

            }else{cout << endl << "[mkgrp] --- Falta parametro >name= " << endl; }

        
    }else if(mkusr  == true){
                bool encon_grp  = cadena.find(">grp=") != string::npos;
                bool encon_user = cadena.find(">user=") != string::npos;
                bool encon_pass = cadena.find(">pass=") != string::npos;
                bool log_con    = cadena.find("mkusr") != string::npos;
                if(log_con && encon_grp  && encon_user && encon_pass){
                    if(!(Mkusr.grupo == "" && Mkusr.pass == "" && Mkusr.user == "")){
                                    usergroup.mkuser(Mkusr.user,Mkusr.pass,Mkusr.grupo);
                    }else{
                        if(Mkusr.grupo == ""){   cout << endl <<"[mkusr] --- Falta valor para >grp= "<< endl;}
                        if(Mkusr.user == ""){ cout << endl <<"[mkusr] --- Falta valor para >user= "<< endl;}
                        if(Mkusr.pass == ""){   cout << endl <<"[mkusr] --- Falta valor para >pass= "<< endl;}
                    }
                }else if(encon_grp  && encon_user ){
                        cout << endl <<"[mkusr] --- Falta parametro >pass= " << endl;
                        if(Mkusr.grupo == ""){   cout << endl <<"[mkusr] --- Falta valor para >grp= "<< endl;}
                        if(Log.user == ""){ cout << endl <<"[mkusr]  --- Falta valor para >user= "<< endl;}
                }else if(encon_user && encon_pass){
                        cout << endl << "[mkusr]  --- Falta parametro >grp= " << endl << endl;
                        if(Log.pass == ""){   cout << endl <<"[mkusr]  --- Falta valor para >pass= "<< endl;}
                        if(Log.user == ""){   cout << endl <<"[mkusr] --- Falta valor para >user= "<< endl;}
                }else if(encon_grp  && encon_pass){
                        cout<< endl << "[mkusr] --- Falta parametro >user= " << endl << endl;
                        if(Mkusr.grupo == ""){   cout << endl <<"[mkusr] --- Falta valor para >grp= "<< endl;}
                        if(Log.pass == ""){ cout << endl <<"[mkusr] --- Falta valor para >pass= "<< endl;}
                }else if(log_con){
                    cout << endl << "[mkusr] --- Falta parametro >user= " << endl; 
                    cout << endl << "[mkusr] --- Falta parametro >pass= " << endl;
                    cout << endl << "[mkusr] --- Falta parametro >grp=   " << endl;
                    cout << endl;
                }



    }else if(rmusr == true){

                        bool encon_usr = cadena.find(">user=") != string::npos;
                        bool rmUSR = cadena.find("rmusr") != string::npos;
                        if(encon_usr && rmUSR){
                            if(Mkusr.user != ""){
                                    usergroup.rmuser(Mkusr.user);
                            }else{cout << endl << "[rmusr] --- Falta valor para >user= " << endl; }

                        }else{cout << endl << "[rmusr] --- Falta parametro >user= " << endl; }


    }else if(rep == true){
            report.passDate(Disk,adminfiles);
            bool name1  = cadena.find(">name=") != string::npos;
            bool id1    = cadena.find(">id=") != string::npos;
            bool path1  = cadena.find(">path=") != string::npos;
            bool rep1   = cadena.find("rep") != string::npos;

            if(name1 && id1  && path1 && rep1){
                        if(!(reporte.id == "" && reporte.name == "" && reporte.path == "")){
                                        report.generarReportes(reporte.name,reporte.path,reporte.id,reporte.ruta);
                        }else{
                            if(reporte.id   == ""){ cout << endl <<"[rep] --- Falta valor para >id=  "<< endl;}
                            if(reporte.name == ""){ cout << endl <<"[rep] --- Falta valor para >name= "<< endl;}
                            if(reporte.path == ""){ cout << endl <<"[rep] --- Falta valor para >path= "<< endl;}
                        }
            }else if(name1  && id1 ){
                   cout << endl <<"[rep] --- Falta parametro >path= " << endl;
                   if(reporte.id   == ""){ cout << endl <<"[rep] --- Falta valor para >id=  "<< endl;}
                   if(reporte.name == ""){ cout << endl <<"[rep] --- Falta valor para >name= "<< endl;}
            }else if(name1 && path1){
                   cout << endl << "[rep]  --- Falta parametro >id= "  << endl;
                   if(reporte.name == ""){ cout << endl <<"[rep] --- Falta valor para >name= "<< endl;}
                   if(reporte.path == ""){ cout << endl <<"[rep] --- Falta valor para >path= "<< endl;}
            }else if(path1  && id1){
                   cout<< endl << "[rep] --- Falta parametro >name= " << endl;
                   if(reporte.path == ""){ cout << endl <<"[rep] --- Falta valor para >path= "<< endl;}
                   if(reporte.id   == ""){ cout << endl <<"[rep] --- Falta valor para >id=  "<< endl;}
            }else if(rep1){
                 cout << endl <<"[rep] --- Falta parametro >path= " << endl;
                 cout << endl << "[rep]  --- Falta parametro >id= "  << endl;
                 cout<< endl << "[rep] --- Falta parametro >name= " << endl;
                 cout << endl;
            }

    }else if(mkdir == true){
        adminfiles.passDate(Disk,usergroup);
        if(cadena.find(">path=") != string::npos && cadena.find("mkdir") != string::npos){
                    string no_creadas = Disk.LimpiarString(mkpath);
                    vector<string> lista_carpeta = adminfiles.extraerCarpetas(no_creadas);
                    adminfiles.MKDIR(lista_carpeta,mkr);

        }else{
            cout << endl << "[mkdir]--- falta parametro obligatorio >path" << endl;
        }
    }

    
}



