#include "compare_lib.h"

uint8_t **read_rgb(FILE *fp, uint32_t width, uint32_t height, uint16_t bpp){
    uint16_t bytespp = bpp/8;
    uint8_t **rgb  = malloc(sizeof(uint16_t *) * height);
    if(!rgb){
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return 0;
    }  
    for (uint32_t i = 0; i < height; i++) {
        rgb[i] = malloc(sizeof(uint16_t) * width);
        if (!rgb[i]){
            fprintf(stdout, "Cannot allocate memory for results string in %lld experiment\n", i);
            for (int j = 0; j < i; j++) {
                free(rgb[i]);
            }
            free(rgb);
            return 0;
        }
    }

    for (long long int i = 0; i < height; i++) {
        for (long long int j = 0; j < width; j++) {
            rgb[i][j] = 0;
            fread(&rgb[i][j], bytespp, 1, fp);
        }
    } 
    return rgb;
}

uint8_t *read_palette(FILE *fp, uint32_t colors){
    uint16_t table_size = colors * 4;// colors_used * bytes of color table
    uint8_t *palette = malloc(sizeof(int) * table_size);
    if(!palette){
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return 0;
    }
    for(uint32_t i = 0; i < table_size; i++){
        palette[i] = 0;
        fread(&palette[i],1,1,fp);
    }
    return palette;

} 


IMAGE *read_Image(FILE *fp,BMPINFOHEADER *info){
    IMAGE *picture = calloc(1,sizeof(IMAGE));
    if (!picture) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return NULL;
    }


    picture->info = info;
    if(!picture->info){
        free(picture);
        return NULL;
    }

    if (picture->info->bpp == 8) {
        picture->palette = read_palette(fp, picture->info->color_used);
        if (!picture->palette) {
            free(picture);
            return NULL;
        }
    }

    picture->rgb = read_rgb(fp,picture->info->width,picture->info->height,picture->info->bpp);
    if(!picture->rgb){
        free(picture);
        return NULL;
    }

    return picture;

}


void free_Image(IMAGE *picture, BMPFILEHEADER *header, BMPINFOHEADER *info){
    free(picture->info);
    free(picture->palette);
    free(picture->rgb);
    free(picture);
    free(info);
    free(header);
}



/////////////////////////////////////// COMPARE_FUNC ///////////////////////////////////////

int compare_tables(IMAGE *picture1, IMAGE *picture2){
    for(uint32_t i = 0; i < picture1->info->color_used; i++ ){
        if(picture1->palette[i] != picture2->palette[i]){
            printf("Table colors error");
            return -1;
        } 
    }
    return 0;
}

void compare_pixels(IMAGE *picture1, IMAGE *picture2) {
    int count_of_pixels = 0;
    for (uint32_t i = 0; i < abs(picture1->info->height); i++){
        for (uint32_t j = 0; j < abs(picture1->info->width); j++){
            uint32_t first = picture1->info->height > 0 ? i : -picture1->info->height - i - 1;
            uint32_t second = picture2->info->height > 0 ? i : -picture2->info->height - i - 1;
            if(picture1->rgb[first][j] != picture2->rgb[second][j]){
                fprintf(stderr, "%lld %lld \n", j, i);
                count_of_pixels++;
                if (count_of_pixels >= 100) break;
            }  
        }
        if (count_of_pixels >= 100) break;   
    }
}

int mine_comparer(char* input1, char* input2){
    FILE *file1;
    FILE *file2;
    BMPINFOHEADER *info1 = malloc(sizeof(BMPINFOHEADER));
    BMPFILEHEADER *header1  = malloc(sizeof(BMPFILEHEADER));
    BMPINFOHEADER *info2 = malloc(sizeof(BMPINFOHEADER));
    BMPFILEHEADER *header2  = malloc(sizeof(BMPFILEHEADER));

    file1 = fopen(input1,"rb");
    if( file1 == NULL){
        fprintf(stdout, "Cannot open file. No file with name %s exists. ", input1);
        return -1;
    }

    fread(header1,sizeof(BMPFILEHEADER),1,file1); 
    fread(info1,sizeof(BMPINFOHEADER),1,file1);
    
    IMAGE *picture1 = read_Image(file1,info1);
    if (!picture1) {
        fprintf(stdout, "Error for a file named %s. ", input1);
        fclose(file1);
        return -1;
    }
    file2 = fopen(input2, "rb");
    if (file2 == NULL) {
        fprintf(stdout, "Cannot open file. No file with name %s exists. ", input2);
        return -1;
    }

    fread(header2,sizeof(BMPFILEHEADER),1,file2); 
    fread(info2,sizeof(BMPINFOHEADER),1,file2);

    IMAGE *picture2 = read_Image(file2,info2);
    if (!picture2) {
        fprintf(stdout, "Error for a file named %s. ", input2);
        fclose(file2);
        return -1;
    }
    
    if ((picture1->info->width != picture2->info->width) ||
        (abs(picture1->info->height) != abs(picture2->info->height))) {
        fprintf(stdout, "Images have different values of width or height. ");
        free_Image(picture1,header1,info1);
        fclose(file1);
        free_Image(picture2,header2,info2);
        fclose(file2); 
        return -1;
    } else if ((header1->size != header2->size)) {
        fprintf(stdout, "Images have different values of sizes. ");
        free_Image(picture1,header1,info1);
        fclose(file1);
        free_Image(picture2,header2,info2);
        fclose(file2); 
        return -1;
    
    } else if ((picture1->info->bpp != picture2->info->bpp)) {
        fprintf(stdout, "Images have different values of bits_per_pixel. ");
        free_Image(picture1,header1,info1);
        fclose(file1);
        free_Image(picture2,header2,info2);
        fclose(file2); 
        return -1;
    } else if ((picture1->info->color_used != picture2->info->color_used)) {
        fprintf(stdout, "Images have different number of colors in the table. ");
        free_Image(picture1,header1,info1);
        fclose(file1);
        free_Image(picture2,header2,info2);
        fclose(file2); 
        return -1;
    } 
    else {
    
        if(!compare_tables(picture1,picture2)){
            compare_pixels(picture1,picture2);
        }
        else{
            free_Image(picture1,header1,info1);
            fclose(file1);
            free_Image(picture2,header2,info2);
            fclose(file2); 
            return -1;
        }
    }

    free_Image(picture1,header1,info1);
    fclose(file1);
    free_Image(picture2,header2,info2);
    fclose(file2);
    return 0;
    
}