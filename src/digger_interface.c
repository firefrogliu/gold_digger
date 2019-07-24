#include "digger_interface.h"
#include "join_pic_detect.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define CFG "yolov3.cfg"
#define WEIGHTS_FILE "yolov3.weights"
#define MAX_THREAD_NUM 10

struct thread_args {
    int rand_seed;
    unsigned char* result;
    const char** picNames;
    void* network_ptr;
};


struct thread_stats{
    pthread_t thread;
    int started;
    int finished;
    int read;
    int canceled;
    pthread_cond_t cond;     
};

struct thread_stats THREADS_STATS[MAX_THREAD_NUM];

// // Declaration of thread condition variable 
// pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER; 
  
// declaring mutex 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

void* init_yolov3_data(){
    void* net = initNetwork(CFG, WEIGHTS_FILE);    
    return net;
}

void enter_to_continue(){
    printf("Press enter to continue\n");
    char enter = 0;
    while (enter != '\r' && enter != '\n') { enter = getchar(); }
    printf("Thank you for pressing enter\n");
}

struct thread_stats * find_thread_stats(pthread_t thread){
    for(int i = 0; i < MAX_THREAD_NUM; i++){
        struct thread_stats* sts = &THREADS_STATS[i];
        if(sts->thread == thread)
            return sts;
    }
    return NULL;   
}

void* thread_func(void* _args){
    /* set thread cancel type to asynchronous to make thread quit as soon as possible */
    int rc = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    printf("pthread_setcanceltype() %lu\n", rc);

    pthread_t  self;
    self = pthread_self();
    printf("creating thread %lu\n", self);

    /* set thread status */ 
    struct thread_stats* sts = NULL;
    for(int i = 0; i < MAX_THREAD_NUM; i++){
        sts = &THREADS_STATS[i];
        if((!sts->started) || (sts->read) || (sts->canceled)){
            printf("set thread %lu stats\n", self);
            sts->thread = self;
            sts->started = 1;
            sts->finished = 0;
            sts->read = 0;
            sts->canceled = 0;
            sts->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER; 
            break;            
        }
    }

    if(sts == NULL){
        printf("cannot creat new thread, thread pool full\n");
        return;
    }
    struct thread_args *args = (struct thread_args *) _args;
    int rand_seed =  args->rand_seed;            
    unsigned char* result = args->result;     
    void* network_ptr = args->network_ptr;
    const char* picNames = args->picNames;
    free(_args);
    pthread_mutex_lock(&lock); 
    int succeed  = join_pic_detect(rand_seed, picNames, result, network_ptr, self); 
    sts->finished = 1;     
    pthread_cond_wait(&(sts->cond), &lock);     
    pthread_mutex_unlock(&lock);
    if(succeed)
        pthread_exit(result);
    else
        pthread_exit(NULL);
}

unsigned char* wait_for_thread(pthread_t thread){
    unsigned char* t_result;    
    printf("getting %lu thread reasult\n", thread);
    pthread_join(thread, &t_result);    
    printf("got %lu thread reasult\n", thread);
    return t_result;
}

pthread_t creat_thread(int rand_seed, const char** picNames, void* network_ptr){
    
    struct thread_args *args = malloc (sizeof (struct thread_args));
    args->rand_seed = rand_seed;
    args->result = malloc(32);
    args->picNames = picNames;
    args->network_ptr = network_ptr;

    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, args);          
    return thread;    
}

void cancel_thread(pthread_t thread){
    
    struct thread_stats* sts = find_thread_stats(thread);
    if(sts == NULL){
        printf("thread does not exist\n");
        return;
    }    
    pthread_cancel(thread);
    sts->canceled = 1;
    printf("cancelled a thread %lu\n", thread);

}



int get_result(pthread_t thread, unsigned char* result){
    // for(int i = 0; i < MAX_THREAD_NUM; i++){
    //     struct thread_stats sts = THREADS_STATS[i];
    //     printf("thread %lu started %lu finished %lu\n", sts.thread, sts.started, sts.finished);
    // }    
    
    struct thread_stats* sts = find_thread_stats(thread);
    if(sts == NULL){
        printf("thread %lu does not exist\n", thread);
        return 0;
    }
    else if (!sts->started){
        printf("thread %lu not started yet\n", thread);
        return 0;        
    }

    else if(!sts->finished){
        printf("thread %lu not finished yet\n", thread);
        return 0;
    }
    else if(sts->read){
        printf("thread %lu has been read\n", thread);
        return 0;
    }
    else if (sts->canceled){
        printf("thread %lu canceled\n", thread);
        return 0;
    }
    else{
        printf("Signaling thread %lu to wake\n", thread); 
        pthread_cond_signal(&(sts->cond));
        unsigned char* tmp_result = wait_for_thread(thread);
        memcpy(result, tmp_result, 32);
        sts->read = 1;
        free(tmp_result);
        return 1;
    }
}


void test(){
    for(int i = 0; i < MAX_THREAD_NUM;i++){
        struct thread_stats* sts = &THREADS_STATS[i];
        sts->thread = i;
        sts->started = 1;
        sts->finished = 0;
    }
    for(int i = 0; i < MAX_THREAD_NUM; i++){
        struct thread_stats sts = THREADS_STATS[i];
        printf("thread %lu started %lu finished %lu\n", sts.thread, sts.started, sts.finished);
    }
}