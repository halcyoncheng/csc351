#include <stdio.h>
#include "translation.h"
#include "frame.h"

void main(){
    
    frame_init();
    // int available = 1024;
    // int allocated = 0;
    // int frame = 0;

    // int64_t first = allocate_frame(16);
    // available -= 16;
    // allocated +=16;
    // printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    // first = allocate_frame(16);
    // available -= 16;
    // allocated +=16;
    // printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);
    for(uint64_t i = 0; i < 1024; i++){
        int result = vm_map(i, i, 1, 0);
        // printf("%d\n", result);
    }
    uint64_t result = vm_locate(513);
    printf("locate:%llu\n", result);
    uint64_t address = vm_translate(4097ULL);
    printf("%llu\n", address);
    address = vm_translate(4098ULL);
    printf("%llu\n", address);
    for(uint64_t i = 0; i < 512; i++){
        int result = vm_unmap(i, 1);
        // printf("%d\n", result);
    }
    result = vm_locate(512);
    printf("locate:%llu\n", result);
    // int result = vm_map(0ULL, 0ULL, 1, 0);
    // printf("%d\n", result);

}
