#ifndef DIGGER_INTERFACES_H
#define DIGGER_INTERFACES_H
#include <pthread.h>


pthread_t creat_thread(int rand_seed, const char** picNames, void* network_ptr);
void cancel_thread(pthread_t thread);
unsigned char* get_result(pthread_t thread);

void* init_yolov3_data();
#endif

