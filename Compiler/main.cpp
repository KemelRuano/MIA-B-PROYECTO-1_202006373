
#include "../HeaderFiles/Analizerlex.h"
#include "../HeaderFiles/Tokens.h"
#include <fstream>
#include<iostream>
#include <sstream>
#include <vector>
using namespace std;


int main(){
    string L_comand;
    cout << endl;
    cout << "*****************************************************************" << endl;
    cout << "******************   Consola de comandos   **********************" << endl;
    cout << "*****************************************************************" << endl;
    cout << "                                      [escriba {exit} para salir]" << endl;
    cout << endl;
    while(true){
        cout << "KemelRuano_PRY1:~$    ";
        getline(cin,L_comand);
        if(L_comand == "exit"){
                cout << "Gracias por utilizar el programa"<<endl;  
                return false;      
        }    
        Analizerlex lexico;
        vector<Tokens> List_token = lexico.AnalizarLex(L_comand);
        lexico.ejecutar_comannd(List_token,L_comand);
        
    }
    return 0;
 }


