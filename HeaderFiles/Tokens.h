#ifndef TOKENS_H
#define TOKENS_H
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class Tokens{

    private:
        string token;
        string lexema;
    
    public:
        Tokens(string tok,string lex){
            token = tok;
            lexema = lex;
        }
        void setToken(string tok){
            token = tok;
        }
        string getToken(){
            return token;
        }
        void setLexema(string lex){
            lexema = lex;
        }
        string getLexema(){
            return lexema;
        }
};
#endif