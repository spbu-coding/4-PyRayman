#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#pragma pack(push, 2)
typedef struct tagBMPFILEHEADER{
    uint16_t file_type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t image_offset;
}BMPFILEHEADER;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct tagBMPINFOHEADER{
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_pixels;
    uint32_t y_pixels;
    uint32_t color_used;
    uint32_t color_important;

}BMPINFOHEADER;
#pragma pack(pop)

typedef struct rgb{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
}RGB;

typedef struct BMP_Image{
    BMPINFOHEADER *info;
    uint8_t *palette;
    RGB **rgb;
}IMAGE;

int mine_comparer(char* input1, char* input2);
int mine_realisation(char* input1, char* input2);