#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string.h>
#include<pthread.h>
#include "kmalloc.h"
#include "translation.h"
#include "frame.h"
#include "ll_double.h"


#define MIN(X, Y) (X < Y ? X : Y);

int main(){
    // frame_init();
    // char* frame_head = palloc(1);
    // pfree(frame_head, 1);

    // char* address = kmalloc(2000);
    // char* address2 = kmalloc(4000);
    // char* address3 = krealloc(address, 4000);
    // // kfree(address);
    // // kfree(address2);
    // printf("%llu\n",*((uint64_t *)(address - 2 * sizeof(uint64_t))));
    // printf("allocated:%ld\n", frames_allocated);

    char *address1 = (char *) kmalloc(2000);
    //printf("%llu\n", (uint64_t)address1);
    printf("%llu\n", *(uint64_t *)(address1-16));

    char *address2 = (char *) kmalloc(4000);
    printf("%llu\n", *(uint64_t *)(address2-16));

    printf("size check %llu\n", *((uint64_t *)(address1+2000)));

    char *address3 = (char *) krealloc(address1,1000);
    //printf("%llu\n", (uint64_t)address3);
    printf("size3:%llu\n", *((uint64_t *)(address3-16)));

    printf("%llu\n", *((uint64_t *)(address1-16)));
    printf("allocated:%ld\n", frames_allocated);

    // srand(time(NULL));

	// // Allocate at most 10K in total
	// int remaining_bytes = 41960;

	// // Keeps track of the allocated chunks
	// struct list allocated_chunks;

	// ll_init(&allocated_chunks);
	// frame_init();
    // int count = 0;
	// while(remaining_bytes > 0) {
    //     count++;
	// 	// Allocate between 1 and 4K bytes, up to the maximum of remaining bytes
	// 	int chunk_size = MIN(remaining_bytes, (rand() % 4096) + 1);
    //     printf("chunck%d:%d\n",count,chunk_size);


	// 	char *memory = kmalloc(chunk_size);
    //     printf("allocatedchunck%d:%llu\n",count,*((uint64_t *)(memory - 2 * sizeof(uint64_t))));

	// 	if(!memory) {
	// 		fprintf(stderr, "Memory is full (this is OK with limited memory), exiting");
	// 		fflush(stdout);
	// 		break;
	// 	}

	// 	remaining_bytes -= chunk_size;
	// 	ll_insert_head(&allocated_chunks, memory);
	// }
    // char *memory;
    // memory = kmalloc(40);
    // ll_insert_head(&allocated_chunks, memory);
    // memory = kmalloc(2621);
    // ll_insert_head(&allocated_chunks, memory);
    // memory = kmalloc(1537);
    // ll_insert_head(&allocated_chunks, memory);
    // struct node *chunck;
    // chunck = ll_remove_tail(&allocated_chunks);
    // printf("location:%llu\n", (uint64_t)chunck->data);
    // printf("allocatedchunck%d:%llu\n",1,*((uint64_t *)(chunck->data - 2 * sizeof(uint64_t))));
    // kfree(chunck->data);
    // chunck = ll_remove_tail(&allocated_chunks);
    // printf("location:%llu\n", (uint64_t)chunck->data);
    // printf("allocatedchunck%d:%llu\n",2,*((uint64_t *)(chunck->data - 2 * sizeof(uint64_t))));
    // kfree(chunck->data);
    // chunck = ll_remove_tail(&allocated_chunks);
    // printf("location:%llu\n", (uint64_t)chunck->data);
    // printf("allocatedchunck%d:%llu\n",3,*((uint64_t *)(chunck->data - 2 * sizeof(uint64_t))));
    // kfree(chunck->data);

    // for(int i = 0; i < 3; i ++){
    //     count++;
	// 	// Allocate between 1 and 4K bytes, up to the maximum of remaining bytes
	// 	int chunk_size = MIN(remaining_bytes, (rand() % 4096) + 1);
    //     printf("chunck%d:%d\n",count,chunk_size);

	// 	char *memory;
    //     memory = kmalloc(chunk_size);
    //     printf("location:%llu\n", (uint64_t)memory);
    //     printf("allocatedchunck%d:%llu\n",count,*((uint64_t *)(memory - 2 * sizeof(uint64_t))));
    //     ll_insert_head(&allocated_chunks, memory);
    // }
    // printf("total:%d\n",count);
    // for(int i = 0; i < 3; i ++){
	// 	struct node *memory;
    //     memory = ll_remove_tail(&allocated_chunks);
    //     printf("location:%llu\n", (uint64_t)memory->data);
    //     printf("allocatedchunck%d:%llu\n",i+1,*((uint64_t *)(memory->data - 2 * sizeof(uint64_t))));
    //     // if(i == 0){
    //     kfree(memory->data);
    //     printf("freedchunck%d:%llu\n",i+1,*((uint64_t *)(memory->data - 2 * sizeof(uint64_t))));

    //     // }
    // }
	// Frees all data previously allocated
	// struct node *first;
    // for(int i = 0; i < 9; i++){
        
    //     first = ll_remove_head(&allocated_chunks);
    //     printf("chunck%d:%llu\n", i+1, *((uint64_t *)(first->data - 2 * sizeof(uint64_t))));
    //     kfree(first->data);
    // }
    // first = ll_remove_head(&allocated_chunks);
    // printf("%llu\n",*((uint64_t *)(first->data - 2 * sizeof(uint64_t))));

    
	// while((first = ll_remove_head(&allocated_chunks))) {
	// 	kfree(first->data);
	// }

	printf("Exiting without crashing. That doesn't mean the program is right. Please check with debug.\n");
	return EXIT_SUCCESS;
}