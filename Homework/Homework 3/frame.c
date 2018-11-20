#include "frame.h"
#include <stdio.h>

uint64_t frames_allocated;

uint64_t frames_available;

uint64_t bitmap[16];

// No need to change or initialize this. This is "empty" space that simulate your physical memory.
_Alignas(4096) char memory[MEMORY_SIZE];

// You should define a bitmap to keep track of the allocated frames here.
// Look at the handout for details on size

void frame_init() {
	// Initialize global variables
	// Initialize the bitmap
	frames_allocated = 0;
	frames_available = 1024;
	for(int i = 0; i < 16; i++){
		for(int j = 0; j < 64; j++){
			bitmap[i] = bitmap[i] & ~(1ULL << j);
		}
	}
}

int64_t allocate_frame(int number_frames) {
	// Consult the bitmap and return the first available frame number, marking it as allocated
	// Increase the frames_allocated, decrease frames_available
	if(number_frames <= 0){
		return -1;
	}
	int number_frames_left = number_frames;
	int found_first_empty = 0;
	int64_t first_entry[2];


	for(int i = 0; i < 16; i++){
		for(int j = 0; j < 64; j++){
			if(bitmap[i] != (bitmap[i] | (1ULL << j))){
				if(found_first_empty == 0){
					found_first_empty = 1;
					number_frames_left--;
					first_entry[0] = i;
					first_entry[1] = j;
					if(number_frames_left == 0){
						bitmap[i] = bitmap[i] | (1ULL << j);
						frames_allocated++;
						frames_available--;
						return i * 64 + j;
					}
				}
				else{
					number_frames_left--;
					if(number_frames_left == 0){
						int a = first_entry[0];
						int b = first_entry[1];
						for(int k = 0; k < number_frames; k++){
							bitmap[a] = bitmap[a] | (1ULL << b);
							frames_allocated++;
							frames_available--;
							b++;
							if(b >= 64){
								a++;
								b = 0;
							}
						}
						return first_entry[0] * 64 + first_entry[1];
					}
				}
			}
			else{
				if(found_first_empty == 1){
					found_first_empty = 0;
					number_frames_left = number_frames;
					first_entry[0] = -1;
					first_entry[1] = -1;
				}
			}
		}
	}

	return -1; // Return according to what's documented in the header file for this module
}

int64_t deallocate_frame(uint64_t frame_number, int number_frames) {
	// Mark the frame as deallocated in the bitmap
	// Decrease the frames_allocated, increase frames_available
	if(number_frames <= 0){
		return  -1;
	}
	int a = frame_number / 64;
	int b = frame_number % 64;
	for(int i = 0; i < number_frames; i++){
		if(bitmap[a] != (bitmap[a] & ~(1ULL << b))){
			b++;
			if(b >= 64){
				a++;
				b = 0;
			}
		}
		else{
			return -1;
		}
	}
	a = frame_number / 64;
	b = frame_number % 64;
	for(int i = 0; i < number_frames; i++){
		bitmap[a] = bitmap[a] & ~(1ULL << b);
		frames_allocated--;
		frames_available++;
		b++;
		if(b >= 64){
			a++;
			b = 0;
		}
	}
	return (int64_t)frame_number;

}
