#ifndef PASSSTRUCT_H
#define PASSSTRUCT_H
#include <string>
using namespace std;

class DATOS_PASS{
    public:

            typedef struct _mkdisk{
                string path = "";
                string size = "";
                string fit = "";
                string unit = "";

            } MKDISK; 

            typedef struct _fdisk{
                string size = "";
                string path = "";
                string name = "";
                string unit = "";
                string type = "";
                string fit = "";
                string delit = "";
                string add = "";

            } FDISK; 

            typedef struct _rmdisk{
       
                string path = "";

            } RMDISK;

            typedef struct _mkfs{
       
                string type = "";
                string id = "";
                string fs = "";

            } MKFS;

            typedef struct _LOGIN{
                string user;
                string pass;
                string id;
            }Login;

            typedef struct _MKGRP{
                string reservada;
            }MKGRP;

            typedef struct _RMGRP{
                string reservada;
            }RMGRP;

            typedef struct _MKUSR{
                string user;
                string pass;
                string grupo;
            }MKUSR;

            typedef struct _REP{
                string id;
                string path;
                string name;
                string ruta;
            }REP;


         

};



typedef struct _mount_id {
    string id;
    string namePartition;
}mount_id;

class Mount {
public:
    std::string disco;
    string path;
    int cont;
    std::vector<mount_id> ids;

    void add_mount_Part(string id,string name) {
        ids.push_back({id , name});
    }
};
#endif

