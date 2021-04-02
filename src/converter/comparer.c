#include "bmp.h"

int main(int argc, char **argv) {
    
    if (argc != 3) {
        fprintf(stdout, "Use %s <file1_name>.bmp <file2_name>.bmp\n", argv[0]);
        return -1;
    }
   

    
   if(mine_comparer(argv[1],argv[2]) != 0){
        fprintf(stdout,"bad\n");
        return -1;
    }
    
    fprintf(stdout,"Return: OK");

    return 0; 
}