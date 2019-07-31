#include "digger_interface.h"
#include "join_pic_detect.h"
#include <stdio.h>
#include <unistd.h>


int main(int argc, char **argv){  
    // test();
    // return 0;
    if(argc < 2){
        fprintf( "usage: %s <function>, rand_seed\n", argv[0]);
        return 0;
    }

    int rand_seed = strtol(argv[1], NULL, 10);   
    const char* picNames[] = {"./16_testPics/00d66ed55093c3bf.jpg",  "./16_testPics/0222359686b52503.jpg",  "./16_testPics/03b34394c4fae1d2.jpg",  "./16_testPics/0574623c2473a463.jpg",  "./16_testPics/076c438efda49fac.jpg"  ,"./16_testPics/0973221d1bc979c1.jpg",  "./16_testPics/0b96750f7bfbef43.jpg",  "./16_testPics/0dc5f1cf71842cbe.jpg",
"./16_testPics/00d67ab9e6db2059.jpg",  "./16_testPics/0222397d2ce9241e.jpg",  "./16_testPics/03b351e2faa608fe.jpg",  "./16_testPics/057463e74cc756bd.jpg",  "./16_testPics/076c44dc65599558.jpg",  "./16_testPics/097335b71ef0ebca.jpg",  "./16_testPics/0b967884421ea018.jpg",  "./16_testPics/0dc6006b96ae1213.jpg"
};
    void* network_ptr = init_yolov3_data();
  
    printf("finished loading network\nNow lets detect\n");


    unsigned long thread = creat_thread(rand_seed, picNames,network_ptr);
    sleep(2);
    unsigned long thread1 = creat_thread(rand_seed, picNames,network_ptr);
    //enter_to_continue();
    int time = 0;
    unsigned char* result = malloc(32);
    unsigned char* result1 = malloc(32);
    while(time < 20){
        printf("time is %d\n", time);
        // if(time == 5){
        //     cancel_thread(thread);
        //     //break;
        // }

       
        int succeed = get_result(thread, result);
        if(succeed){
            printf("result %p\n", result);
            printf("lets print result in main\n");  
            print_bytes(result, 32, "result in main");
        }
        
        int succeed1 = get_result(thread1, result1);
        if(succeed1){
            printf("result %p\n", result1);
            printf("lets print result in main\n");  
            print_bytes(result1, 32, "result1 in main");
        }
        sleep(1);
        time++;
    }
    
    //printf("cant wait any more, abort!\n");
    //cancel_thread(thread);
    //wait_for_thread(thread);
    
    return 0;    
}
