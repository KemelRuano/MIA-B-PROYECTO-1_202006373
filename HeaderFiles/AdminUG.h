#ifndef ADMINUG_H
#define ADMINUG_H
#include <string>
#include <vector>
using namespace std;
#include "../HeaderFiles/AdminDisk.h"
class User_Group{

    public:
         typedef struct _User {
               string user;
               string password;
               string id;
               int uid;
         } User;
         User logeado;
         bool estado = false;
         void Login(string user,string pass , string id , AdminDisk);
         vector<string> extraer(string,char);
         void cerrarsesion();
         void mkgrp(string name);
         void rmgrp(string name);
         void mkuser(string userss,string password,string grupo);
         void rmuser(string user);
    
    private:
      AdminDisk admindk;
};
#endif