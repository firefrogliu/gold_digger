#include "digger_interface.h"
#include "join_pic_detect.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

struct thread_args {
    int rand_seed;
    unsigned char* result;
    const char** picNames;
    void* network_ptr;
};


#define CFG "yolov3.cfg"
#define WEIGHTS_FILE "yolov3.weights"

int thread_finished = 0;
int thread_been_read  = 0;

// Declaration of thread condition variable 
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER; 
  
// declaring mutex 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

void* init_yolov3_data(){
    void* net = initNetwork(CFG, WEIGHTS_FILE);    
    return net;
}

void* thread_func(void* _args){
    thread_finished = 0;
    pthread_mutex_lock(&lock); 
    struct thread_args *args = (struct thread_args *) _args;
    int rand_seed =  args->rand_seed;        
    //printf("rand_seed in thread is %d\n", rand_seed);    
    unsigned char* result = args->result; 
    
    void* network_ptr = args->network_ptr;
    const char* picNames = args->picNames;
    int succeed  = join_pic_detect(rand_seed, picNames, result, network_ptr);
    printf("\n");
    thread_finished = 1;
    //printf("Waiting on condition variable cond1\n"); 
    pthread_cond_wait(&cond1, &lock); 
    //print_bytes(result, 32, "result in thread");
    pthread_mutex_unlock(&lock); 
    //printf("thread quiting\n");
    //printf("arg result p %p\n",result);
    free(_args);
    if(succeed)
        pthread_exit(result);
    else
        pthread_exit(NULL);
}

unsigned char* wait_for_thread(pthread_t thread){
    unsigned char* t_result;    
    pthread_join(thread, &t_result);    
    //print_bytes(t_result, 32, "result in thread return");
    //printf("t_result result p %p\n",t_result);
    return t_result;
}

pthread_t creat_thread(int rand_seed, const char** picNames, void* network_ptr){
    thread_been_read = 0;
    struct thread_args *args = malloc (sizeof (struct thread_args));
    args->rand_seed = rand_seed;
    args->result = malloc(32);
    args->picNames = picNames;
    args->network_ptr = network_ptr;

    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, args);  
    //free(args);
    return thread;    
}

void cancel_thread(pthread_t thread){
    
    printf("cancelling a thread %d\n", thread);
    pthread_cancel(thread);
    printf("cancelled a thread %d\n", thread);
}

unsigned char* get_result(pthread_t thread){
    unsigned char* result = NULL;
    
    if(thread_been_read){
        printf("thread has been read\n");
        return NULL;
    }

    if(!thread_finished){
        printf("thread not finished yet\n");
        return NULL;
    }
    else{
        printf("Signaling thread to wake\n"); 
        pthread_cond_signal(&cond1);
        result = wait_for_thread(thread);
        // printf("result %p\n", result);
        // print_bytes(result, 32, "result in interface");
        thread_been_read = 1;
        return result;
    }
}

