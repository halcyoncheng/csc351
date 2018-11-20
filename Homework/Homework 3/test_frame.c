#include <stdio.h>
#include "frame.h"


void main(){
    frame_init();
    int available = 1024;
    int allocated = 0;
    int frame = 0;

    int64_t first = allocate_frame(16);
    available -= 16;
    allocated +=16;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = allocate_frame(64);
    available -= 64;
    allocated += 64;
    frame = 16;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = deallocate_frame(1000,16);
    frame = -1;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = allocate_frame(128);
    available -= 128;
    allocated += 128;
    frame = 80;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = allocate_frame(817);
    frame = -1;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);
    
    first = allocate_frame(816);
    available -= 816;
    allocated += 816;
    frame = 208;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = allocate_frame(1);
    frame = -1;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = deallocate_frame(0,16);
    available += 16;
    allocated -= 16;
    frame = 0;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = deallocate_frame(15,1);
    frame = -1;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = deallocate_frame(16,1009);
    frame = -1;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = deallocate_frame(16,1008);
    available += 1008;
    allocated -= 1008;
    frame = 16;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = allocate_frame(1025);
    frame = -1;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = allocate_frame(1024);
    available -= 1024;
    allocated += 1024;
    frame = 0;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = deallocate_frame(1,1024);
    frame = -1;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = deallocate_frame(0,1024);
    available += 1024;
    allocated -= 1024;
    frame = 0;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = allocate_frame(64);
    available -= 64;
    allocated += 64;
    frame = 0;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = deallocate_frame(16,16);
    available += 16;
    allocated -= 16;
    frame = 16;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = allocate_frame(32);
    available -= 32;
    allocated += 32;
    frame = 64;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = allocate_frame(16);
    available -= 16;
    allocated += 16;
    frame = 16;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

    first = allocate_frame(1);
    available -= 1;
    allocated += 1;
    frame = 96;
    printf("Expected:%d,%d,%d Actual:%ld,%ld,%d\n",allocated,available,frame,frames_allocated,frames_available,first);

}