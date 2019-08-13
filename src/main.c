#include "digger_interface.h"
#include "join_pic_detect.h"
#include <stdio.h>
#include <unistd.h>
#include "sclog4c/sclog4c.h"
#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

void sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}


int main(int argc, char **argv){  
    // test();
    // return 0;
    if(argc < 2){
        fprintf("usage: %s <function>, rand_seed\n", argv[0]);
        return 0;
    }
    //sclog4c_level = SL4C_ALL;
    int rand_seed = strtol(argv[1], NULL, 10);   
    // logm(SL4C_DEBUG,"Program name: %s, %d", argv[0], rand_seed);
    // logm(SL4C_DEBUG,"logging sth\n");
    // return 0;

    const char* picNames[] = {"./16_testPics/00d66ed55093c3bf.jpg",  "./16_testPics/0222359686b52503.jpg",  "./16_testPics/03b34394c4fae1d2.jpg",  "./16_testPics/0574623c2473a463.jpg",  "./16_testPics/076c438efda49fac.jpg"  ,"./16_testPics/0973221d1bc979c1.jpg",  "./16_testPics/0b96750f7bfbef43.jpg",  "./16_testPics/0dc5f1cf71842cbe.jpg",
"./16_testPics/00d67ab9e6db2059.jpg",  "./16_testPics/0222397d2ce9241e.jpg",  "./16_testPics/03b351e2faa608fe.jpg",  "./16_testPics/057463e74cc756bd.jpg",  "./16_testPics/076c44dc65599558.jpg",  "./16_testPics/097335b71ef0ebca.jpg",  "./16_testPics/0b967884421ea018.jpg",  "./16_testPics/0dc6006b96ae1213.jpg"
};
    void* network_ptr = init_yolov3_data("yolov3.weights","yolov3.cfg", "coco.names");
  
    logm(SL4C_DEBUG,"finished loading network\nNow lets detect\n");


    unsigned long thread = creat_thread(rand_seed, picNames,network_ptr);
    //sleep(2);
    //unsigned long thread1 = creat_thread(rand_seed, picNames,network_ptr);
    //enter_to_continue();
    int tic = 0;
    unsigned char* result = malloc(32);
    unsigned char* result1 = malloc(32);


    

    while(tic < 200){
        // if(time == 5){
        //     cancel_thread(thread);
        //     //break;
        // }

        char buffer[26];
        time_t timer;
        time(&timer);
        struct tm* tm_info;
        tm_info = localtime(&timer);
        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);                
        int succeed = get_result(thread, result);
        if(succeed){
            logm(SL4C_DEBUG,"result %p\n", result);
            logm(SL4C_DEBUG,"lets print result in main\n");  
            print_bytes(result, 32, "result in main");
            break;
        }
        
        // int succeed1 = get_result(thread1, result1);
        // if(succeed1){
        //     logm(SL4C_DEBUG,"result %p\n", result1);
        //     logm(SL4C_DEBUG,"lets print result in main\n");  
        //     print_bytes(result1, 32, "result1 in main");
        // }
        //sleep(1);

        sleep_ms(100);

        tic++;
    }
    
    //logm(SL4C_DEBUG,"cant wait any more, abort!\n");
    //cancel_thread(thread);
    //wait_for_thread(thread);
    
    return 0;    
}
