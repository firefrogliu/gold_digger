#include "digger_interface.h"
#include "join_pic_detect.h"
#include <stdio.h>
#include <unistd.h>


int main(int argc, char **argv){    
    if(argc < 2){
        fprintf( "usage: %s <function>, rand_seed\n", argv[0]);
        return 0;
    }

    int rand_seed = strtol(argv[1], NULL, 10);   
    const char* picNames[] = {"./testPics/00d66ed55093c3bf.jpg",  "./testPics/0222359686b52503.jpg",  "./testPics/03b34394c4fae1d2.jpg",  "./testPics/0574623c2473a463.jpg",  "./testPics/076c438efda49fac.jpg"  ,"./testPics/0973221d1bc979c1.jpg",  "./testPics/0b96750f7bfbef43.jpg",  "./testPics/0dc5f1cf71842cbe.jpg",
"./testPics/00d67ab9e6db2059.jpg",  "./testPics/0222397d2ce9241e.jpg",  "./testPics/03b351e2faa608fe.jpg",  "./testPics/057463e74cc756bd.jpg",  "./testPics/076c44dc65599558.jpg",  "./testPics/097335b71ef0ebca.jpg",  "./testPics/0b967884421ea018.jpg",  "./testPics/0dc6006b96ae1213.jpg"
};
    void* network_ptr = init_yolov3_data();
  
    printf("finished loading network\nNow lets detect\n");


    unsigned long thread = creat_thread(rand_seed, picNames,network_ptr);
    int time = 0;
    while(time < 20){
        if(time == 10){
            cancel_thread(thread);
            break;
        }

        unsigned char* result = get_result(thread);
        if(result != NULL){
            printf("result %p\n", result);
            printf("lets print result in main\n");  
            print_bytes(result, 32, "result in main");
        }
        sleep(1);
        time++;
    }
    
    //printf("cant wait any more, abort!\n");
    //cancel_thread(thread);
    //wait_for_thread(thread);
    
    return 0;    
}
