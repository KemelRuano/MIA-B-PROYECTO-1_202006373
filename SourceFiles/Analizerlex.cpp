#include "../HeaderFiles/Analizerlex.h"
#include "../HeaderFiles/Tokens.h"
#include "../HeaderFiles/AdminDisk.h"
#include "../HeaderFiles/pass_struct.h"
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

vector<Tokens> Analizerlex::AnalizarLex(string cadena){

    int estado = 0;
    int index = 0;
    vector <Tokens> list_Tokens;
    string lexema = "";
    vector <string> List_reservada = {"mkdisk","rmdisk","fdisk","mount","unmount","mkfs"};
    vector <string> List_atrib = {">size=",">path=",">fit=",">unit=",">name=",">type=",">delete=",">add=",">id=",">fs="};
   
   
   
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
                    }else if(temp >= char(48) && temp <= char(57)){
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
                  if(temp >= char(48) && temp <= char(57)){
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
void Analizerlex::ejecutar_comannd(vector<Tokens> List_token,string cadena){
    // indica que tipo de comando se esta ejecutando
    bool mkdisk = false;
    bool rmdisk = false;
    bool fdisk = false;
    DATOS_PASS::MKDISK Mkdisk;
    DATOS_PASS::FDISK Fdisk;
    DATOS_PASS::RMDISK Rmdisk;

          //ver que tipo de comando es 

            for(int i = 0; i < List_token.size(); i++){
                    if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "mkdisk"){
                            mkdisk = true;

                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "rmdisk"){
                            rmdisk = true;

                    }else if(List_token[i].getToken() == "RESERVADA" && List_token[i].getLexema() == "fdisk"){
                            fdisk = true;
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
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">name=" && i+1 < List_token.size() && List_token[i+1].getToken() == "ID"){
                                                    Fdisk.name =  List_token[i+1].getLexema();                   
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">type=" && i+1 < List_token.size() && List_token[i+1].getToken() == "TYPE"){
                                                    Fdisk.type =  List_token[i+1].getLexema();                   
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">add=" && i+1 < List_token.size() && List_token[i+1].getToken() == "NUMERO"){
                                                    Fdisk.add =  List_token[i+1].getLexema();                   
                                }else if(List_token[i].getToken() == "ATRIBUTO" && List_token[i].getLexema() == ">delete=" && i+1 < List_token.size() && List_token[i+1].getToken() == "ID"){
                                                    Fdisk.delit =  List_token[i+1].getLexema();                 
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
    }

    // // --------------------------------------------  PARA EL COMANDO FDISK
    if(rmdisk == true){
        if( cadena.substr(0, 6) == "rmdisk" && cadena.substr(7, 6) == ">path=" && cadena.substr(13,cadena.length()) != Rmdisk.path){
              Disk.eliminar_disk(Rmdisk.path);
        }else{
             if(cadena.find(">path=") == string::npos) cout <<"     [rmdisk] falto parametro ---- >path=" << endl; 
              if(Rmdisk.path == ""){cout << "     [rmkdisk] >path= falta valor" << endl;}
             cout << "comando rmdisk invalido" << endl;
        }

    }

    // // ----------------------------------- PARA FDISK
   
    if(fdisk == true){
        if(Fdisk.delit == ""){
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
            if( cadena.find(">name=") != string::npos && Fdisk.name != "" && \
                cadena.find(">path=") != string::npos && Fdisk.path != "" && \
                 cadena.find(">delete=") != string::npos && Fdisk.delit != ""){

                        cout << "es para eliminar" << endl;
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

    // otro 
    
}



