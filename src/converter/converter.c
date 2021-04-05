
#include "bmp.h"
#include "negative_func.c"


int main(int argc, char **argv){
    int mine_count = 0;
    int theirs_count = 0;

    if (argc != 4) {
        fprintf(stdout, "Use %s --mine (or --theirs) <input file>.bmp <output file>.bmp\n", argv[0]);
        return -1;
    }

    if (strncmp(argv[1], "--mine", 6) == 0) {
        mine_count += 1;
    } else if (strncmp(argv[1], "--theirs", 8) == 0) {
        theirs_count += 1;
    }

    if (mine_count != 1 && theirs_count != 1) {
        fprintf(stdout, "Use %s --mine (or --theirs) <input file>.bmp <output file>.bmp\n", argv[0]);
        return -1;
    }

    if(mine_count == 1){
        int output = mine_realisation(argv[2],argv[3]);
        return output;
    }

    else if(theirs_count == 1){
        int output = theirs_realisation(argv);
        return output;
        
    }
    return 0;
}