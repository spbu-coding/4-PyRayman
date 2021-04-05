#include "qdbmp.h"
#include <stdio.h>

int theirs_realisation(char *argv[]) {
    UCHAR r, g, b;
    UINT width, height;
    UINT x, y;
    BMP *bmp;

    /* Read an image file */
    bmp = BMP_ReadFile(argv[2]);
    BMP_CHECK_ERROR(stdout, -1);

    /* Get image's dimensions */
    width = BMP_GetWidth(bmp);
    height = BMP_GetHeight(bmp);

    /* Iterate through all the image's pixels */
    if (bmp->Header.BitsPerPixel == 24) {
        for (x = 0; x < width; ++x) {
            for (y = 0; y < height; ++y) {
                /* Get pixel's RGB values */
                BMP_GetPixelRGB(bmp, x, y, &r, &g, &b);

                /* Invert RGB values */
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
    /* Save result */
    BMP_WriteFile(bmp, argv[3]);
    BMP_CHECK_ERROR(stdout, -2);


    /* Free all memory allocated for the image */
    BMP_Free(bmp);

    return 0;
}

