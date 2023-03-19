
#include "../HeaderFiles/Analizerlex.h"
#include "../HeaderFiles/Tokens.h"
#include <fstream>
#include<iostream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
using namespace std;
#include <dirent.h>

void LeerScript(string cadena,Analizerlex lexico){
   string path_start = ">path="; 
   string ruta;
   size_t pos = cadena.find(path_start);
   if (pos != string::npos) { 
      pos += path_start.length(); 
      ruta = cadena.substr(pos, cadena.length() - pos);
   }

    string linea = "";
   if(ruta.find(".eea") != string::npos){

        ifstream files(ruta);
        string line;
        while (getline(files, line)){
            line += "  ";
            vector<Tokens> List_token = lexico.AnalizarLex(line);
            lexico.ejecutar_comannd(List_token,line);
            line = "";
            
        }
        
        // Cierra el archivo
        files.close();


    }else{
         cout << endl;
         cout << "████████  extension invalida solo se acpeta .eea" << endl;
         cout << endl;
    }
}

int main(){
    string L_comand;
    cout << endl;
    cout << "*****************************************************************" << endl;
    cout << "******************   Consola de comandos   **********************" << endl;
    cout << "*****************************************************************" << endl;
    cout << "                                      [escriba {exit} para salir]" << endl;
    cout << endl;
    while(true){
        Analizerlex lexico;
        cout << "KemelRuano_PRY1:~$    ";
        getline(cin,L_comand);
        
        if(L_comand == "exit"){
                cout << "Gracias por utilizar el programa"<<endl;  
                return false;      
        }else if(L_comand.find("execute") != string::npos && L_comand.find(">path=") != string::npos){
                         LeerScript(L_comand,lexico);

        }else{
            vector<Tokens> List_token = lexico.AnalizarLex(L_comand);
            lexico.ejecutar_comannd(List_token,L_comand);
        }
       
        
    }
    return 0;
 }


