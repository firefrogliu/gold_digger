#ifndef JOIN_PIC_H
#define JOIn_PIC_H
#include <stdio.h>


#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
EXTERNC int join_pics(int rand_seed, int width,int height,int divide_x,int divide_y,const char* pics_path, const char* join_pic_name);
#endif