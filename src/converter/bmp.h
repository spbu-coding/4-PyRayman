#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#pragma pack(push, 2)
typedef struct tagBMPFILEHEADER{
    int16_t file_type;
    int32_t size;
    int16_t reserved1;
    int16_t reserved2;
    int32_t image_offset;
}BMPFILEHEADER;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct tagBMPINFOHEADER{
    int32_t header_size;
    int32_t width;
    int32_t height;
    int16_t planes;
    int16_t bpp;
    int32_t compression;
    int32_t image_size;
    int32_t x_pixels;
    int32_t y_pixels;
    int32_t color_used;
    int32_t color_important;

}BMPINFOHEADER;
#pragma pack(pop)

typedef struct rgb{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
}RGB;

typedef struct BMP_Image{
    BMPINFOHEADER *info;
    int8_t *palette;
    RGB **rgb;
}IMAGE;

int mine_comparer(char* input1, char* input2);
int mine_realisation(char* input1, char* input2);