#include "../HeaderFiles/Tokens.h"
#include "../HeaderFiles/AdminDisk.h"
#include "../HeaderFiles/Class_Estruct.h"
#include "../HeaderFiles/pass_struct.h"
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>
#include <cstdio>
#include <algorithm>
#include <stdlib.h>
#include <cctype>
#include <cstring>
#include "string"

using namespace std;

int  startValue;
void AdminDisk::crear_disk(string size,string path,string unit,string fit){
        // limpiar comillas si trae espacio

        Estructuras::DISCO mbr;

        for (int i = 0; i < path.length(); i++){
              if(path[i] == '"'){
                  path.erase(i,1);
              }
        } 
        //Carpetas
        string ruta = obtner_rutaCarpeta(path);
        bool exist_carpeta = ver_existeCarpeta(ruta);
        if (exist_carpeta == false){
            if ( mkdir(ruta.c_str(), 0755) == 0) {
                     cout << "  correcto ✓✓✓ [mkdisk] ----- carpeta creada" << endl;
            }
        }

        bool estado = true;
        if(file_existe(path) == true){
                cout << "  error xxx   [mkdisk]---- ya existe el disco con ese nombre " << endl;
                 mostrar_mbr(path);
                cout << "  error xxx   [mkdisk]---- cree uno nuevo con un nombre distinto" << endl;
        }else{

                int tamano = stoi(size);
                if(stoi(size) > 0){
                    if(unit == "m"){
                        tamano = tamano * 1024 * 1024;
                    }else if(unit == "k"){
                          tamano = tamano * 1024;
                    }else if(unit == ""){
                        tamano = tamano * 1024 * 1024;
                    }
                    estado = false;
                }else{
                    cout << "   error xxx  [mkdisk] ---- error el tamano del disco debe ser mayor a 0" << endl;
                }
                char is_fit;
                if(fit == "bf"){
                    is_fit = 'b';
                }else if(fit == "wf"){
                     is_fit = 'w';
                }else if(fit == "ff"){
                    is_fit = 'f';
                }else if(fit == ""){
                    is_fit = 'f';
                }
                // numero aleatorio para asig_disk
            
                if(estado == false){
                    mbr.size_disk = tamano;
                    mbr.time_disk = time(nullptr);
                    mbr.fit_disk = is_fit;  
                    mbr.asig_disk = rand() % 9999 + 100;  


                    mbr.mbr_Part_1 = Estructuras::Partition();
                    mbr.mbr_Part_2 = Estructuras::Partition();
                    mbr.mbr_Part_3 = Estructuras::Partition();
                    mbr.mbr_Part_4 = Estructuras::Partition();

                    FILE *archivo = fopen(path.c_str(), "w+b");

                    fwrite("\0", 1, 1, archivo);
                    fseek(archivo, tamano-1, SEEK_SET);
                    fwrite("\0", 1, 1, archivo);
                    rewind(archivo);
                    fwrite(&mbr, sizeof(Estructuras::DISCO), 1, archivo);
                    fclose(archivo);
                    cout << "  correcto ✓✓✓ [mkdisk] ----- disco creado" << endl;
                }
                mostrar_mbr(path);
                
        }
        
}
void AdminDisk::mostrar_mbr(string path){
    Estructuras::DISCO mbr;
    FILE *imprimir = fopen(path.c_str(), "r");
    if(imprimir!=NULL){
        fseek(imprimir, 0, SEEK_SET);
        fread(&mbr,sizeof(Estructuras::DISCO), 1,imprimir);
        struct tm *now = localtime(&mbr.time_disk);
        string fecha;
        string hora;
        string FomatoFH;
        fecha = to_string(now->tm_mday) + "-" + to_string((now->tm_mon + 1)) + "-" + to_string((now->tm_year + 1900));
        hora = to_string( now->tm_hour) + ":" + to_string(now->tm_min) + ":" + to_string(now->tm_sec);
        FomatoFH = fecha + ' ' + hora;
        
        cout << "--------------------MBR--------------------" << endl;
        cout << "--------  " << "size_disk:         " << mbr.size_disk << " bytes"<< endl;
        cout << "--------  " << "fecha_creacion:    " << FomatoFH << endl;
        cout << "--------  " << "tipo de ajuste:    " << mbr.fit_disk << endl;
        cout << "--------  " << "numero_asignacion: " << mbr.asig_disk << endl;
        cout << "--------  " << "tamano MBR:        "  << sizeof(mbr)<< " bytes"<< endl;
        if(mbr.mbr_Part_1.part_status == '1'){
                cout << "--------------------Paticion 1--------------------" << endl;
                cout << "--------  " << "nombre:            " << mbr.mbr_Part_1.part_name << endl;
                cout << "--------  " << "tamano:            " << mbr.mbr_Part_1.part_size << endl;
                cout << "--------  " << "inicio:            " << mbr.mbr_Part_1.part_start << endl;
                cout << "--------  " << "ocupado:           " << mbr.mbr_Part_1.part_status << endl;
                cout << "--------  " << "tipo de particion: "  << mbr.mbr_Part_1.part_type<< endl;
                cout << "--------  " << "ajuste:            "  << mbr.mbr_Part_1.part_fit<< endl;
                if(mbr.mbr_Part_1.part_type == 'e'){
                       vector<Estructuras::EBR> list_ext = getlogics(mbr.mbr_Part_1,path);
                       cout << "--------------------Particiones Logica--------------------" << endl;
                       for (Estructuras::EBR record: list_ext){
                                cout << "--------------------EBR--------------------" << endl;
                                cout << "--------  " << "nombre:            " << record.part_name << endl;
                                cout << "--------  " << "tamano:            " << record.part_size << endl;
                                cout << "--------  " << "inicio:            " << record.part_start << endl;
                                cout << "--------  " << "siguiente:         "  << record.part_next  << endl;
                                cout << "--------  " << "ocupado:           " << record.part_status << endl;
                                cout << "--------  " << "ajuste:            "  << record.part_fit<< endl;
                    
                       }
                       
                }

       }

       if(mbr.mbr_Part_2.part_status == '1'){
                cout << "--------------------Paticion 2--------------------" << endl;
                cout << "--------  " << "nombre:            " << mbr.mbr_Part_2.part_name << endl;
                cout << "--------  " << "tamano:            " << mbr.mbr_Part_2.part_size << endl;
                cout << "--------  " << "inicio:            " << mbr.mbr_Part_2.part_start << endl;
                cout << "--------  " << "ocupado:           " << mbr.mbr_Part_2.part_status << endl;
                cout << "--------  " << "tipo de particion: "  << mbr.mbr_Part_2.part_type<< endl;
                cout << "--------  " << "ajuste:            "  << mbr.mbr_Part_2.part_fit<< endl;
                if(mbr.mbr_Part_2.part_type == 'e'){
                       vector<Estructuras::EBR> list_ext = getlogics(mbr.mbr_Part_2,path);
                       cout << "--------------------Particiones Logica--------------------" << endl;
                       for (Estructuras::EBR record: list_ext){
                                cout << "--------------------EBR--------------------" << endl;
                                cout << "--------  " << "nombre:            " << record.part_name << endl;
                                cout << "--------  " << "tamano:            " << record.part_size << endl;
                                cout << "--------  " << "inicio:            " << record.part_start << endl;
                                cout << "--------  " << "siguiente:         "  << record.part_next  << endl;
                                cout << "--------  " << "ocupado:           " << record.part_status << endl;
                                cout << "--------  " << "ajuste:            "  << record.part_fit<< endl;
                    
                       }
                       
                }

       }

       if(mbr.mbr_Part_3.part_status == '1'){
                cout << "--------------------Paticion 3--------------------" << endl;
                cout << "--------  " << "nombre:            " << mbr.mbr_Part_3.part_name << endl;
                cout << "--------  " << "tamano:            " << mbr.mbr_Part_3.part_size << endl;
                cout << "--------  " << "inicio:            " << mbr.mbr_Part_3.part_start << endl;
                cout << "--------  " << "ocupado:           " << mbr.mbr_Part_3.part_status << endl;
                cout << "--------  " << "tipo de particion: "  << mbr.mbr_Part_3.part_type<< endl;
                cout << "--------  " << "ajuste:            "  << mbr.mbr_Part_3.part_fit<< endl;
                if(mbr.mbr_Part_3.part_type == 'e'){
                       vector<Estructuras::EBR> list_ext = getlogics(mbr.mbr_Part_3,path);
                       cout << "--------------------Particiones Logica--------------------" << endl;
                       for (Estructuras::EBR record: list_ext){
                                cout << "--------------------EBR--------------------" << endl;
                                cout << "--------  " << "nombre:            " << record.part_name << endl;
                                cout << "--------  " << "tamano:            " << record.part_size << endl;
                                cout << "--------  " << "inicio:            " << record.part_start << endl;
                                cout << "--------  " << "siguiente:         "  << record.part_next  << endl;
                                cout << "--------  " << "ocupado:           " << record.part_status << endl;
                                cout << "--------  " << "ajuste:            "  << record.part_fit<< endl;
                    
                       }
                       
                }

       }

       if(mbr.mbr_Part_4.part_status == '1'){
                cout << "--------------------Paticion 4--------------------" << endl;
                cout << "--------  " << "nombre:            " << mbr.mbr_Part_4.part_name << endl;
                cout << "--------  " << "tamano:            " << mbr.mbr_Part_4.part_size << endl;
                cout << "--------  " << "inicio:            " << mbr.mbr_Part_4.part_start << endl;
                cout << "--------  " << "ocupado:           " << mbr.mbr_Part_4.part_status << endl;
                cout << "--------  " << "tipo de particion: "  << mbr.mbr_Part_4.part_type<< endl;
                cout << "--------  " << "ajuste:            "  << mbr.mbr_Part_4.part_fit<< endl;
                if(mbr.mbr_Part_4.part_type == 'e'){
                       vector<Estructuras::EBR> list_ext = getlogics(mbr.mbr_Part_4,path);
                       cout << "--------------------Particiones Logica--------------------" << endl;
                       for (Estructuras::EBR record: list_ext){
                                cout << "--------------------EBR--------------------" << endl;
                                cout << "--------  " << "nombre:            " << record.part_name << endl;
                                cout << "--------  " << "tamano:            " << record.part_size << endl;
                                cout << "--------  " << "inicio:            " << record.part_start << endl;
                                cout << "--------  " << "siguiente:         "  << record.part_next  << endl;
                                cout << "--------  " << "ocupado:           " << record.part_status << endl;
                                cout << "--------  " << "ajuste:            "  << record.part_fit<< endl;
                    
                       }
                       
                }

       }

                
      

                  
    }
    fclose(imprimir);


}

void AdminDisk::CrearParticion(DATOS_PASS::FDISK fdisk){
    
    try{

        startValue = 0;
        string named_name = fdisk.name;
        // limpiar si trae comillas
        string path = fdisk.path;
        for (int i = 0; i < path.length(); i++){
                if(path[i] == '"'){
                    path.erase(i,1);
                }
        } 
        char tipo_part;
        bool is_type = false;
        if(fdisk.type == "l"){
                 is_type = true;
                 tipo_part = 'l';
        }else if(fdisk.type == "p"){
                tipo_part = 'p';
        }else if(fdisk.type == "e"){
                tipo_part = 'e';
        }

        char tipo_disk;
        if(fdisk.fit == "ff"){
            tipo_disk = 'f';
        }else if(fdisk.fit == "wf"){
            tipo_disk = 'w';
        }else if(fdisk.fit == "bf"){
            tipo_disk = 'b';
        }


        int size_bytes = 0;
        if(stoi(fdisk.size) > 0){
            
            if(fdisk.unit == "m"){
                size_bytes = stoi(fdisk.size)*1024*1024;
            }else if (fdisk.unit == "k"){
                size_bytes = stoi(fdisk.size)*1024;
            }else if (fdisk.unit == "b"){
                size_bytes = stoi(fdisk.size);
            }else{
                size_bytes = stoi(fdisk.size)*1024;
            }
            

            if(file_existe(path) == true){
                // leer el mbr 
                Estructuras::DISCO Disco;
                FILE *file = fopen(path.c_str(), "rb+");
                rewind(file);
                fread(&Disco, sizeof(Disco), 1, file);
                fclose(file);
                // aqui anadimos a una lista las particiones del mbr

               vector<Estructuras::Partition> partitions = List_partition(Disco);
               vector<Transition> between;
              

                
                int used = 0;
                int ext = 0;
                int c = 1;
                int base = sizeof(Disco);
                Estructuras::Partition extended;
                for(Estructuras::Partition prttn: partitions){
                        if(prttn.part_status == '1'){
                            Transition trn;
                            trn.partition = c;
                            trn.start = prttn.part_start;
                            trn.end = prttn.part_start + prttn.part_size;

                            trn.before = trn.start - base;
                            base = trn.end;
                            
                            if(used !=0)
                            {
                                between.at(used-1).after = trn.start - (between.at(used-1).end);
                            }
                            between.push_back(trn);
                            used++;

                            if(prttn.part_type == 'e'){
                                ext++;
                                extended = prttn;
                            }
                        }
                        if(used == 4 && !(is_type)){
                            throw runtime_error("[fdisk]--- El Disco alcanzo su maximo de particiones");
                            cout << endl;
                        }else if(ext==1 && tipo_part == 'e'){
                            throw runtime_error("[fdisk] --- El maximo de particiones [ESTENDIDAS] por disco es 1");
                            cout << endl;
                        }
                        c++;
                }
                
                if (ext == 0 && tipo_part == 'l') {
                            throw runtime_error("[fdisk] --- Cree antes una particion [EXTENDIDA]");
                            cout << endl;
                }
                if (used != 0) {
                        between.at(between.size() - 1).after = Disco.size_disk - between.at(between.size() - 1).end;
                }
                
                

                try{
                    findby(Disco,named_name,path);
                    cout << "[fdisk] --- Ya existe una particion con ese nombre" << endl;
                    cout << "[fdisk] --- cree una con otro nombre " << endl;
                    cout << endl;
                    return;
                }catch(exception &e){

                }


                Estructuras::Partition transitions;
                transitions.part_status = '1';
                transitions.part_size = size_bytes;
                transitions.part_type = tipo_part; 
                transitions.part_fit = tipo_disk;
                strcpy(transitions.part_name, named_name.c_str());
              

                if(is_type){
                    logic(transitions,extended,path);
                    return;
                }

                Disco = adjust(Disco, transitions, between,partitions, used);
                
                FILE *bfile = fopen(path.c_str(), "rb+");
                if (bfile != NULL) {
                    fseek(bfile, 0, SEEK_SET);
                    fwrite(&Disco, sizeof(Estructuras::DISCO), 1, bfile);
                    if (fdisk.type == "e") {
                        
                        Estructuras::EBR ebr;
                        ebr.part_start = startValue;
                        fseek(bfile, startValue, SEEK_SET);
                        fwrite(&ebr, sizeof( Estructuras::EBR), 1, bfile);
                        cout << "    [fdisk] ----- Particion [EXTENDIDA] creada " << endl;
                        cout << endl;
                    }
                    fclose(bfile);
                  
                    
                }


            }else{ cout << "    [fdisk] ----- error no existe el disco cree uno con el comando mkdisk" << endl;  cout << endl; }
            
        }else{ cout << "     [fdisk] el tamano tiene que ser mayor a 0" << endl;  cout << endl; }

    }catch(exception &e){
        cout << e.what() << endl;
        return;
    }
    

}





vector<Estructuras::Partition> AdminDisk::List_partition(Estructuras::DISCO mbr){
    vector <Estructuras::Partition> List_new;
    List_new.push_back(mbr.mbr_Part_1);
    List_new.push_back(mbr.mbr_Part_2);
    List_new.push_back(mbr.mbr_Part_3);
    List_new.push_back(mbr.mbr_Part_4);

    return List_new;

}


 Estructuras::DISCO 
 AdminDisk::adjust(Estructuras::DISCO  mbr, Estructuras::Partition p,  vector<Transition> t,vector<Estructuras::Partition> ps, int u) {
    if (u == 0) {
        p.part_start = sizeof(mbr);
        startValue = p.part_start;
        mbr.mbr_Part_1 = p;
        return mbr;
    } else {
        Transition toUse;
        int c = 0;
        for (Transition tr : t) {
            if (c == 0) {
                toUse = tr;
                c++;
                continue;
            }
            
            if (mbr.fit_disk == 'f') {
                if (toUse.before >= p.part_size || toUse.after >= p.part_size) {
                    break;
                }
                toUse = tr;
            }else if (mbr.fit_disk == 'b') {
                if (toUse.before < p.part_size || toUse.after < p.part_size) {
                    toUse = tr;
                } else {
                    if (tr.before >= p.part_size || tr.after >= p.part_size) {
                        int b1 = toUse.before - p.part_size;
                        int a1 = toUse.after - p.part_size;
                        int b2 = tr.before - p.part_size;
                        int a2 = tr.after - p.part_size;

                        if ((b1 < b2 && b1 < a2) || (a1 < b2 && a1 < a2)) {
                            c++;
                            continue;
                        }
                        toUse = tr;
                    }
                }

            }else if (mbr.fit_disk == 'w') {
                if (!(toUse.before >= p.part_size) || !(toUse.after >= p.part_size)) {
                    toUse = tr;
                } else {
                    if (tr.before >= p.part_size || tr.after >= p.part_size) {
                        int b1 = toUse.before - p.part_size;
                        int a1 = toUse.after - p.part_size;
                        int b2 = tr.before - p.part_size;
                        int a2 = tr.after - p.part_size;

                        if ((b1 > b2 && b1 > a2) || (a1 > b2 && a1 > a2)) {
                            c++;
                            continue;
                        }
                        toUse = tr;
                    }
                }
            }
            
            c++;
        }
        
        if (toUse.before >= p.part_size || toUse.after >= p.part_size) {
            if (mbr.fit_disk == 'f') {
                if (toUse.before >= p.part_size) {
                    p.part_start = (toUse.start - toUse.before);
                    startValue = p.part_start;
                } else {
                    p.part_start = toUse.end;
                    startValue = p.part_start;
                }
            } else if (mbr.fit_disk  == 'b') {
                int b1 = toUse.before - p.part_size;
                int a1 = toUse.after - p.part_size;

                if ((toUse.before >= p.part_size && b1 < a1) || !(toUse.after >= p.part_start)) {
                    p.part_start = (toUse.start - toUse.before);
                    startValue = p.part_start;
                } else {
                    p.part_start = toUse.end;
                    startValue = p.part_start;
                }
            } else if (mbr.fit_disk == 'w') {
                int b1 = toUse.before - p.part_size;
                int a1 = toUse.after - p.part_size;

                if ((toUse.before >= p.part_size && b1 > a1) || !(toUse.after >= p.part_start)) {
                    p.part_start = (toUse.start - toUse.before);
                    startValue = p.part_start;
                } else {
                    p.part_start = toUse.end;
                    startValue = p.part_start;
                }
            }

            Estructuras::Partition partitions[4];
            for (int i = 0; i < ps.size(); i++) {
                partitions[i] = ps.at(i);
            }

            for (auto &partition : partitions) {
                if (partition.part_status == '0') {
                    partition = p;
                    break;
                }
            }

            Estructuras::Partition aux;
            for (int i = 3; i >= 0; i--) {
                for (int j = 0; j < i; j++) {
                    if ((partitions[j].part_start > partitions[j + 1].part_start)) {
                        aux = partitions[j + 1];
                        partitions[j + 1] = partitions[j];
                        partitions[j] = aux;
                    }
                }
            }

            for (int i = 3; i >= 0; i--) {
                for (int j = 0; j < i; j++) {
                    if (partitions[j].part_status == '0') {
                        aux = partitions[j];
                        partitions[j] = partitions[j + 1];
                        partitions[j + 1] = aux;
                    }
                }
            }

            mbr.mbr_Part_1 = partitions[0];
            mbr.mbr_Part_2 = partitions[1];
            mbr.mbr_Part_3 = partitions[2];
            mbr.mbr_Part_4 = partitions[3];
            return mbr;
        } else {
            throw runtime_error("  [fdisk]---- no hay suficiente espacio para realizar la particion");
            cout << endl;
        }
    }
}

void AdminDisk::logic(Estructuras::Partition partition, Estructuras::Partition ep, string p) {
    Estructuras::EBR nlogic;
    nlogic.part_status = '1';
    nlogic.part_fit = partition.part_fit;
    nlogic.part_size = partition.part_size;
    nlogic.part_next = -1;
    strcpy(nlogic.part_name, partition.part_name);

    FILE *file = fopen(p.c_str(), "rb+");
    rewind(file);
    Estructuras::EBR tmp;
    fseek(file, ep.part_start, SEEK_SET);
    fread(&tmp, sizeof(Estructuras::EBR), 1, file);
    int size = 0;
    do {
        cout << "tamono eber: " << sizeof(Estructuras::EBR) << endl;
        cout << "temp_size: " << tmp.part_size << endl;
        cout << "1 " << size << endl;
        size += sizeof(Estructuras::EBR) + tmp.part_size;
        cout << "2 " << size << endl;
        if (tmp.part_status == '0' && tmp.part_next == -1) {
            nlogic.part_start = tmp.part_start;
            nlogic.part_next = nlogic.part_start + nlogic.part_size + sizeof(Estructuras::EBR);
            cout << "resto -" <<ep.part_size << endl;
            cout << "tamano" <<size << endl;
            cout << "nlogicsize: "<<nlogic.part_size << endl;
            if ((ep.part_size - size) <= nlogic.part_size) {

                throw runtime_error("   [fdisk] --- almacenamiento al maximo nose puede crear la particion logica");
                 cout << endl;
            }
            fseek(file, nlogic.part_start, SEEK_SET);
            fwrite(&nlogic, sizeof(Estructuras::EBR), 1, file);
            fseek(file, nlogic.part_next, SEEK_SET);
            Estructuras::EBR addLogic;
            addLogic.part_status = '0';
            addLogic.part_next = -1;
            addLogic.part_start = nlogic.part_next;
            fseek(file, addLogic.part_start, SEEK_SET);
            fwrite(&addLogic, sizeof(Estructuras::EBR), 1, file);
            fclose(file);
            cout << " [fdisk] ---- partición [LOGICA ]creada correctamente " << endl;
            cout << endl;
            return;
        }
        fseek(file, tmp.part_next, SEEK_SET);
        fread(&tmp, sizeof(Estructuras::EBR), 1, file);

    } while (true);
}



vector<Estructuras::EBR> AdminDisk::getlogics(Estructuras::Partition partition, string p) {
    vector<Estructuras::EBR> ebrs;

    FILE *file = fopen(p.c_str(), "rb+");
    rewind(file);
    Estructuras::EBR tmp;
    fseek(file, partition.part_start, SEEK_SET);
    fread(&tmp, sizeof(Estructuras::EBR), 1, file);
    do {
        if (!(tmp.part_status == '0' && tmp.part_next == -1)) {
            if (tmp.part_status != '0') {
                ebrs.push_back(tmp);
            }
            fseek(file, tmp.part_next, SEEK_SET);
            fread(&tmp, sizeof(Estructuras::EBR), 1, file);
        } else {
            fclose(file);
            break;
        }
    } while (true);
    return ebrs;
}

Estructuras::Partition AdminDisk::findby(Estructuras::DISCO mbr, string name, string path) {
    Estructuras::Partition partitions[4];
    partitions[0] = mbr.mbr_Part_1;
    partitions[1] = mbr.mbr_Part_2;
    partitions[2] = mbr.mbr_Part_3;
    partitions[3] = mbr.mbr_Part_4;

    bool ext = false;
    Estructuras::Partition extended;
    int numpart = 0;
    for (auto &partition : partitions) {
        if (partition.part_status == '1') {
            if (partition.part_name == name) {
                return partition;
            } else if (partition.part_type == 'e') {
                ext = true;
                extended = partition;
            }
 
        }
    }
    if (ext) {
        vector<Estructuras::EBR> ebrs = getlogics(extended, path);
        for (Estructuras::EBR ebr : ebrs) {
            if (ebr.part_status == '1') {
                if (ebr.part_name == name) {
                    Estructuras::Partition tmp;
                    tmp.part_status = '1';
                    tmp.part_type = 'l';
                    tmp.part_fit = ebr.part_fit;
                    tmp.part_start = ebr.part_start;
                    tmp.part_size = ebr.part_size;
                    strcpy(tmp.part_name, ebr.part_name);
                    return tmp;
                }
            }
        }
    }
    throw runtime_error("[fdisk]---- La partición no existe");
    cout << endl;
}


string AdminDisk::LimpiarString(string antes){
    for (int i = 0; i < antes.length(); i++){
              if(antes[i] == '"'){
                  antes.erase(i,1);
              }
    } 

    return antes;

}

void AdminDisk::addEspace(string path,string nombre, string unidades , string cantidad){

    try {
        string name_search = LimpiarString(nombre);
        string pathLimpiado = LimpiarString(path);
        int add = stoi(cantidad);
        bool aument = false;
        bool dismin = false;
        if(add > 0){
             aument = true;
        }else if(add < 0){
            dismin = true;

        }
        if(unidades == "m"){
            add = stoi(cantidad) * 1024 * 1024;
        }else if (unidades == "k"){
            add = stoi(cantidad) * 1024;
        }

        FILE *file = fopen(pathLimpiado.c_str(), "rb+");
        if (file == NULL) {
            throw runtime_error("   [fdisk]---- NO existe el disco");
        }

        Estructuras::DISCO disk;
        rewind(file);
        fread(&disk, sizeof(Estructuras::DISCO), 1, file);

        Estructuras::Partition  parts;
        findby(disk, name_search, pathLimpiado);

        Estructuras::Partition partitions[4];
        partitions[0] = disk.mbr_Part_1;
        partitions[1] = disk.mbr_Part_2;
        partitions[2] = disk.mbr_Part_3;
        partitions[3] = disk.mbr_Part_4;
        bool encontrado = false;
        bool encontrado2 = false;
        int aumento = 0;
        int indice;
        for (int i = 0; i < 4; i++){
            if(partitions[i].part_status == '1'){
                if(partitions[i].part_name == name_search){
                        if(aument){
                                int sizess = partitions[i].part_start + partitions[i].part_size +  partitions[i+1].part_size +  partitions[i+2].part_size +  partitions[i+3].part_size;
                                if((sizess+add) <= disk.size_disk){
                                    int temp = partitions[i].part_size;
                                    partitions[i].part_size = (temp + add);                  
                                    encontrado = true;
                                    aumento = i + 1;
                                }else{
                                    cout << "[fdisk] ----- no hay mas espacio en el disco para aumentar la particion" << endl;
                                    break;
                                }
                        }
                        if(dismin){
                               int sizess = partitions[i].part_size - abs(add);
                               if(sizess > sizeof(Estructuras::Partition)){ 
                                    partitions[i].part_size = sizess;
                                    aumento = i + 1;
                                    encontrado2 = true;
                                }else{
                                    cout << "[fdisk] debe quedar al menos 27 bytes en la particion" << endl;
                                }
                        }
                       
                }
            }
        }
        if(encontrado){
                for (int x = aumento; x < 4; x++){
                        int star = partitions[x-1].part_start;
                        int size = partitions[x-1].part_size;
                        partitions[x].part_start = star + size ;
                }
                 cout <<"[fdisk]--- Partición aumentada correctamente" << endl;
        }    
        if(encontrado2){

            for (int i = aumento; i < 4; i++){
              int star = partitions[i-1].part_size + partitions[i-1].part_start;
              partitions[i].part_start = star; 
            
            }
             cout <<"[fdisk]--- Partición disminuida correctamente" << endl;
         }
        
        

        disk.mbr_Part_1 = partitions[0];
        disk.mbr_Part_2 = partitions[1];
        disk.mbr_Part_3 = partitions[2];
        disk.mbr_Part_4 = partitions[3];    
     

        rewind(file);
        fwrite(&disk, sizeof(Estructuras::DISCO), 1, file);
        fclose(file);
        mostrar_mbr(pathLimpiado);

    }catch (exception &e) {
        cout << e.what() << endl;
        return;
    }

}


void AdminDisk::EliminarParticion(string date, string p, string n) {
    try {
        
        string path = LimpiarString(p);
        string nombre = LimpiarString(n);
     

        if ( date != "full") {
            throw runtime_error("[fdisk]--- >delete parametro full no especificado");
        }

        FILE *file = fopen(path.c_str(), "rb+");
        if (file == NULL) {
            throw runtime_error("[fdisk]--- NO existe el disco");
        }

        Estructuras::DISCO disk;
        rewind(file);
        fread(&disk, sizeof(Estructuras::DISCO), 1, file);

        findby(disk, nombre, path);

        Estructuras::Partition partitions[4];
        partitions[0] = disk.mbr_Part_1;
        partitions[1] = disk.mbr_Part_2;
        partitions[2] = disk.mbr_Part_3;
        partitions[3] = disk.mbr_Part_4;
  
        Estructuras::Partition past;
        bool fll = false;
        for (int i = 0; i < 4; i++) {
            if (partitions[i].part_status == '1') {
                    if (partitions[i].part_type == 'p') {
                         if (partitions[i].part_name == nombre) {
                            if(date == "full"){
                                  past = partitions[i];
                                  partitions[i] = Estructuras::Partition();
                                  fll = true;
                            }
                            break; 
                         }
                    }else{
                            if (partitions[i].part_name == nombre) {
                                if(date == "full"){
                                    past = partitions[i];
                                    partitions[i] = Estructuras::Partition();
                                    fll = true;
                                }
                                break;
                            }
                            vector<Estructuras::EBR> ebrs = getlogics(partitions[i], path);
                            int count = 0;
                            for (Estructuras::EBR ebr : ebrs) {
                                if (ebr.part_name == nombre) {
                                    ebr.part_status = '0';
                                }
                                fseek(file, ebr.part_start, SEEK_SET);
                                fwrite(&ebr, sizeof(Estructuras::EBR), 1, file);
                                count++;
                            }
                    }
            }
        }

        Estructuras::Partition aux;
        for (int i = 3; i >= 0; i--) {
            for (int j = 0; j < i; j++) {
                if (partitions[j].part_status == '0') {
                    aux = partitions[j];
                    partitions[j] = partitions[j + 1];
                    partitions[j + 1] = aux;
                }
            }
        }

        disk.mbr_Part_1 = partitions[0];
        disk.mbr_Part_2 = partitions[1];
        disk.mbr_Part_3 = partitions[2];
        disk.mbr_Part_4 = partitions[3];

        rewind(file);
        fwrite(&disk, sizeof(Estructuras::DISCO), 1, file);
        if (fll) {
            fseek(file, past.part_start, SEEK_SET);
            int num = static_cast<int>(past.part_size / 2);
            fwrite("\0", sizeof("\0"), num, file);
        }
        fclose(file);
        cout << "[fdisk] --- partición eliminada correctamente " << endl;
    }catch (exception &e) {
        cout << e.what() << endl;
        return;
    }
}
















//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void AdminDisk::eliminar_disk(string path){
    for (int i = 0; i < path.length(); i++){
              if(path[i] == '"'){
                  path.erase(i,1);
              }
   } 

    if (remove(path.c_str()) == 0) {
          cout << "     [mkdisk] Disco eliminado" << endl;
    }else{
          cout << "     [mkdisk] No hay disco que eliminar" << endl;
    }
}

string AdminDisk::obtner_rutaCarpeta(string path){       
        string wordToCount = "/";
        int cant_carpeta = count(path.begin(), path.end(), wordToCount[0]); 
        int cap = 0;
        int index = 0;
        string temp = "";
        while (index < path.length()){
            if(path[index] == '/'){
                 if (cap == (cant_carpeta - 1)){
                        temp += '/';
                        break;
                 }
                 cap +=  1;
            }
            temp += path[index]; 
            index += 1;
        }
    return temp;

}

bool AdminDisk::ver_existeCarpeta(string path){
    
        DIR* directorio = opendir(path.c_str());
            if (directorio) {
                closedir(directorio);
                return true;
            } else {
                return false;
            }
}

bool AdminDisk::file_existe(string documento){
    ifstream f(documento.c_str());

    if(f.good()== 0){
        return false;
    }else{
        return true;
    }
}


//-----------------------------------------------------------Mount------------------------------------------------------------------
string AdminDisk::extrerNameDisk(string path){
    string encontrado = obtner_rutaCarpeta(path);
    int index = 0;
    string nuevo;
    while(index < path.length()){
        if(nuevo == encontrado){
             break;
        }
         nuevo += path[index];
      index++;
    }
    int longname = path.length() - (index+4);
    string temp = path.substr(index,longname);
     
    return temp;
}

void AdminDisk::mount(string name,string path){

    try{
        string paths = LimpiarString(path);
        string nombre = LimpiarString(name);
        string nombreDisco = extrerNameDisk(paths);

        Estructuras::DISCO Disco;
        FILE *file = fopen(paths.c_str(), "rb+");
        if (file == NULL) {
                throw runtime_error("[mount]--- NO existe el disco");
                fclose(file);
        }else{
            rewind(file);
            fread(&Disco, sizeof(Disco), 1, file);
            fclose(file);
            
        }
        
        bool encontrado_P = false;
        bool encontrado_L = false;
   

        Estructuras::Partition partitions[4];
        partitions[0] = Disco.mbr_Part_1;
        partitions[1] = Disco.mbr_Part_2;
        partitions[2] = Disco.mbr_Part_3;
        partitions[3] = Disco.mbr_Part_4;


        for (auto &buscadoPart: partitions){
            if(buscadoPart.part_type == 'p'){
                if(buscadoPart.part_name == nombre){
                    encontrado_P = true;
                    break;
                }
                  
            }else{
                vector<Estructuras::EBR> logics = getlogics(buscadoPart, paths);
                for (Estructuras::EBR buscadoPart2: logics){
                    if(buscadoPart2.part_name == nombre){
                            encontrado_L = true;
                            break; 
                    }
                }
            }
        }
         
        string parte1 = "73";
        if(encontrado_P || encontrado_L){
            bool repetido = false;

            for(auto& mount: List_mounts){
                if(mount.disco == nombreDisco){
                    repetido = true;
                    mount.cont += 1;
                    parte1 += to_string(mount.cont);
                    parte1 += nombreDisco;
                    mount.add_mount_Part(parte1,nombre);
                    break;

                }
            }

            if(!repetido){
                List_mounts.push_back(Mount());
                List_mounts.back().disco = nombreDisco;
                List_mounts.back().path = paths;
                List_mounts.back().cont += 1;
                parte1 +=  to_string(List_mounts.back().cont);
                parte1 += nombreDisco;
                List_mounts.back().add_mount_Part(parte1,nombre);
            }
            



        }else{
            throw runtime_error("[fdisk]---- La partición no existe");
        }

       verVector();

    }catch(exception &e){
        cout << e.what() << endl;
    }
    
}


void AdminDisk::verVector(){

    for (const auto& mount :  List_mounts) {
        cout << mount.disco << ":\n";
        cout << "ruta: " << mount.path << "\n";
        for (const auto& id : mount.ids) {
           cout << "mount: " << id.id << "\n";
           cout << "nombreParticion: " << id.namePartition << "\n";
        }
    }



}

void AdminDisk::unmount(string name){

    string  name_id = LimpiarString(name);
    bool encontrado = false;
    for(int x = 0; x < List_mounts.size(); x++){
        for(int i = 0; i < List_mounts[x].ids.size(); i++){
               if(List_mounts[x].ids[i].id == name_id ){
                        encontrado = true;
               }
        }
    }
    
    if(encontrado){
        for (auto& mount : List_mounts) {
            auto it = std::find_if(mount.ids.begin(), mount.ids.end(), [name_id](const mount_id& id) { return id.id == name_id; });
            if (it != mount.ids.end()) { mount.ids.erase(it);}               
        }
        cout << "[mount] --- desmonatdo exitosamente"<< endl;
        cout << endl;
    }else{
         cout << "[mount] --- No existe un montaje con este id "<< endl;
         cout << endl;
    }
   
    
    
    verVector();

}

Estructuras::Partition AdminDisk::EncontrarPartition(string id,string *p){

    
    string nombreParticion;
    string path;
    string name_id = LimpiarString(id);
    for (int i = 0; i < List_mounts.size(); i++){
            for (int x = 0; x < List_mounts[i].ids.size(); x++){
                if(List_mounts[i].ids[x].id == name_id){
                    path = List_mounts[i].path;
                    nombreParticion = List_mounts[i].ids[x].namePartition;
                }
            }   
    }
    *p = path;
    Estructuras::DISCO mbr;
    FILE *enviar = fopen(path.c_str(),"r");
    if( enviar == NULL){
        throw runtime_error(" [mkfs]-- No existe el disco");
    }
    rewind(enviar);
    fread(&mbr, sizeof(Estructuras::DISCO),1,enviar);
    fclose(enviar);
    return findby(mbr,nombreParticion,path);
    
}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
               //--------------------------------  MKFS  ---------------------------------------------------



void AdminDisk::mkfs(string a , string b, string c){
     
    try{

            string id = LimpiarString(a);
            string tipo = LimpiarString(b);
            string tipo_fs = LimpiarString(c);

            if(tipo != "full"){
                tipo = "full";
            }
            if(!(tipo_fs == "2fs" || tipo_fs == "3fs")){
                cout << " [mkfs]--- el valor fs no es valido" << endl;
                return;
            }

            string path;
            Estructuras::Partition particion = EncontrarPartition(id , &path);

            int num_bloques;
            cout << particion.part_size << endl;
            int ext2  = (particion.part_size - sizeof(Estructuras::Superblock))/(4 + sizeof(Estructuras::Inodes) + 3 * sizeof(Estructuras::Fileblock));
            int ext3  = (particion.part_size - sizeof(Estructuras::Superblock))/(4 + sizeof(Estructuras::Journaling) + sizeof(Estructuras::Inodes)+ 3 * sizeof(Estructuras::Fileblock));
            
            Estructuras::Superblock  superbloque;
            superbloque.s_mtime = time(0);  //Última fecha en el que el sistema fue montado
            superbloque.s_umtime = time(0); //Última fecha en que el sistema fue desmontado
            superbloque.s_mnt_count = 1; // Indica cuantas veces se ha montado el sistema
            
            if(tipo_fs == "2fs"){
                superbloque.s_filesystem_type = 2;      // Guarda el número que identifica el sistema de archivos utilizado
                superbloque.s_inodes_count = ext2;       //Guarda el número total de inodos
                superbloque.s_blocks_count = 3*ext2;     //Guarda el número total de bloques
                superbloque.s_free_blocks_count = 3*ext2; // Contiene el número de bloques libres
                superbloque.s_free_inodes_count = ext2;  // Contiene el número de inodos libres
                cout << ext2 << endl;
                Format_ext2(superbloque,particion,ext2,path);
            }else if(tipo_fs == "3fs"){
                superbloque.s_filesystem_type = 3;    // Guarda el número que identifica el sistema de archivos utilizado
                superbloque.s_inodes_count = ext3;       //Guarda el número total de inodos
                superbloque.s_blocks_count = 3*ext3;     //Guarda el número total de bloques
                superbloque.s_free_blocks_count = 3*ext3; // Contiene el número de bloques libres
                superbloque.s_free_inodes_count = ext3;  // Contiene el número de inodos libres
                Format_ext3(superbloque,particion,ext3,path);
            }



         

    }catch(exception &e){
        cout << e.what() << endl;
    }
}


void AdminDisk::Format_ext2(Estructuras::Superblock superbloque,Estructuras::Partition particion,int bloques , string path){

    superbloque.s_bm_inode_start = particion.part_start + sizeof(Estructuras::Superblock); // byte donde iniciara en el archivo binario
    superbloque.s_bm_block_start = superbloque.s_bm_inode_start + bloques;
    superbloque.s_inode_start = superbloque.s_bm_block_start + (3 * bloques);
    superbloque.s_block_start = superbloque.s_inode_start  + (bloques * sizeof(Estructuras::Inodes));
    char tmp = '0';
    int abarca = superbloque.s_block_start + ( 3 * bloques * sizeof(Estructuras::Folderblock));
    if( abarca < particion.part_size){

            FILE *leer = fopen(path.c_str(), "rb+");
            // inicio del superbloque 
            fseek(leer,particion.part_start,SEEK_SET);
            fwrite(&superbloque, sizeof(Estructuras::Superblock),1,leer);
            // inicio del bitmap de inodos
            fseek(leer,superbloque.s_bm_inode_start,SEEK_SET);
            for (int x = 0; x < bloques; x++){
                    fwrite(&tmp,sizeof(char),1,leer);
            }
            // inicio del bitmap de bloques
            fseek(leer, superbloque.s_bm_block_start,SEEK_SET);
            for (int x = 0; x < (3 * bloques); x++){
                    fwrite(&tmp,sizeof(char),1,leer);
            }

            // como n es el numero de estructas entonces inicia en superbloque.s_inode_start
            // entonces se escribe la estrucuta de inodos la cantidad de veces que sea bloques
            Estructuras::Inodes inodo;
            fseek(leer, superbloque.s_inode_start,SEEK_SET);
            for (int x = 0; x < bloques; x++){
                    fwrite(&inodo,sizeof(Estructuras::Inodes),1,leer);
            }

            Estructuras::Folderblock bloqueCarpetas;
            fseek(leer, superbloque.s_inode_start,SEEK_SET);
            for (int x = 0; x < ( 3 * bloques); x++){
                    fwrite(&bloqueCarpetas,sizeof(Estructuras::Folderblock),1,leer);
            }
            fclose(leer);

            Estructuras::Superblock  readsuperbloque;
            FILE *supblock = fopen(path.c_str(), "rb");
            fseek(supblock,particion.part_start,SEEK_SET);
            fread(&readsuperbloque,sizeof(Estructuras::Superblock), 1 , supblock);
            fclose(supblock);
            // EL INODO CREA LA CARPETA RAIZ
            inodo.i_uid = 1;        // UID del usuario propietario del archivo o carpeta
            inodo.i_gid = 1;        //GID del grupo al que pertenece el archivo o carpeta.
            inodo.i_size = 0;       // Tamaño del archivo en bytes
            inodo.i_atime = superbloque.s_umtime;   // Última fecha en que se leyó el inodo sin modificarlo
            inodo.i_ctime = superbloque.s_umtime;   // Fecha en la que se creó el inodo
            inodo.i_mtime = superbloque.s_umtime;   // Última fecha en la que se modifica el inodo
            inodo.i_block[0] = 0; // array en los que los primeros 12 registros son bloques directos. El 13 será el número del bloque simple indirecto. El 14 será el número del bloque doble indirecto. El 15 será el número del bloque triple indirecto Si no son utilizados tendrá el valor-1.
            inodo.i_type = '0';    // ndica si es archivo o carpeta. Tendrá los siguientes valores: 1 = Archivo 0 = Carpeta
            inodo.i_perm = 664;  //  permisos  los primeros 3 bits Uusuario i_uid. los siguientes 3  grupo que pertenece y ultimos 3 permisos de otros usuarios.
          
           // BLOQUE DE TIPO CARPETA
            Estructuras::Folderblock bloke;
            strcpy(bloke.b_content[0].b_name,".");
            bloke.b_content[0].b_inodo = 0;
            strcpy(bloke.b_content[1].b_name, "..");
            bloke.b_content[1].b_inodo = 0;
            strcpy(bloke.b_content[2].b_name, "user.txt");
            bloke.b_content[2].b_inodo = 1;
            strcpy(bloke.b_content[3].b_name, "-");
            bloke.b_content[3].b_inodo = -1;


            string data = "1,G,root\n1,U,root,root,123\n";
            Estructuras::Inodes inodetmp;
            inodetmp.i_uid = 1;
            inodetmp.i_gid = 1;
            inodetmp.i_size = sizeof(data.c_str()) + sizeof(Estructuras::Folderblock);
            inodetmp.i_atime = superbloque.s_umtime;
            inodetmp.i_ctime = superbloque.s_umtime;
            inodetmp.i_mtime = superbloque.s_umtime;
            inodetmp.i_type = '1';
            inodetmp.i_perm = 664;
            inodetmp.i_block[0] = 1;
            
            inodo.i_size = inodetmp.i_size + sizeof(Estructuras::Folderblock) + sizeof(Estructuras::Inodes);
            // BLOQUE DE TIPO ARCHIVO
            Estructuras::Fileblock fileb;
            strcpy(fileb.b_content, data.c_str());



            FILE *bfiles = fopen(path.c_str(), "rb+");
            fseek(bfiles, superbloque.s_bm_inode_start, SEEK_SET);
            char caracter = '1';
            fwrite(&caracter, sizeof(caracter), 1, bfiles);
            fwrite(&caracter, sizeof(caracter), 1, bfiles);

            fseek(bfiles, superbloque.s_bm_block_start, SEEK_SET);
            fwrite(&caracter, sizeof(caracter), 1, bfiles);
            fwrite(&caracter, sizeof(caracter), 1, bfiles);

            fseek(bfiles, superbloque.s_inode_start, SEEK_SET);
            fwrite(&inodo, sizeof(Estructuras::Inodes), 1, bfiles);
            fwrite(&inodetmp, sizeof(Estructuras::Inodes), 1, bfiles);

            fseek(bfiles, superbloque.s_block_start, SEEK_SET);
            fwrite(&bloke, sizeof(Estructuras::Folderblock), 1, bfiles);
            fwrite(&fileb, sizeof(Estructuras::Fileblock), 1, bfiles);
            fclose(bfiles);
    }else{
        cout << endl;
        cout << "[mkfs] --- el tamano del sistema de archivos Ext2 es mayor a la particion " << particion.part_name << endl;
        cout << "   tamano del sistema: " << abarca << " vs  tamano de la particion: " << particion.part_size << endl;
        cout << "   soluciones:" << endl;
        cout << "   1. aumente el tamano de la particion." << endl;
        cout << "   2. pruebe con otro mount." << endl;
        cout << "   3. cree un nuevo disco con mayor capacidad y particiones con mayor tamano." << endl;
        cout << endl;
    }
}

void AdminDisk::Format_ext3(Estructuras::Superblock superbloque,Estructuras::Partition particion,int bloques , string path){
    
    superbloque.s_bm_inode_start = particion.part_start + sizeof(Estructuras::Superblock) + (bloques * sizeof(Estructuras::Journaling));
    superbloque.s_bm_block_start = superbloque.s_bm_inode_start + bloques;
    superbloque.s_inode_start = superbloque.s_bm_block_start + (3 * bloques);
    superbloque.s_block_start = superbloque.s_inode_start  + (bloques * sizeof(Estructuras::Inodes));
    char tmp = '0';
    FILE *leer = fopen(path.c_str(), "rb+");
    fseek(leer,particion.part_start,SEEK_SET);
     // superbloque en el binario
    fwrite(&superbloque, sizeof(Estructuras::Superblock),1,leer);
    // journaling en el binario
    Estructuras::Journaling journing;
    for(int x = 0; x < bloques; x++){
        fwrite(&journing,sizeof(Estructuras::Journaling),1,leer);
    }
    // inicio del bitmap de inodos
    fseek(leer,superbloque.s_bm_inode_start,SEEK_SET);
    for (int x = 0; x < bloques; x++){
            fwrite(&tmp,sizeof(char),1,leer);
    }
    // inicio del bitmap de bloques
    fseek(leer, superbloque.s_bm_block_start,SEEK_SET);
    for (int x = 0; x < (3 * bloques); x++){
            fwrite(&tmp,sizeof(char),1,leer);
    }

    // como n es el numero de estructas entonces inicia en superbloque.s_inode_start
    // entonces se escribe la estrucuta de inodos la cantidad de veces que sea bloques
    Estructuras::Inodes inodo;
    fseek(leer, superbloque.s_inode_start,SEEK_SET);
    for (int x = 0; x < bloques; x++){
            fwrite(&inodo,sizeof(Estructuras::Inodes),1,leer);
    }

    Estructuras::Folderblock bloqueCarpetas;
    fseek(leer, superbloque.s_inode_start,SEEK_SET);
    for (int x = 0; x < ( 3 * bloques); x++){
            fwrite(&bloqueCarpetas,sizeof(Estructuras::Folderblock),1,leer);
    }
    fclose(leer);

    Estructuras::Superblock  readsuperbloque;
    FILE *supblock = fopen(path.c_str(), "rb");
    fseek(supblock,particion.part_start,SEEK_SET);
    fread(&readsuperbloque,sizeof(Estructuras::Superblock), 1 , supblock);
    fclose(supblock);

    inodo.i_uid = 1;        // UID del usuario propietario del archivo o carpeta
    inodo.i_gid = 1;        //GID del grupo al que pertenece el archivo o carpeta.
    inodo.i_size = 0;       // Tamaño del archivo en bytes
    inodo.i_atime = superbloque.s_umtime;   // Última fecha en que se leyó el inodo sin modificarlo
    inodo.i_ctime = superbloque.s_umtime;   // Fecha en la que se creó el inodo
    inodo.i_mtime = superbloque.s_umtime;   // Última fecha en la que se modifica el inodo
    inodo.i_type = 0;    // ndica si es archivo o carpeta. Tendrá los siguientes valores: 1 = Archivo 0 = Carpeta
    inodo.i_perm = 664;  //  permisos  los primeros 3 bits Uusuario i_uid. los siguientes 3  grupo que pertenece y ultimos 3 permisos de otros usuarios.
    inodo.i_block[0] = 0; // array en los que los primeros 12 registros son bloques directos. El 13 será el número del bloque simple indirecto. El 14 será el número del bloque doble indirecto. El 15 será el número del bloque triple indirecto Si no son utilizados tendrá el valor-1.
    
    
    strcpy(journing.content , "raiz");
    strcpy(journing.path,"/");
    journing.type = 0;
    strcpy(journing.operation , "mkdir");
    journing.date = superbloque.s_umtime;

    Estructuras::Folderblock bloke;
    strcpy(bloke.b_content[0].b_name,".");
    bloke.b_content[0].b_inodo = 0;
    strcpy(bloke.b_content[1].b_name, "..");
    bloke.b_content[1].b_inodo = 0;
    strcpy(bloke.b_content[2].b_name, "user.txt");
    bloke.b_content[2].b_inodo = 1;

    string data = "1,G,root\n1,U,root,root,123\n";
    Estructuras::Inodes inodetmp;
    inodetmp.i_uid = 1;
    inodetmp.i_gid = 1;
    inodetmp.i_size = sizeof(data.c_str()) + sizeof(Estructuras::Folderblock);
    inodetmp.i_atime = superbloque.s_umtime;
    inodetmp.i_ctime = superbloque.s_umtime;
    inodetmp.i_mtime = superbloque.s_umtime;
    inodetmp.i_type = 1;
    inodetmp.i_perm = 664;
    inodetmp.i_block[0] = 1;
    
    inodo.i_size = inodetmp.i_size + sizeof(Estructuras::Folderblock) + sizeof(Estructuras::Inodes);
    
    Estructuras::Journaling journingtemp;
    strcpy(journingtemp.content, data.c_str());
    strcpy(journingtemp.path, "/user.txt");
    journingtemp.type = 1;
    journingtemp.size = sizeof(data) + sizeof(Estructuras::Folderblock);
    strcpy(journingtemp.operation, "mkfl");
    journingtemp.date = superbloque.s_umtime;

    Estructuras::Fileblock fileb;
    strcpy(fileb.b_content, data.c_str());

    FILE *bfiles = fopen(path.c_str(), "rb+");

    fseek(bfiles, particion.part_start + sizeof(Estructuras::Superblock), SEEK_SET);
    fwrite(&journing, sizeof(Estructuras::Journaling), 1, bfiles);
    fwrite(&journingtemp, sizeof(Estructuras::Journaling), 1, bfiles);
    fseek(bfiles, superbloque.s_bm_inode_start,SEEK_SET);
    char caracter = '1';
    fwrite(&caracter, sizeof(caracter), 1, bfiles);
    fwrite(&caracter, sizeof(caracter), 1, bfiles);

    fseek(bfiles, superbloque.s_bm_block_start, SEEK_SET);
    fwrite(&caracter, sizeof(caracter), 1, bfiles);
    fwrite(&caracter, sizeof(caracter), 1, bfiles);

    fseek(bfiles, superbloque.s_inode_start, SEEK_SET);
    fwrite(&inodo, sizeof(Estructuras::Inodes), 1, bfiles);
    fwrite(&inodetmp, sizeof(Estructuras::Inodes), 1, bfiles);

    fseek(bfiles, superbloque.s_block_start, SEEK_SET);
    fwrite(&bloke, sizeof(Estructuras::Folderblock), 1, bfiles);
    fwrite(&fileb, sizeof(Estructuras::Fileblock), 1, bfiles);
    fclose(bfiles);


}