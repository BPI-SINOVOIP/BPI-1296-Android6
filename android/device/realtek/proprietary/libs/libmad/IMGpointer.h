#ifndef __IMGPOINTER_H
#define __IMGPOINTER_H

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct _IMGpointer2
{
    int field;
    int data_format;
    int image_width;
    int image_height;
    int internal_index;
    unsigned char *dpY;
    unsigned char *dpUV;
} IMGpointer2;

typedef struct _IMGpointer3
{
    int field;
    int data_format;
    int image_width;
    int image_height;
    int image_Ystride;
    int image_UVstride;
    int internal_index;
    unsigned char *img;
    unsigned char *dpY;
    unsigned char *dpUV;
} IMGpointer3;

void img3_init(IMGpointer3* imgout, int width, int height);

void img_init(IMGpointer2* img, int width, int height);
void img_output(IMGpointer2* img, char * file_name);
void img_output_by_prefix(IMGpointer2* img, const char * file_prefix, const int file_index);
void img_output_y4(char * file_name, const IMGpointer2* ptr );
void img_output_uv20(char * file_name, IMGpointer2* ptr  );
void img_open(IMGpointer3* img, char * file_name);
void img_open_by_prefix(IMGpointer3* img, const char * file_prefix, const int file_index);
void img_open_y4(char * file_name, const IMGpointer3* ptr );
void img_open_uv20(char * file_name, IMGpointer3* ptr  );
#ifdef __cplusplus
}
#endif
#endif


