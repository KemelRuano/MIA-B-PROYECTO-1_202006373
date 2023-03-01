#ifndef ANALIZERLEX_H
#define ANALIZERLEX_H
#include "../HeaderFiles/Tokens.h"
#include <vector>
#include <iostream>
#include <string>
using namespace std;

class Analizerlex{
    public:
        vector<Tokens> AnalizarLex(string cadena);
        void ejecutar_comannd(vector<Tokens>,string cadena);

};
#endif