#include "../HeaderFiles/reportes.h"
#include "../HeaderFiles/AdminDisk.h"
#include "../HeaderFiles/Class_Estruct.h"
#include <iostream>
#include <string.h>
#include <string>
#include <sstream>
#include <cctype>
#include <stdlib.h>
#include <fstream>
#include <sys/stat.h>
#include <cmath>
using namespace std;

void reportes::passDate(AdminDisk admin)
{
    admindisk = admin;
}

string reportes::minusculas(string str)
{
    for (auto &c : str)
    {
        c = std::tolower(c);
    }
    return str;
}

string reportes::Fecha(time_t valor){

    struct tm *now = localtime(&valor);
    string fecha;
    string hora;
    string FomatoFH;
    fecha = to_string(now->tm_mday) + "-" + to_string((now->tm_mon + 1)) + "-" + to_string((now->tm_year + 1900));
    hora = to_string(now->tm_hour) + ":" + to_string(now->tm_min) + ":" + to_string(now->tm_sec);
    FomatoFH = fecha + ' ' + hora;

    return  FomatoFH;
}

void reportes::generarReportes(string name, string pt, string ids, string rute)
{

    string nombre = admindisk.LimpiarString(name);
    string path = admindisk.LimpiarString(pt);
    string id = admindisk.LimpiarString(ids);
    string ruta = admindisk.LimpiarString(rute);
    Estructuras::Partition particion;

    try
    {
        string path_disco;
        particion = admindisk.EncontrarPartition(id, &path_disco);

        if (minusculas(nombre) == "mbr")
        {
            MBR(path, path_disco);
        }
        else if (minusculas(nombre) == "disk")
        {
            DISK(path, path_disco);
        }
        else if (minusculas(nombre) == "bm_inode")
        {
            bms(path, path_disco, particion,true);
        }
        else if (minusculas(nombre) == "bm_block")
        {
            bms(path, path_disco, particion,false);
        }else if(minusculas(nombre) == "inode"){
              INODE(path, path_disco, particion);
        }else if(minusculas(nombre) == "block"){
           Block(path, path_disco, particion);
        }else if(minusculas(nombre) == "sb"){
            superbloque(path,path_disco,particion);
        }else{
            cout << "[rep] --- No existe este tipo de reporte" << endl;
            return;
        }
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
    }
}

void reportes::MBR(string ruta, string path)
{

    Estructuras::DISCO mbr;
    FILE *imprimir = fopen(path.c_str(), "r");
    if (imprimir == NULL)
    {
        throw runtime_error("[fdisk]-- NO existe el disco");
    }
    fseek(imprimir, 0, SEEK_SET);
    fread(&mbr, sizeof(Estructuras::DISCO), 1, imprimir);

    struct tm *now = localtime(&mbr.time_disk);
    string fecha;
    string hora;
    string FomatoFH;
    fecha = to_string(now->tm_mday) + "-" + to_string((now->tm_mon + 1)) + "-" + to_string((now->tm_year + 1900));
    hora = to_string(now->tm_hour) + ":" + to_string(now->tm_min) + ":" + to_string(now->tm_sec);
    FomatoFH = fecha + ' ' + hora;

    if (admindisk.ver_existeCarpeta(admindisk.obtner_rutaCarpeta(ruta)) == false)
    {
        if (mkdir(ruta.c_str(), 0755) == 0)
            ;
    }
    string rutaimage;
    string rutadot = ruta.substr(0, ruta.find('.'));
    rutaimage = rutadot + ".png";
    rutadot += ".dot";
    string s(1, mbr.fit_disk);

    std::ofstream file;
    file.open(rutadot);

    file << "digraph {\n";
    file << "node [shape=plaintext fontname=\"Courier New\"] \n";
    file << "tabla [label=<<table border=\"3\" cellborder=\"1\" cellspacing=\"2\">\n";
    file << "   <tr>\n";
    file << "       <td colspan=\"30\" bgcolor=\"#E5D518\" border=\"3\" > REPORTE DE MBR </td> \n";
    file << "   </tr>\n";
    file << "   <tr>\n";
    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\">mbr_tamano: </td> \n";
    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + to_string(mbr.size_disk) + " </td> \n";
    file << "   </tr>\n";
    file << "   <tr>\n";
    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\">mbr_fecha_creacion: </td> \n";
    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + FomatoFH + " </td> \n";
    file << "   </tr>\n";
    file << "   <tr>\n";
    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\">mbr_disk_asignature: </td> \n";
    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + to_string(mbr.asig_disk) + " </td> \n";
    file << "   </tr>\n";
    file << "   <tr>\n";
    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\">mbr_type_ajust: </td> \n";
    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + s + " </td> \n";
    file << "   </tr>\n";
    vector<Estructuras::Partition> List_part = admindisk.List_partition(mbr);
    for (int i = 0; i < List_part.size(); i++)
    {
        if (List_part[i].part_status == '1')
        {
            if (List_part[i].part_type == 'e')
            {
                vector<Estructuras::EBR> list_ext = admindisk.getlogics(List_part[i], path);
                for (Estructuras::EBR record : list_ext)
                {
                    std::string str_sts(1, record.part_status);
                    std::string ptr_name(record.part_name);
                    std::string ptr_fit(1, record.part_fit);
                    file << "   <tr>\n";
                    file << "       <td colspan=\"30\" align=\"left\" bgcolor=\"#27EADE\" border=\"1\"> PARTICION LOGICA</td> \n";
                    file << "   </tr>\n";
                    file << "   <tr>\n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_status: </td> \n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + str_sts + " </td> \n";
                    file << "   </tr>\n";
                    file << "   <tr>\n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_fit: </td> \n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + ptr_fit + " </td> \n";
                    file << "   </tr>\n";
                    file << "   <tr>\n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_size: </td> \n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + to_string(record.part_size) + " </td> \n";
                    file << "   </tr>\n";
                    file << "   <tr>\n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_start: </td> \n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + to_string(record.part_start) + " </td> \n";
                    file << "   </tr>\n";
                    file << "   <tr>\n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_next: </td> \n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + to_string(record.part_next) + " </td> \n";
                    file << "   </tr>\n";
                    file << "   <tr>\n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_name: </td> \n";
                    file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + ptr_name + " </td> \n";
                    file << "   </tr>\n";
                }
            }
            else if (List_part[i].part_type == 'p')
            {
                std::string str(List_part[i].part_name);
                std::string status(1, List_part[i].part_status);
                std::string tipo(1, List_part[i].part_type);
                std::string Fit(1, List_part[i].part_fit);
                file << "   <tr>\n";
                file << "       <td colspan=\"30\" align=\"left\" bgcolor=\"#EA2745\" border=\"1\"> PARTICION PRIMARIA</td> \n";
                file << "   </tr>\n";
                file << "   <tr>\n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_status: </td> \n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + str + " </td> \n";
                file << "   </tr>\n";
                file << "   <tr>\n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_size: </td> \n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + to_string(List_part[i].part_size) + " </td> \n";
                file << "   </tr>\n";
                file << "   <tr>\n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_start: </td> \n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + to_string(List_part[i].part_start) + " </td> \n";
                file << "   </tr>\n";
                file << "   <tr>\n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_status: </td> \n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + status + " </td> \n";
                file << "   </tr>\n";
                file << "   <tr>\n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_type: </td> \n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + tipo + " </td> \n";
                file << "   </tr>\n";
                file << "   <tr>\n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> part_fit: </td> \n";
                file << "       <td align=\"center\" colspan=\"15\" bgcolor=\"#C7C4A6\"> " + Fit + " </td> \n";
                file << "   </tr>\n";
            }
        }
    }

    file << " </table>>]";
    file << "label = \"Reporte  MBR \n By: Kemel Ruano\" \n";
    file << "}\n";

    file.close();
    string dotneon = "dot -Tpng " + rutadot + " -o " + rutaimage;
    system(dotneon.c_str());
}

void reportes::DISK(string ruta, string path)
{

    Estructuras::DISCO mbr;
    FILE *imprimir = fopen(path.c_str(), "r");
    if (imprimir == NULL)
    {
        throw runtime_error("[fdisk]-- NO existe el disco");
    }
    fseek(imprimir, 0, SEEK_SET);
    fread(&mbr, sizeof(Estructuras::DISCO), 1, imprimir);

    if (admindisk.ver_existeCarpeta(admindisk.obtner_rutaCarpeta(ruta)) == false)
    {
        if (mkdir(ruta.c_str(), 0755) == 0)
            ;
    }
    string rutaimage;
    string rutadot = ruta.substr(0, ruta.find('.'));
    rutaimage = rutadot + ".png";
    rutadot += ".dot";

    std::ofstream file;
    file.open(rutadot);
    string mBR = "   MBR  ";
    file << "digraph {\n";
    file << "node [shape=plaintext];\n";
    file << "A [label=<<TABLE BORDER=\"6\" CELLBORDER=\"2\" CELLSPACING=\"1\" WIDTH=\"300\" HEIGHT=\"200\">\n";
    file << "<TR>\n";
    file << " <TD ROWSPAN=\"3\" WIDTH=\"300\" HEIGHT=\"200\">" + mBR + "</TD>\n";
    int contBlockLogic = 0;
    int es_acoupado = 0;
    int es_acoupadoLogic = 0;
    int tot = 0;
    int totLogic = 0;
    vector<Estructuras::Partition> List_part = admindisk.List_partition(mbr);
    for (int i = 0; i < List_part.size(); i++)
    {
        if (List_part[i].part_status == '1')
        {
            std::string str(List_part[i].part_name);
            if (List_part[i].part_type == 'p')
            {
                double en_num = List_part[i].part_size;
                double es_entero = (en_num / mbr.size_disk) * 100;
                double es_porcentaje = round(es_entero * 100.0) / 100.0;
                std::stringstream ss;
                ss << es_porcentaje;
                string porcentaje = ss.str() + " % del disco";
                file << "<TD ROWSPAN=\"3\" WIDTH=\"300\" HEIGHT=\"200\"> PARTICION PRIMARIA <BR/>" + str + "<BR/>" + porcentaje + "</TD>\n";
            }
            else if (List_part[i].part_type == 'e')
            {
                int numero = List_part[i].part_size;
                double en_num = numero;
                double es_entero = (en_num / mbr.size_disk) * 100;
                double es_porcentaje = round(es_entero * 100.0) / 100.0;
                std::stringstream ss;
                ss << es_porcentaje;
                string porcentaje = ss.str() + " % del disco";
                file << "<TD>\n";
                file << "   <TABLE BORDER=\"2\"  CELLBORDER=\"5\" CELLSPACING=\"3\"  WIDTH=\"300\" HEIGHT=\"200\">\n";
                vector<Estructuras::EBR> list_ext = admindisk.getlogics(List_part[i], path);
                for (int x = 0; x < list_ext.size(); x++)
                {
                    contBlockLogic += 2;
                    es_acoupadoLogic = list_ext[x].part_start + list_ext[x].part_size + sizeof(Estructuras::EBR);
                    if (!(es_acoupadoLogic == list_ext[x + 1].part_start))
                    {
                        contBlockLogic += 1;
                    }
                }
                file << "       <TR>\n";
                file << "           <TD COLSPAN=\"" + to_string(contBlockLogic) + "\" WIDTH=\"300\" HEIGHT=\"200\"> PARTICION EXTENDIDA <BR/>" + str + "<BR/>" + porcentaje + "</TD> \n";
                file << "       </TR>\n";
                file << "       <TR>\n";
                es_acoupadoLogic = 0;
                for (int x = 0; x < list_ext.size(); x++)
                {
                    if (list_ext[x].part_status == '1')
                    {
                        file << "               <TD WIDTH=\"300\" HEIGHT=\"200\"> EBR </TD>\n";
                        std::string str1(list_ext[x].part_name);
                        int numero = list_ext[x].part_size;
                        double en_num = numero;
                        double es_entero = (en_num / List_part[i].part_size) * 100;
                        double es_porcentaje = round(es_entero * 100.0) / 100.0;
                        std::stringstream ss;
                        ss << es_porcentaje;
                        string porcentaje = ss.str() + " % de la extendida";
                        file << "               <TD WIDTH=\"300\" HEIGHT=\"200\"> PARTICION LOGICA <BR/>" + str1 + "<BR/>" + porcentaje + "</TD>\n";
                    }

                    es_acoupadoLogic = list_ext[x].part_start + list_ext[x].part_size + sizeof(Estructuras::EBR);
                    totLogic += list_ext[x].part_size;
                    if (!(es_acoupadoLogic == list_ext[x + 1].part_start))
                    {
                        double en_num = List_part[i].part_size - totLogic;
                        double es_entero = (en_num / List_part[i].part_size) * 100;
                        double es_porcentaje = round(es_entero * 100.0) / 100.0;
                        std::stringstream ss;
                        ss << es_porcentaje;
                        string porcentaje = ss.str() + " % de la extendida";
                        file << "                   <TD WIDTH=\"300\" HEIGHT=\"200\">  LIBRE <BR/>" + porcentaje + "</TD>\n";
                    }
                }
                file << "       </TR>\n";
                file << "   </TABLE>\n";
                file << "</TD>\n";
            }

            es_acoupado = List_part[i].part_start + List_part[i].part_size;
            tot += List_part[i].part_size;
            if (!(es_acoupado == List_part[i + 1].part_start))
            {
                double en_num = mbr.size_disk - tot;
                double es_entero = (en_num / mbr.size_disk) * 100;
                double es_porcentaje = round(es_entero * 100.0) / 100.0;
                std::stringstream ss;
                ss << es_porcentaje;
                string porcentaje = ss.str() + " % del disco";
                file << "<TD ROWSPAN=\"3\" WIDTH=\"300\" HEIGHT=\"200\">  LIBRE <BR/>" + porcentaje + "</TD>\n";
            }
        }
    }
    file << "</TR>\n";
    file << "</TABLE>>];\n";
    file << "label = \"Reporte DISK \n By: Kemel Ruano\" \n";
    file << "}\n";

    file.close();
    string dotneon = "dot -Tpng " + rutadot + " -o " + rutaimage;
    system(dotneon.c_str());
    cout << "[rep] ---- Reporte Disk Generado con exito" << endl;
}

void reportes::bms(string ruta, string path, Estructuras::Partition encontrado,bool tipobmp)
{

    Estructuras::Superblock superbloque;
    if (admindisk.ver_existeCarpeta(admindisk.obtner_rutaCarpeta(ruta)) == false){
        if (mkdir(ruta.c_str(), 0755) == 0);
    }
    FILE *imprimir = fopen(path.c_str(), "r");
    if (imprimir == NULL){
        throw runtime_error("[fdisk]-- NO existe el disco");
    }
    string rutadot = ruta.substr(0, ruta.find('.'));
    rutadot += ".txt";


    fseek(imprimir, encontrado.part_start, SEEK_SET);
    fread(&superbloque, sizeof(Estructuras::Superblock), 1, imprimir);

    int size_bits = 0;
    string titulo = "";
    if(tipobmp == true){
                 titulo = "    BITMAP DE INODOS   ";
                 size_bits = superbloque.s_bm_block_start - superbloque.s_bm_inode_start;
                 fseek(imprimir, superbloque.s_bm_inode_start, SEEK_SET);

    }else{
                 titulo = "   BITMAP DE BLOQUES   ";
                 size_bits = (superbloque.s_bm_block_start - superbloque.s_bm_inode_start) * 3;
                 fseek(imprimir, superbloque.s_bm_block_start, SEEK_SET);
    }
    char temp[size_bits];
    fread(&temp, sizeof(temp), 1, imprimir);
    
    fclose(imprimir);
    std::ofstream archivo;
    archivo.open(rutadot);
    archivo << "▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄" << endl;
    archivo << "███████████████████████████"<<titulo<<"███████████████████████████████" << endl;
    archivo << "▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ " << endl;
    archivo << "╔═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╗" << endl;
    int count = 0;
    int estado = false;
    for (int i = 0; i < size_bits; i++)
    {
        if (count == 0)
        {
            archivo << "║ " << temp[i] << " ║";
        }

        if (count > 0 && count < 19)
        {
            archivo << " " << temp[i] << " ║";
        }
        if (count == 19)
        {
            archivo << " " << temp[i] << " ║" << endl;
            archivo << "╠═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╣" << endl;
            estado = true;
        }
        count++;
        if (estado)
        {
            count = 0;
            estado = false;
        }
    }
    if (!(count == 0))
    {
        for (int x = count; x < 20; x++)
        {
            archivo << "   ║";
            if (x == 19)
            {
                archivo << endl;
            }
        }
    }
    archivo << "╚═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╝" << endl;

    archivo.close();

    if(tipobmp){ 
        cout << endl;
        cout << "████████   [rep]--- Reporte bm_inodo generado con exito" << endl;
        cout << endl;
    }else{
        cout << endl;
        cout << "████████    [rep]--- Reporte bm_block generado con exito" << endl;
        cout << endl;
    }
}

void reportes::INODE(string ruta, string path,Estructuras::Partition encontrado){

    Estructuras::Superblock superbloque;

    if (admindisk.ver_existeCarpeta(admindisk.obtner_rutaCarpeta(ruta)) == false){
        if (mkdir(ruta.c_str(), 0755) == 0);
    }

    FILE *imprimir = fopen(path.c_str(), "r");
    if (imprimir == NULL){
        throw runtime_error("[fdisk]-- NO existe el disco");
    }
    fseek(imprimir, encontrado.part_start, SEEK_SET);
    fread(&superbloque, sizeof(Estructuras::Superblock), 1, imprimir);
    int total = superbloque.s_bm_block_start - superbloque.s_bm_inode_start;


    char temp[total];
    fseek(imprimir,superbloque.s_bm_inode_start , SEEK_SET);
    fread(&temp,sizeof(temp),1,imprimir);
    Estructuras::Inodes inodo;


    string rutaimage;
    string rutadot = ruta.substr(0, ruta.find('.'));
    rutaimage = rutadot + ".png";
    rutadot += ".dot";

    std::ofstream file;
    file.open(rutadot);
    file << "digraph g { \n";
    file << "   node[shape=plaintext]; \n";
    file << "   graph [rankdir = LR bgcolor = white style=filled]; \n";
    

  
    for(int x = 0; x < total; x++){
        if(temp[x] == '1'){
                inodo = Estructuras::Inodes();
                fseek(imprimir,superbloque.s_inode_start + (sizeof(Estructuras::Inodes)*x), SEEK_SET);
                fread(&inodo,sizeof(Estructuras::Inodes),1,imprimir);
                file << " node_"+to_string(x)+"[label=<<table border=\"2\" cellspacing=\"2\">";
                file << "       <tr> \n ";
                if(inodo.i_type == '0'){
                    file << "          <td bgcolor=\"red\" COLSPAN =\"2\"><b> Inodo "+ to_string(x) +"</b> </td> \n";
                    
                }else{
                    file << "          <td bgcolor=\"yellow\" COLSPAN =\"2\"><b> Inodo "+ to_string(x) +"</b> </td> \n";
                }
                file << "       </tr> \n";
                file << "<tr><td bgcolor=\"skyblue\"> UID:     </td><td>"+to_string(inodo.i_uid)+"</td></tr> \n";
                file << "<tr><td bgcolor=\"skyblue\"> GID:     </td><td>"+to_string(inodo.i_gid)+"</td></tr> \n";
                file << "<tr><td bgcolor=\"skyblue\"> TAMANO:  </td><td>"+to_string(inodo.i_size)+"</td></tr> \n";
                  file << "<tr><td bgcolor=\"skyblue\"> TIPO:  </td><td>"+ string(1, inodo.i_type) +"</td></tr> \n";
                file << "<tr><td bgcolor=\"skyblue\"> TIME:    </td><td>"+Fecha(inodo.i_atime)+"</td></tr> \n";
                file << "<tr><td bgcolor=\"skyblue\"> TIME:    </td><td>"+Fecha(inodo.i_ctime)+"</td></tr> \n";
                file << "<tr><td bgcolor=\"skyblue\"> TIME:    </td><td>"+Fecha(inodo.i_mtime)+"</td></tr> \n";
                for(int y = 0; y < 16; y++){
                        file << "<tr><td bgcolor=\"skyblue\"> APD_"+to_string(y)+"   </td><td>"+to_string(inodo.i_block[y])+"</td></tr> \n";
                
                    }
                file << "<tr><td bgcolor=\"skyblue\"> PERMISO:    </td><td>"+to_string(inodo.i_perm)+"</td></tr> \n";

                file << "           </table>>];\n";

                

                if( x > 0){
                    file << "node_"+to_string(x - 1)+" ->  node_"+ to_string(x)+"; \n";
                }

        }
    
    }

    file << "label = \"Reporte INODO \n By: Kemel Ruano\" \n";
    file << "}\n";

    file.close();
    string dotneon = "dot -Tpng " + rutadot + " -o " + rutaimage;
    system(dotneon.c_str());
    cout << endl;
    cout << "████████    [rep]--- Reporte INODO Generado con exito" << endl;
    cout << endl;
 
    
}

void reportes::Block(string ruta, string path,Estructuras::Partition encontrado){

    Estructuras::Superblock superbloque;
    Estructuras::Folderblock folder;
    Estructuras::Inodes inodo;
    Estructuras::Fileblock Archivo;
    if (admindisk.ver_existeCarpeta(admindisk.obtner_rutaCarpeta(ruta)) == false){
        if (mkdir(ruta.c_str(), 0755) == 0);
    }
    FILE *imprimir = fopen(path.c_str(), "r");
    if (imprimir == NULL){
        throw runtime_error("[fdisk]-- NO existe el disco");
    }
    
    fseek(imprimir,encontrado.part_start,SEEK_SET);
    fread(&superbloque,sizeof(Estructuras::Superblock),1,imprimir);
    
  
    char temp[superbloque.s_inodes_count];
    fseek(imprimir,superbloque.s_bm_inode_start , SEEK_SET);
    fread(&temp,sizeof(temp),1,imprimir);

    char temp2[superbloque.s_blocks_count];
    fseek(imprimir,superbloque.s_bm_block_start , SEEK_SET);
    fread(&temp2,sizeof(temp2),1,imprimir);

    string rutaimage;
    string rutadot = ruta.substr(0, ruta.find('.'));
    rutaimage = rutadot + ".png";
    rutadot += ".dot";

    std::ofstream file;
    file.open(rutadot);
    file << "digraph g { \n";
    file << "   node[shape=plaintext]; \n";
    file << "   graph [rankdir = LR bgcolor = white style=filled]; \n";


    for(int y = 0; y < superbloque.s_inodes_count; y++){
          if(temp[y] == '1'){
             inodo = Estructuras::Inodes();
             fseek(imprimir,superbloque.s_inode_start + ( sizeof(Estructuras::Inodes) * y),SEEK_SET);
             fread(&inodo,sizeof(Estructuras::Inodes),1,imprimir);
             int dato = -1;
             int dato2 = -1;
             for(int x = 0; x < 16; x++){
                    if (inodo.i_block[x] != -1) {
                        if(inodo.i_type == '0'){
                            dato = inodo.i_block[x];
                            folder = Estructuras::Folderblock();
                            fseek(imprimir,superbloque.s_block_start + (sizeof(Estructuras::Folderblock)*inodo.i_block[x]),SEEK_SET);
                            fread(&folder,sizeof(Estructuras::Folderblock),1,imprimir);
                            file << " block_"+to_string(inodo.i_block[x])+"[label=<<table border=\"2\" cellspacing=\"2\"> \n";
                            file << "       <tr> \n ";
                            file << "          <td bgcolor=\"red\" COLSPAN =\"2\"><b> Bloque "+ to_string(inodo.i_block[x]) +"</b> </td> \n";
                            file << "       </tr> \n ";
                            string name;
                            for(int z = 0; z < 4; z++){ 
                                name += folder.b_content[z].b_name;
                                file << "<tr><td bgcolor=\"skyblue\"> "+ name +"  </td><td>"+to_string(folder.b_content[z].b_inodo)+"</td></tr> \n";
                                name = "";
                            }
                            file << "           </table>>];\n";
                            

                        }else if(inodo.i_type == '1'){
                            dato = inodo.i_block[x];
                            Archivo = Estructuras::Fileblock();
                            fseek(imprimir,superbloque.s_block_start + (sizeof(Estructuras::Fileblock)*inodo.i_block[x]),SEEK_SET);
                            fread(&Archivo,sizeof(Estructuras::Fileblock),1,imprimir);
                            cout << Archivo.b_content << endl;
                            string name;
                            name += Archivo.b_content;
                            file << " block_"+to_string(inodo.i_block[x])+"[label=<<table border=\"2\" cellspacing=\"2\"> \n";
                            file << "       <tr> \n ";
                            file << "          <td bgcolor=\"yellow\" COLSPAN =\"2\"><b> Bloque "+ to_string(inodo.i_block[x]) +"</b> </td> \n";
                            file << "       </tr> \n ";
                            file << "<tr><td bgcolor=\"skyblue\"> "+ name +"</td></tr> \n";
                            file << "           </table>>];\n";
                            name = "";
                            
                       }
                        if(dato > 0){
                            file << "block_"+to_string(dato-1)+" -> block_"+to_string(dato)+"; \n";
                            dato = -1;
                        }
                    }
                }
                   
                   
          }
    }
    fclose(imprimir);

    file << "label = \"Reporte BLOQUES \n By: Kemel Ruano\" \n";
    file << "}\n";

    file.close();
    string dotneon = "dot -Tpng " + rutadot + " -o " + rutaimage;
    system(dotneon.c_str());
    cout << endl;
    cout << "████████    [rep]--- Reporte BLOCK Generado con exito" << endl;
    cout << endl;


}


void reportes::superbloque(string ruta, string path,Estructuras::Partition encontrado){

    Estructuras::Superblock superbloque;
    if (admindisk.ver_existeCarpeta(admindisk.obtner_rutaCarpeta(ruta)) == false){
        if (mkdir(ruta.c_str(), 0755) == 0);
    }
    FILE *imprimir = fopen(path.c_str(), "r");
    if (imprimir == NULL){
        throw runtime_error("[fdisk]-- NO existe el disco");
    }
    string rutaimage;
    string rutadot = ruta.substr(0, ruta.find('.'));
    rutaimage = rutadot + ".png";
    rutadot += ".dot";

    fseek(imprimir,encontrado.part_start,SEEK_SET);
    fread(&superbloque,sizeof(Estructuras::Superblock),1,imprimir);

    ofstream file;
    file.open(rutadot);
    file << "digraph{  \n";
    file << "   node[shape=plaintext]; \n";
    file << "   graph [rankdir = LR bgcolor = white style=filled fontname = \"Courier New\"]; \n";
    file << "   Tabla[fontname = \"Courier New\" label=<<table border=\"2\" cellspacing=\"1\" cellborder = \"2\" width = \"300\" bgcolor = \"black\"> \n";
    file << "       <tr>  <td bgcolor=\"orange\" COLSPAN =\"2\" width = \"300\" height = \"50\"><b> SUPER_BLOQUE</b> </td>  </tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> filesystem_type  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_filesystem_type) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Total inodos  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_inodes_count) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Total bloques  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_blocks_count) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Total bloques Libres  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_free_blocks_count) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Total inodos Libres </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_free_inodes_count) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Ultima Fecha Montado </td><td bgcolor=\"white\"> "+ Fecha(superbloque.s_mtime) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Ultima Fecha Desmontado </td><td bgcolor=\"white\"> "+ Fecha(superbloque.s_umtime) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Desmontado  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_mnt_count) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Magic  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_magic) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Tamano Inodo  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_inode_size) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Tamano Bloque  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_block_size) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Primer Inodo Libre  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_fist_ino) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Primer Bloque Libre  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_first_blo) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Inicio BM Inodo  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_bm_inode_start) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Inicio BM Bloque  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_bm_block_start) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Inicio Tabla Inodo  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_inode_size) +"</td></tr> \n";
    file << "       <tr><td bgcolor=\"skyblue\"> Inicio Tabla Bloques  </td><td bgcolor=\"white\"> "+ to_string(superbloque.s_block_start) +"</td></tr> \n";
    file << "   </table>>]; \n";
    file << "   label = \"Reporte SB By: Kemel Ruano\"; \n";
    file << "}";
    fclose(imprimir);
    file.close();
    string dotneon = "dot -Tpng " + rutadot + " -o " + rutaimage;
    system(dotneon.c_str());
    cout << endl;
    cout << "████████    [rep]--- Reporte SB Generado con exito" << endl;
    cout << endl;



}



