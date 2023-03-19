#ifndef CLASSESTRUCT_H
#define CLASSESTRUCT_H
#include <string>
#include <ctime>
using namespace std;
class Estructuras{
    public:
            Estructuras();
            typedef struct _Partition {
                char part_status = '0'; 
                char part_type; 
                char part_fit; 
                int part_start = -1; 
                int part_size = 0; 
                char part_name[16]; 
            } Partition; 

            typedef struct _MBR 
            {
                int size_disk; // 4 bytes
                time_t time_disk; // 19 bytes
                int asig_disk; // 4 bytes
                char fit_disk; // 1 byte
                Partition mbr_Part_1; // 27 bytes
                Partition mbr_Part_2;  // 27 bytes
                Partition mbr_Part_3;   // 27 bytes
                Partition mbr_Part_4; // 27 bytes

            } DISCO; // 125 bytes
            
            
        typedef struct _EBR {
            char part_status = '0';
            char part_fit;
            int part_start;
            int part_size = 0;
            int part_next = -1;
            char part_name[16];
        } EBR;

        typedef struct _Inodes {
            int i_uid = -1;
            int i_gid = -1;
            int i_size = -1;
            time_t i_atime;
            time_t i_ctime;
            time_t i_mtime;
            int i_block[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,-1};
            char i_type = '3';
            int i_perm = -1;
        } Inodes;

 // --------------------------------------------------------       
        //ASOCIDA A UN INODO DE CARPETAS 
        /// CONTENIDO DE CARPETAS 
        typedef struct _Content {
                char b_name[12];
                int b_inodo = -1;
        } Content;
         // BLOQUE DE CAPETAS 
        typedef struct _Folderblock //64
        {
            Content b_content[4];
        } Folderblock;

        typedef struct _Superblock {
            int s_filesystem_type = 0;
            int s_inodes_count = 0;
            int s_blocks_count = 0;
            int s_free_blocks_count = 0;
            int s_free_inodes_count = 0;
            time_t s_mtime;
            time_t s_umtime;
            int s_mnt_count;
            int s_magic = 0xEF53;
            int s_inode_size = sizeof(Inodes);
            int s_block_size= sizeof(Folderblock);
            int s_fist_ino = 0;
            int s_first_blo = 0;
            int s_bm_inode_start = 0;
            int s_bm_block_start = 0;
            int s_inode_start = 0;
            int s_block_start = 0;
        } Superblock;


        // BLOQUE DE ARCHIVOS 
        typedef struct _Fileblock //64
        {
            char b_content[64];
        } Fileblock;
        // BLOQUE DE APUNTADORES
        typedef struct _Pointerblock //64
        {                                             //termina directos           ?  [Indirecto simple] , [indirecto doble] , [indirecto triple]
            int b_pointers[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        } Pointerblock;

        typedef struct _Journaling {
            char operation[10] = "";
            char type = -1;
            char path[100] = "";
            char content[60] = "";
            time_t date;
            int size = 0;
        } Journaling;


};
#endif