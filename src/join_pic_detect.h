#ifndef JOIN_PIC_DETECT_H
#define JOIN_PIC_DETECT_H
void print_bytes(unsigned char* bytes, int len, char* name);
void print_hello_join_pic_detect();
int join_pic_detect(int rand_seed, const char** picNames,unsigned char* result, void* network_ptr);
void* initNetwork(char *cfgfile,char *weightfile);
#endif