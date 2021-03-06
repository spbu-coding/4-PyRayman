#include "bmp.h"

//////////////////////////////////// READ IMAGE DATA ////////////////////////////////////


RGB **read_rgb(FILE *fp, int32_t width, int32_t height, int16_t bpp){
    height = abs(height);
    RGB **rgb  = malloc(sizeof(void*) * height);
    if(!rgb){
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return 0;
    }  
    
    for (int32_t i = 0; i < height; i++) {
        rgb[i] = malloc(sizeof(RGB) * width);
        if (!rgb[i]){
            fprintf(stdout, "Cannot allocate memory for results string in %d experiment\n", i);
            for (int16_t j = 0; j < i; j++) {
                free(rgb[i]);
            }
            free(rgb);
            return 0;
        }
        fread(rgb[i],width , sizeof(RGB), fp);
    }
 
 
    return rgb;
}

int8_t *read_palette(FILE *fp, int32_t colors){
    int16_t table_size = colors * 4;// colors_used * bytes of color table
    int8_t *palette = malloc(sizeof(int) * table_size);
    if(!palette){
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return 0;
    }
    for(int32_t i = 0; i < table_size; i++){
        palette[i] = 0;
        fread(&palette[i],1,1,fp);
    }
    return palette;
} 


IMAGE *read_Image(FILE *fp,BMPINFOHEADER *info,IMAGE *picture){

    picture->info = info;
    if(!picture->info){
        printf("Not infoheader error\n");
        free(picture);
        return NULL;
    }
    
    if (picture->info->bpp == 8) {
        picture->palette = read_palette(fp, picture->info->color_used);
        if (!picture->palette) {
            fprintf(stdout, "Invalid Palette \n");
            free(picture);
            return NULL;
        }
    }
    
    picture->rgb = read_rgb(fp,picture->info->width,picture->info->height,picture->info->bpp);

    if(!picture->rgb){
        fprintf(stdout, "Invalid RGB data");
        free(picture);
        return NULL;
    }

    return picture;
}

void rgb_convert(IMAGE *picture){
    for(int32_t i = 0; i < abs(picture->info->height); i++){
        for(int32_t j = 0; j < picture->info->width; j++){
            picture->rgb[i][j].red = ~picture->rgb[i][j].red;
            picture->rgb[i][j].green = ~picture->rgb[i][j].green;
            picture->rgb[i][j].blue = ~picture->rgb[i][j].blue;
        }
    }
}

void palette_convert(IMAGE *picture){
    for(int32_t i = 0; i < picture->info->color_used * 4; i++ ){
        if ((i + 1) % 4 != 0) {
            picture->palette[i] = ~picture->palette[i];
        }
    }
}

void create_BMPImage(BMPFILEHEADER *header,BMPINFOHEADER *info, IMAGE *picture,char *output){
    FILE *fp = fopen(output,"wb");
    if (fp == NULL){
        fprintf(stdout,"Cannot create file \"%s\"",output);
    }
    fwrite(header,sizeof(BMPFILEHEADER),1,fp);
    fwrite(info,sizeof(BMPINFOHEADER),1,fp);
    if ( picture->info->bpp == 8) {
        for (int32_t i = 0; i < picture->info->color_used * 4; i++) {
            fwrite(&picture->palette[i], 1, 1, fp);
        }
    }
    
    for (int32_t i = 0; i < abs(picture->info->height); i++) { 
        fwrite(picture->rgb[i],picture->info->width , sizeof(RGB), fp);
    } 
    
    fclose(fp);
}

void free_Image(IMAGE *picture, BMPFILEHEADER *header){
    
    for (int32_t i = 0; i < abs(picture->info->height); i++){
        free(picture->rgb[i]);    
    }
    free(picture->info);
    
    if(picture->palette != NULL){
        free(picture->palette);
    }

    free(picture->rgb);
    free(picture);
    free(header);
}

////////////////////////////////////READ_BMP////////////////////////////////////

int read_BMP(FILE *fp, BMPINFOHEADER *info, BMPFILEHEADER *header, IMAGE *picture){
    fread(header,sizeof(BMPFILEHEADER),1,fp); 
    fread(info,sizeof(BMPINFOHEADER),1,fp);
    

    if (header->file_type != 19778){
        fprintf(stdout,"Error format. The converter only supports BMP format.");
        fclose(fp);
        return -1;
       
    }
    if (info->planes != 1){
        fprintf(stdout, "Error planes. The number of planes must be 1.");
        fclose(fp);
        return -2;
        
    }
    if (info->bpp != 8 && info->bpp != 24){
        fprintf(stdout,"Error bits per pixel error. The converter only supports 8 and 24 bits per pixel images.");
        fclose(fp);
        return -2;
    }
    if(info->compression != 0){
        fprintf(stdout, "Error compression. Only uncompressed images are supported");
        fclose(fp);
        return -2;
    }
    if(header->reserved1 || header->reserved2){
        fprintf(stdout,"Broken file. File offset 0x06, 4 bytes: expected to get value 0");
        fclose(fp);
        return -2;
    }
    if(info->header_size != 40){
        fprintf(stdout,"Not supported BMP version - incorrect header size. Expected to get value 40");
        fclose(fp);
        return -2;
        
    }
    picture = read_Image(fp,info,picture);
    
    if(!picture){
        fclose(fp);
        return -1;
    }
    return 0;   
}


int mine_realisation(char* input_name, char* output_name){
    FILE *fp = fopen(input_name,"rb");
    if (fp == NULL) {
        fprintf(stdout, "Cannot open file. No file with name %s exists.\n", input_name);
        return -1;
    }

    BMPINFOHEADER *info = malloc(sizeof(BMPINFOHEADER));
    BMPFILEHEADER *header  = malloc(sizeof(BMPFILEHEADER));
    IMAGE *picture = malloc(sizeof(IMAGE));
    if (!picture) {
        fprintf(stdout, "Cannot allocate memory for results string in all experiment\n");
        return -1;
    }
    
    int output = read_BMP(fp,info,header,picture);
    
    if(output != 0 ){
        free_Image(picture,header);
        fclose(fp);       
        return output;
    }
    
    if ( info->bpp == 8){
        palette_convert(picture);
    }
    else{
        rgb_convert(picture);
    }
    
    create_BMPImage(header,info,picture,output_name);
    free_Image(picture,header);
    fclose(fp); 

    return 0;
}

/////////////////////////////////////// COMPARE_FUNC ///////////////////////////////////////

int compare_tables(IMAGE *picture1, IMAGE *picture2){  
    for(int32_t i = 0; i < picture1->info->color_used; i++ ){
        if(picture1->palette[i] != picture2->palette[i]){
            printf("Table colors error");
            return -1;
        } 
    }
    return 0;
}

bool check_colors(RGB pixel1, RGB pixel2){
    return (pixel1.red == pixel2.red && pixel1.green == pixel2.green && pixel1.blue == pixel2.blue);
}

void compare_pixels(IMAGE *picture1, IMAGE *picture2) {
    int count_of_pixels = 0;
    int32_t height = abs(picture1->info->height);
    int32_t height_1 = picture1->info->height;
    int32_t height_2 = picture2->info->height;

    for (int32_t i = 0; i < height; i++){
        for(int32_t j = 0; j < picture1->info->width; j++){

            int32_t abs_i_1 = height_1 > 0 ? i : abs(height_1) - i - 1;
            int32_t abs_i_2 = height_2 > 0 ? i : abs(height_2) - i - 1;
            if(!check_colors(picture1->rgb[abs_i_1][j], picture2->rgb[abs_i_2][j]) ){
                fprintf(stderr, "%d %d \n",j, i);
                count_of_pixels++;
                
            }
            if (count_of_pixels >= 100) break;
        }   
        if(count_of_pixels >= 100) break;
    }
}

int mine_comparer(char* input1, char* input2){
    FILE *file1 = fopen(input1,"rb");
    if (file1 == NULL) {
        fprintf(stdout, "Cannot open file. No file with name %s exists.\n", input1);
        return -1;
    }

    FILE *file2 = fopen(input2,"rb");
    if (file2 == NULL) {
        fprintf(stdout, "Cannot open file. No file with name %s exists.\n", input2);
        return -1;
    }

    BMPINFOHEADER *info1 = malloc(sizeof(BMPINFOHEADER));
    BMPFILEHEADER *header1  = malloc(sizeof(BMPFILEHEADER));
    IMAGE *picture1 = malloc(sizeof(IMAGE));

    BMPINFOHEADER *info2 = malloc(sizeof(BMPINFOHEADER));
    BMPFILEHEADER *header2  = malloc(sizeof(BMPFILEHEADER));
    IMAGE *picture2 = malloc(sizeof(IMAGE));
    
    int output1 = read_BMP(file1,info1,header1,picture1);
    int output2 = read_BMP(file2,info2,header2,picture2);
    
    if(output1 != 0){
        free_Image(picture1,header1);
        free_Image(picture2,header2);
        fclose(file1);
        fclose(file2);
        return output1;
    }
    if(output2 != 0){
        free_Image(picture1,header1);
        free_Image(picture2,header2);
        fclose(file1);
        fclose(file2);
        return output2;
    }
    
   
    if ((abs(picture1->info->width) != abs(picture2->info->width)) ||
        (abs(picture1->info->height) != abs(picture2->info->height))) {
        fprintf(stdout, "Images have different values of width or height. ");
        free_Image(picture1,header1);
        fclose(file1);
        free_Image(picture2,header2);
        fclose(file2); 
        return -1;

    } else if ((header1->size != header2->size)) {
        fprintf(stdout, "Images have different values of sizes. ");
        free_Image(picture1,header1);
        fclose(file1);
        free_Image(picture2,header2);
        fclose(file2); 
        return -1;
    
    } else if ((picture1->info->bpp != picture2->info->bpp)) {
        fprintf(stdout, "Images have different values of bits_per_pixel. ");
        free_Image(picture1,header1);
        fclose(file1);
        free_Image(picture2,header2);
        fclose(file2); 
        return -1;
    } else if ((picture1->info->color_used != picture2->info->color_used)) {
        fprintf(stdout, "Images have different number of colors in the table. ");
        free_Image(picture1,header1);
        fclose(file1);
        free_Image(picture2,header2);
        fclose(file2); 
        return -1;
    } 
    else {
    
        if(!compare_tables(picture1,picture2)){
            compare_pixels(picture1,picture2);
        }
        else{
            free_Image(picture1,header1);
            fclose(file1);
            free_Image(picture2,header2);
            fclose(file2); 
            return -1;
        }
    }
    
    free_Image(picture1,header1);
    fclose(file1);
    free_Image(picture2,header2);
    fclose(file2);
   
    return 0; 
}



