#ifndef CLASSESTRUCT_H
#define CLASSESTRUCT_H
#include <ctime>
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
                
            

};
#endif