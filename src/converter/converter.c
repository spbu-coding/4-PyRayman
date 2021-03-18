
#include "qdbmp.h"
#include <stdio.h>
#include "bmp.c"

int theirs_realisation(char *input, char *output){
    UCHAR r, g, b;
    UINT width, height;
    UINT x, y;
    BMP *bmp;

    bmp = BMP_ReadFile(input);
    BMP_CHECK_ERROR(stdout, -1);

    width = BMP_GetWidth(bmp);
    height = BMP_GetHeight(bmp);

     
    if (bmp->Header.BitsPerPixel == 24) {
        for (x = 0; x < width; ++x) {
            for (y = 0; y < height; ++y) {
                BMP_GetPixelRGB(bmp, x, y, &r, &g, &b);
                BMP_SetPixelRGB(bmp, x, y, ~r, ~g, ~b);
            }
        }
    } else if (bmp->Header.BitsPerPixel == 8) {
        for (long long int i = 0; i < BMP_PALETTE_SIZE_8bpp; i++) {
            if ((i + 1) % 4 != 0) {
                bmp->Palette[i] = ~bmp->Palette[i];
            }
        }
    }

    BMP_WriteFile(bmp, output);
    BMP_CHECK_ERROR(stdout, -2);

    BMP_Free(bmp);

    return 0;
}


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
        int output = theirs_realisation(argv[2],argv[3]);
        return output;
        
    }
    return 0;
}