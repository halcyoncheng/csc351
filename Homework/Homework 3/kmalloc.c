#include "kmalloc.h"
#include "frame.h"
#include "translation.h"

// After you are done, you don't need to import it anymore. This is here just for the dummy code on malloc/free.
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define MAGIC_NUMBER 0xc0dec0de
#define FRAME_SIZE 4096

char *free_list_head = NULL;

void *palloc(uint64_t number) {
	// TODO:
	// 1) Allocate <parameter:number> frames of memory
	// frame_number = allocate_frame(...)
	// 2) Find the first page number (virtual) that is not mapped to a frame (physical), and <parameter:number> of them are consecutive
	// page_number = vm_locate(...)
	// 3) IGNORE the result of the previous call, and map the frame number to itself
	// page_number = FRAME_NUMBER (overwrites previous value, for the sake of simulation. See notes below.)
	// vm_map(page_number, frame_number, number, /* use flags = 0 */);
	// 4) Return the address of the first byte of the allocated page [see note below]
	// return PAGE_GET_ADDRESS(page_number);

	// NOTE:
	// - You are simulating the hardware automatic address translation in software.
	// - The palloc() function would return the address of the first virtual (mapped) address, based on the page number of step (2),
	//   but your application would not understand this since it only understands its own application addressing.
	// - Therefore, you are going to SIMULATE that there is another layer of automatic virtual addressing by
	//   always mapping page number X to the address of a frame number X
	// - Inside a real kernel, you wouldn't perform the overwrite of the page number in step (3),
	//   but the hardware would be performing the automatic translation between page_number to frame_number

	// Make sure, that in the end of this function:
	// vm_translate(page) is FRAME_ADDRESS(frame) 
	if(number <= 0){
		return 0;
	}
	int64_t frame_number = allocate_frame(number);
	if(frame_number == -1){
		return 0;
	}
	uint64_t page_number = vm_locate(number);
	page_number = frame_number;
	vm_map(page_number, frame_number, number, 0);
	return (char*)PAGE_ADDRESS(page_number);

	// Dummy code: you cannot use malloc/free
	// return malloc(4196 * number);
}

void pfree(void* address, uint64_t number) {
	// TODO:
	// 1) Unmap the page number starting at the provided address (the address is the first byte of a page)
	//    This means: 
	// 2) Do that for the number of times provided in the parameter number

	// page = PAGE_NUMBER(address)
	// frame = vm_translate(page)
	//
	// vm_unmap(page)
	// deallocate_frame(frame)


	//TODO: what is the parameter address?
	uint64_t page_number = PAGE_NUMBER((char*)address);
	//TODO: shift by 12? Should this return frame address or frame number?
	uint64_t frame_address = vm_translate(page_number << 12);

	vm_unmap(page_number, number);

	deallocate_frame(page_number, number);


	// Dummy code: you cannot use malloc/free
	// free(address);
}

void *kmalloc(uint64_t size) {
	// TODO:
	// - Implement a linked list of free chunks using only palloc() [see notes below]
	// - Use the first-fit strategy to allocate a chunk
	int number_of_frames;
	if(((size + 2 * sizeof(uint64_t)) % FRAME_SIZE) == 0){
		number_of_frames = ((size + 2 * sizeof(uint64_t)) / FRAME_SIZE);
	}
	else{
		number_of_frames = ((size + 2 * sizeof(uint64_t)) / FRAME_SIZE) + 1;
	}
	char *current_node = NULL;
	char *last_current_node = NULL;
	char *frame_head = NULL;
	if(size <= 0){
		return 0;
	}
	if(free_list_head == NULL){
		frame_init();
		if((frame_head = palloc(number_of_frames))){
			*((uint64_t *)(frame_head)) = size;
			*((uint64_t *)(frame_head + sizeof(uint64_t))) = MAGIC_NUMBER;
			free_list_head = frame_head + 2 * sizeof(uint64_t) + size;
			*((uint64_t *)(free_list_head)) = (number_of_frames * FRAME_SIZE) - size - 4 * sizeof(uint64_t);
			*((uint64_t *)(free_list_head + sizeof(uint64_t))) = 0;
			return frame_head + 2 * sizeof(uint64_t);
		}
		else{
			return 0;
		}
	}
	
	current_node = free_list_head;
	while(*((uint64_t *)(current_node)) < size){
		if(*((uint64_t *)(current_node + sizeof(uint64_t))) == 0){
			if((frame_head = palloc(number_of_frames))){
				*((uint64_t *)(frame_head)) = size;
				*((uint64_t *)(frame_head + sizeof(uint64_t))) = MAGIC_NUMBER;
				*((uint64_t *)(current_node + sizeof(uint64_t))) = (uint64_t)(frame_head + 2 * sizeof(uint64_t) + size);
				current_node = (char*)(*((uint64_t *)(current_node + sizeof(uint64_t))));
				*((uint64_t *)(current_node)) = (number_of_frames * FRAME_SIZE) - size - 4 * sizeof(uint64_t);
				*((uint64_t *)(current_node + sizeof(uint64_t))) = 0;
				return frame_head + 2 * sizeof(uint64_t);
			}
			else{
				return 0;
			}
		}
		else{
			last_current_node = current_node;
			current_node = (char*)(*((uint64_t *)(current_node + sizeof(uint64_t))));
		}
	}
	if(current_node != free_list_head){
		*((uint64_t *)(last_current_node + sizeof(uint64_t))) = (uint64_t)(current_node + 2 * sizeof(uint64_t) + size);
	}
	else{
		free_list_head = current_node + 2 * sizeof(uint64_t) + size;
	}
	last_current_node = current_node;
	current_node = current_node + 2 * sizeof(uint64_t) + size;
	*((uint64_t *)(current_node)) = *((uint64_t *)(last_current_node)) - size - 2 * sizeof(uint64_t);
	*((uint64_t *)(current_node + sizeof(uint64_t))) = *((uint64_t *)(last_current_node + sizeof(uint64_t)));
	*((uint64_t *)(last_current_node)) = size;
	*((uint64_t *)(last_current_node + sizeof(uint64_t))) = MAGIC_NUMBER;
	return last_current_node + 2 * sizeof(uint64_t);

	// Dummy code: you cannot use malloc/free/realloc
	// return malloc(size);
}

void *krealloc(void *address, uint64_t size) {
	// TODO:
	// - If the address is becoming smaller, return the last frames that have become unused with vm_unmap() and frame_deallocate()
	// - If the address is becoming bigger, and is possible to allocate new contiguous pages to extend the chunk size,
	//   allocate new frames and self-map their numbers to page numbers (see notes on the palloc() function for a discussion of self-mapping)
	// - If the address is becomming bigger, but it is not possible to allocate new contigous pages to extend the chunk size,
	//   use kmallloc() to allocate a new chunk, then use memcpy() to copy the original bytes to the new chunk, and return the new chunk's address.
	//   Before returning, free the old chunk (by merging their free space to another header)
	char *current_node = NULL;
	char *last_current_node = NULL;
	int all_free_nodes_after = 0;
	int free_node_before = 0;
	if((char*)address < free_list_head){
		all_free_nodes_after = 1;
	}
	if(*((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) >= size){

		if(all_free_nodes_after){
			if(((char*)address + *((uint64_t *)((char*)address - 2 * sizeof(uint64_t)))) == free_list_head){
				*((uint64_t *)((char*)address + size)) = *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) - size +\
					*((uint64_t *)free_list_head);
				*((uint64_t *)((char*)address + size + sizeof(uint64_t))) = *((uint64_t *)(free_list_head + sizeof(uint64_t)));
			}
			else{
				*((uint64_t *)((char*)address + size)) = *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) - size -\
					2 * sizeof(uint64_t);
				*((uint64_t *)((char*)address + size + sizeof(uint64_t))) = (uint64_t)free_list_head;
			}
			*((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) = size;
			free_list_head = (char*)address + size;
			return (char*)address;
		}
		current_node = free_list_head;
		while(current_node < (char*)address){
			if(*((uint64_t *)(current_node + sizeof(uint64_t))) == 0){
				//no nodes after it
				*((uint64_t *)((char*)address + size)) = *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) - size -\
					2 * sizeof(uint64_t);
				*((uint64_t *)((char*)address + size + sizeof(uint64_t))) = 0;
				*((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) = size;
				*((uint64_t *)(current_node + sizeof(uint64_t))) = (uint64_t)((char*)address + size);
				return (char*)address;
			}
			else{
				last_current_node = current_node;
				current_node = (char*)(*((uint64_t *)(current_node + sizeof(uint64_t))));
			}
		}
		if(((char*)address + *((uint64_t *)((char*)address - 2 * sizeof(uint64_t)))) == current_node){
			//free node right after it
			*((uint64_t *)((char*)address + size)) = *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) - size +\
				*((uint64_t *)current_node);
			*((uint64_t *)((char*)address + size + sizeof(uint64_t))) = *((uint64_t *)(current_node + sizeof(uint64_t)));
		}
		else{
			*((uint64_t *)((char*)address + size)) = *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) - size -\
				2 * sizeof(uint64_t);
			*((uint64_t *)((char*)address + size + sizeof(uint64_t))) = (uint64_t)current_node;
		}
		*((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) = size;
		*((uint64_t *)(last_current_node + sizeof(uint64_t))) = (uint64_t)((char*)address + size);
		return (char*)address;
		
	}
	else{
		current_node = free_list_head;
		
		if(all_free_nodes_after){
			printf("yes\n");
			if((uint64_t)current_node ==\
				(uint64_t)((char*)address + *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))))){
				printf("how\n");
				if(*((uint64_t *)(current_node)) > size - (*((uint64_t *)((char*)address - 2 * sizeof(uint64_t))))){
					//fit, extend used space
					printf("yeap\n");
					uint64_t difference = *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) - size;
					*((uint64_t *)((char*)address + size)) = *((uint64_t *)(current_node)) - difference;
					*((uint64_t *)((char*)address + size + sizeof(uint64_t))) = *((uint64_t *)(current_node + sizeof(uint64_t)));
					free_list_head = (char*)address + size;
					*((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) = size;
					return (char*)address;
				}
			}
		}
		while(*((uint64_t *)(current_node + sizeof(uint64_t))) !=\
			(uint64_t)((char*)address + *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))))){

			if(*((uint64_t *)(current_node + sizeof(uint64_t))) == 0){
				//no free node right after, need to kmalloc new chunk
				char *new_address;
				if((new_address = kmalloc(size))){
					memcpy(new_address, (char*)address, *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))));
					kfree((char*)address);
					return new_address;
				}
				else{
					return 0;
				}
			}
			else{
				last_current_node = current_node;
				current_node = (char*)(*((uint64_t *)(current_node + sizeof(uint64_t))));
			}
		}
		last_current_node = current_node;
		current_node = (char*)(*((uint64_t *)(current_node + sizeof(uint64_t))));
		if(*((uint64_t *)(current_node)) < (size - *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))))){
			//does not fit, need to kmalloc new chunk
			char *new_address;
			if((new_address = kmalloc(size))){
				memcpy(new_address, (char*)address, *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))));
				kfree((char*)address);
				return new_address;
			}
			else{
				return 0;
			}
		}
		else{
			//fit, extend used space
			uint64_t difference = *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) - size;
			*((uint64_t *)((char*)address + size)) = *((uint64_t *)(current_node)) - difference;
			*((uint64_t *)((char*)address + size + sizeof(uint64_t))) = *((uint64_t *)(current_node + sizeof(uint64_t)));
			*((uint64_t *)(last_current_node + sizeof(uint64_t))) = (uint64_t)((char*)address + size);
			*((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) = size;
			return (char*)address;
		}
	}

	// Dummy code: you cannot use malloc/free/realloc
	return realloc(address, size);
}

void kfree(void *address) {
	// TODO:
	// - Make the space used by the address free
	// - Return any frames that have become unused with vm_unmap() and frame_deallocate()
	char *current_node = NULL;
	char *last_current_node = NULL;
	int free_node_before = 0;
	int all_free_nodes_after = 0;
	if(*((uint64_t *)((char*)address - sizeof(uint64_t))) == MAGIC_NUMBER){
		uint64_t size = *((uint64_t *)((char*)address - 2 * sizeof(uint64_t)));
		current_node = free_list_head;
		if((char*)address < free_list_head){
			//All free nodes are after it, make it the free_list_head
			if(((char*)address + size) == free_list_head){
				*((uint64_t *)((char*)address - sizeof(uint64_t))) = *((uint64_t *)(free_list_head + sizeof(uint64_t)));
				*((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) = size + *((uint64_t *)(free_list_head)) + 2 * sizeof(uint64_t);
				free_list_head = (char*)address - 2 * sizeof(uint64_t);
				return;
			}
			else{
				*((uint64_t *)((char*)address - sizeof(uint64_t))) = (uint64_t)free_list_head;
				free_list_head = (char*)address - 2 * sizeof(uint64_t);
				return;
			}
		}
		while(*((uint64_t *)(current_node + sizeof(uint64_t))) != (uint64_t)((char*)address + size)){
			if((current_node + *((uint64_t *)(current_node)) + 2 * sizeof(uint64_t)) == (char*)address){
				free_node_before = 1;
			}
			if(*((uint64_t *)(current_node + sizeof(uint64_t))) == 0){
				
				current_node = free_list_head;
				last_current_node = NULL;
				while(current_node < (char*)address){
					if(*((uint64_t *)(current_node + sizeof(uint64_t))) == 0){						
						break;
					}
					else{
						last_current_node = current_node;
						current_node = (char*)(*((uint64_t *)(current_node + sizeof(uint64_t))));
					}
				}

				if(free_node_before){				
					//no free list right after it
					*((uint64_t *)(current_node)) = *((uint64_t *)(current_node)) + *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) +\
						2 * sizeof(uint64_t);
					*((uint64_t *)((char*)address - sizeof(uint64_t))) = 1;
					return;
				}
				else{
					//no free list right before and after it
					if(*((uint64_t *)(current_node + sizeof(uint64_t))) == 0){
						//no nodes after it
						*((uint64_t *)(current_node + sizeof(uint64_t))) = (uint64_t)((char*)address - 2 * sizeof(uint64_t));
						*((uint64_t *)((char*)address - sizeof(uint64_t))) = 0;
						return;
					}
					else{
						//nodes after it
						last_current_node = current_node;
						current_node = (char*)(*((uint64_t *)(current_node + sizeof(uint64_t))));
						*((uint64_t *)(last_current_node + sizeof(uint64_t))) = (uint64_t)((char*)address - 2 * sizeof(uint64_t));
						*((uint64_t *)((char*)address - sizeof(uint64_t))) = *((uint64_t *)(current_node));
						return;
					}
						
					
				}
			}
			else{
				last_current_node = current_node;
				current_node = (char*)(*((uint64_t *)(current_node + sizeof(uint64_t))));
			}
		}
		if((current_node + *((uint64_t *)(current_node)) + 2 * sizeof(uint64_t)) == (char*)address){
			free_node_before = 1;
		}
		
		last_current_node = current_node;
		current_node = (char*)(*((uint64_t *)(current_node + sizeof(uint64_t))));
		if(free_node_before){
			*((uint64_t *)(last_current_node)) = *((uint64_t *)(last_current_node)) + *((uint64_t *)((char*)address -\
				2 * sizeof(uint64_t))) + *((uint64_t *)(current_node)) + 4 * sizeof(uint64_t);
			*((uint64_t *)(last_current_node + sizeof(uint64_t))) = *((uint64_t *)(current_node + sizeof(uint64_t)));
			*((uint64_t *)((char*)address - sizeof(uint64_t))) = 1;
			return;
		}
		else{
			*((uint64_t *)(last_current_node + sizeof(uint64_t))) = (uint64_t)((char*)address - 2 * sizeof(uint64_t));
			*((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) = *((uint64_t *)((char*)address - 2 * sizeof(uint64_t))) +\
				*((uint64_t *)(current_node)) + 2 * sizeof(uint64_t);
			*((uint64_t *)((char*)address - sizeof(uint64_t))) = *((uint64_t *)(current_node + sizeof(uint64_t)));
			return;
		}
		
	}

	// Dummy code: you cannot use malloc/free/realloc
	// return free(address);
}
