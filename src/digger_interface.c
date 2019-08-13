#include "digger_interface.h"
#include "join_pic_detect.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "sclog4c/sclog4c.h"
#include "md5.h"
#include <time.h>
#include <string.h>

#define MAX_THREAD_NUM 10

#define WEIGHTS_FILE_MD5 {0xc8, 0x4e, 0x5b, 0x99, 0xd0, 0xe5, 0x2c, 0xd4, 0x66, 0xae, 0x71, 0x0c, 0xad, 0xf6, 0xd8, 0x4c}
#define CFG_MD5 {0x9b, 0x7d, 0x21, 0xd6, 0xbb, 0xf6, 0x3a, 0x7c, 0xa9, 0xb6, 0x38, 0x4d, 0x6c, 0xf6, 0x4a, 0x2e}
#define COCO_NAME_MD5 {0x8f, 0xc5, 0x5, 0x61, 0x36, 0x1f, 0x8b, 0xcf, 0x96, 0xb0, 0x17, 0x70, 0x86, 0xe7, 0x61, 0x6c}

extern char* WEIGHTS_FILE;

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

void* init_yolov3_data(const char* weight_file, const char* cfg, const char* coco_names){
    const unsigned char weight_file_md5[] = WEIGHTS_FILE_MD5;
    const unsigned char cfg_md5[] = CFG_MD5;
    const unsigned char coco_name_md5[] = COCO_NAME_MD5;
    int validate = 0;
    WEIGHTS_FILE = malloc(256);    
    COCONAME = malloc(256);
    CFG = malloc(256);


    strcpy(WEIGHTS_FILE,weight_file);
    strcpy(CFG,cfg);
    strcpy(COCONAME,coco_names);

    // logm(SL4C_DEBUG,"weight file is %s\n", WEIGHTS_FILE);
    // logm(SL4C_DEBUG,"CFG file is %s\n", CFG);
    // logm(SL4C_DEBUG,"COCONAME file is %s\n", COCONAME);
    // enter_to_continue();
    // logm(SL4C_DEBUG,"im here\n");

    validate = validate_md5(WEIGHTS_FILE,weight_file_md5);
    if(validate != 1){
        logm(SL4C_DEBUG, "weight file currupted\n");
        return NULL;
    }
    else{
        logm(SL4C_DEBUG, "weight file is correct!\n");
    } 
   
    validate = validate_md5(CFG,cfg_md5);
    if(validate != 1){
        logm(SL4C_DEBUG, "cfg file currupted\n");
        return NULL;
    }
    else{
        logm(SL4C_DEBUG, "cfg file is correct!\n");
    } 
    validate = validate_md5(COCONAME,coco_name_md5);
    if(validate != 1){
        logm(SL4C_DEBUG, "coco name file currupted\n");
        return NULL;
    }
    else{
        logm(SL4C_DEBUG, "coco name file is correct!\n");
    }

    void* net = initNetwork(CFG, WEIGHTS_FILE);    
    return net;
}

void enter_to_continue(){
    logm(SL4C_DEBUG, "Press enter to continue\n");
    char enter = 0;
    while (enter != '\r' && enter != '\n') { enter = getchar(); }
    logm(SL4C_DEBUG, "Thank you for pressing enter\n");
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
    logm(SL4C_DEBUG, "pthread_setcanceltype() %lu\n", rc);

    pthread_t  self;
    self = pthread_self();
    logm(SL4C_DEBUG, "creating thread %lu\n", self);

    /* set thread status */ 
    struct thread_stats* sts = NULL;
    for(int i = 0; i < MAX_THREAD_NUM; i++){
        sts = &THREADS_STATS[i];
        if((!sts->started) || (sts->read) || (sts->canceled)){
            logm(SL4C_DEBUG, "set thread %lu stats \n", self);
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
        logm(SL4C_DEBUG, "cannot creat new thread, thread pool full\n");
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
    logm(SL4C_DEBUG, "getting %lu thread reasult\n", thread);
    pthread_join(thread, &t_result);    
    logm(SL4C_DEBUG, "got %lu thread reasult\n", thread);
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
        logm(SL4C_DEBUG, "thread does not exist\n");
        return;
    }    
    pthread_cancel(thread);
    sts->canceled = 1;
    logm(SL4C_DEBUG, "cancelled a thread %lu\n", thread);

}



int get_result(pthread_t thread, unsigned char* result){
    // for(int i = 0; i < MAX_THREAD_NUM; i++){
    //     struct thread_stats sts = THREADS_STATS[i];
    //     logm(SL4C_DEBUG, "thread %lu started %lu finished %lu", sts.thread, sts.started, sts.finished);
    // }    
    char buffer[26];
    time_t timer;
    time(&timer);
    struct tm* tm_info;
    tm_info = localtime(&timer);
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    logm(SL4C_DEBUG, "time is %s\n", buffer);

    struct thread_stats* sts = find_thread_stats(thread);
    if(sts == NULL){
        logm(SL4C_DEBUG, "thread %lu does not exist\n", thread);
        return 0;
    }
    else if (!sts->started){
        logm(SL4C_DEBUG, "thread %lu not started yet\n", thread);
        return 0;        
    }

    else if(!sts->finished){
        logm(SL4C_DEBUG, "thread %lu not finished yet\n", thread);
        return 0;
    }
    else if(sts->read){
        logm(SL4C_DEBUG, "thread %lu has been read\n", thread);
        return 0;
    }
    else if (sts->canceled){
        logm(SL4C_DEBUG, "thread %lu canceled\n", thread);
        return 0;
    }
    else{
        logm(SL4C_DEBUG, "Signaling thread %lu to wake\n", thread); 
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
        logm(SL4C_DEBUG, "thread %lu started %lu finished %lu\n", sts.thread, sts.started, sts.finished);
    }
}