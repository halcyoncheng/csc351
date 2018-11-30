#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "files.h"

#include "storage.h"
#include "bitmap.h"

int inodes_per_block = -1;
uint64_t number_inode_blocks = -1;
uint64_t number_bitmap_blocks = -1;

#define DIV_UP(A, B) ((A) % (B) == 0 ? (A) / (B) : (A) / (B) + 1)

// Private prototypes
int ifile_grow(inode_t *inode, uint64_t new_size);

int pointers_read(pointer_block_t *pointers, char *buffer, uint64_t how_many, uint64_t from);
int pointers_write(pointer_block_t *pointers, char *buffer, uint64_t how_many, uint64_t to);

void format() {
	// Empty block (all zeroes)
	char zeroed_block[BLOCK_SIZE];

	memset(zeroed_block, 0, BLOCK_SIZE);

	// Write enough blocks to represent MAX_FILES inodes
	// Zeroed blocks imply all inode fields are zero, which is appropriate

	int inodes_per_block = BLOCK_SIZE / sizeof(inode_t);

	number_inode_blocks = DIV_UP(MAX_FILES, inodes_per_block);

	for(int i = 0; i < number_inode_blocks; i++) {
		storage_write_block(i, zeroed_block);
	}

	// Write enough blocks to represent a bitmap for NUM_BLOCKS blocks
	// Zeroed blocks imply all bitmap entries are 0, which is appropriate
	//TODO: why do need so much bit map blocks
	number_bitmap_blocks = DIV_UP(DIV_UP(NUM_BLOCKS, 8), BLOCK_SIZE);

	for(int i = number_inode_blocks; i < number_inode_blocks + number_bitmap_blocks; i++) {
		storage_write_block(i, zeroed_block);
	}

	// Initialize bitmap

	// The bitmap functions will take care of allocating only blocks from disk that are beyond
	// those the ones that we created above to store the filesystem metadata.

	bitmap_init(number_inode_blocks, number_bitmap_blocks, NUM_BLOCKS);
}

// TODO
int ifile_create(uint64_t inode_number) {
	// If the inode_number is bigger than the maximum inode number,
	// return error
	if(inode_number > MAX_FILES){
		return -1;
	}

	// Calculate the block storing the inode_number
	// Calculate the offset within the block containing the inode_number
	int block_index = inode_number / inodes_per_block;
	int offset = inode_number % inodes_per_block;
	
	
	// Reads the block storing the inode_number
	// inode_t buffer[inodes_per_block];
	char inode_block[BLOCK_SIZE];
	if(storage_read_block(block_index, inode_block) == -1){
		return -1;
	}
	
	// Find the inode_number entry within the block
	inode_t* inode = ((inode_t*)(inode_block + offset * sizeof(inode_t)));

	// Update the inode:
	//  a) Set the file size to zero
	//  b) Set the used flags to true
	//  c) Allocate a new block using bitmap_allocate_block(), and set the inode's head_pointer_block to point to it
	//     This block will be the (unique) indirect block containing all the file's disk blocks.
	inode->size = 0ULL;
	inode->flags_used = 0b1;
	//TODO: is this the right way to point to the block
	int new_block_num = bitmap_allocate_block();
	if(new_block_num == -1){
		return -1;
	}
	// buffer[offset].head_pointer_block = (uint64_t)(storage + new_block_num * BLOCK_SIZE);
	inode->head_pointer_block = new_block_num;

	// Writes the block storing the inode_number back to disk, since we made changes on it
	if(storage_write_block(block_index, inode_block) == -1){
		return -1;
	}

	// Reads the block containing the inode's head_pointer_block
	pointer_block_t indirect_buffer;
	if(storage_read_block(inode->head_pointer_block, &indirect_buffer) == -1){
		return -1;
	}

	// Zeroes all entries, except the first, which should point to a newly allocated DATA block
	// (again found by bitmap_allocate_block()) that stores the first block of the file's DATA.
	for(int i = 1; i < 512; i++){
		indirect_buffer.entry[i] = 0ULL;
	}
	new_block_num = bitmap_allocate_block();
	if(new_block_num == -1){
		return -1;
	}
	indirect_buffer.entry[0] = new_block_num;

	// Writes the block containing the inode's head_pointer_block
	if(storage_write_block(inode->head_pointer_block, &indirect_buffer) == -1){
		return -1;
	}
	return 0;
}

int ifile_grow(inode_t *inode, uint64_t new_size) {
	int current_block_number = inode->size / BLOCK_SIZE;
	int last_block_number = new_size / BLOCK_SIZE;

	char block[BLOCK_SIZE];

	storage_read_block(inode->head_pointer_block, block);

	pointer_block_t *pointers = (pointer_block_t *) block;

	for(uint64_t i = current_block_number + 1; i <= last_block_number; i++) {
		pointers->entry[i] = bitmap_allocate_block();

		if(pointers->entry[i] == -1) {
			// Deallocate previously allocated blocks

			for(uint64_t j = i; j >= current_block_number + 1; j--) {
				bitmap_deallocate_block(j);
				pointers->entry[j] = -1;
			}

			break;
		}
	}

	storage_write_block(inode->head_pointer_block, block);

	inode->size = new_size;

	return 0;
}

// TODO
int ifile_read(uint64_t inode_number, char *buffer, uint64_t how_many, uint64_t from) {
	// Calculate the block storing the inode_number
	// Calculate the offset within the block containing the inode_number
	int block_index = inode_number / inodes_per_block;
	int offset = inode_number % inodes_per_block;


	// Reads the block storing the inode_number
	char inode_block[BLOCK_SIZE];
	if(storage_read_block(block_index, inode_block) == -1){
		return -1;
	}

	// Find the inode_number entry within the block
	inode_t* inode = ((inode_t*)(inode_block + offset * sizeof(inode_t)));

	// If (from + how_many) is bigger than the file size, you are trying to read past the end of the file:
	// in this case, return -1
	if(from + how_many > inode->size){
		return -1;
	}

	// Otherwise, read the inode's head_pointer_block, and call
	// the pointers_read() function, which will read all the necessary file's blocks
	char block[BLOCK_SIZE];
	if(storage_read_block(inode->head_pointer_block, block) == -1){
		return -1;
	}
	pointer_block_t *pointers = (pointer_block_t *) block;
	pointers_read(pointers, buffer, how_many, from);
	return 0;
}

int pointers_read(pointer_block_t *pointers, char *buffer, uint64_t how_many, uint64_t from) {
	// Block number and offset within a block for the first block
	int first_block_number = from / BLOCK_SIZE;
	int first_block_offset = from % BLOCK_SIZE;

	// Block number and offset within a block for the last block
	int last_block_number = (from + how_many) / BLOCK_SIZE;
	int last_block_offset = (from + how_many) % BLOCK_SIZE;

	char block[BLOCK_SIZE];

	// If everything can be done in a single block...
	if(first_block_number == last_block_number) {
		// Read the block
		storage_read_block(pointers->entry[first_block_number], block);
		// Copy the appropriate bytes into the buffer
		memcpy(buffer, block + first_block_offset, how_many);

		// All done!
		return 0;
	}

	// Otherwise, we must read from multiple blocks

	uint64_t position = 0;

	// Read/Copy first block
	storage_read_block(pointers->entry[first_block_number], block);
	memcpy(buffer, block + first_block_offset, BLOCK_SIZE - first_block_offset);

	// Read/Copy intermediate blocks
	position += (BLOCK_SIZE - first_block_offset);

	for(int i = first_block_number + 1; i <= last_block_number - 1; i++) {
		storage_read_block(pointers->entry[i], block);
		memcpy(buffer + position, block, BLOCK_SIZE);

		position += BLOCK_SIZE;
	}

	// Read/Copy last block
	storage_read_block(pointers->entry[last_block_number], block);
	memcpy(buffer + position, block, last_block_offset);

	return 0;
}

// TODO
int ifile_write(uint64_t inode_number, void *buffer, uint64_t how_many, uint64_t to) {
	// Calculate the block storing the inode_number
	// Calculate the offset within the block containing the inode_number
	int block_index = inode_number / inodes_per_block;
	int offset = inode_number % inodes_per_block;

	// Reads the block storing the inode_number
	char inode_block[BLOCK_SIZE];
	if(storage_read_block(block_index, inode_block) == -1){
		return -1;
	}

	// Find the inode_number entry within the block
	inode_t* inode = ((inode_t *)(inode_block + offset * sizeof(inode_t)));

	// If (from + how_many) is bigger than the file size, you are trying to write past the end of the file:
	// in this case, call the ifile_grow() function to resize the file.
	if(to + how_many > inode->size){
		ifile_grow(inode, to + how_many);
		if(storage_write_block(block_index, inode_block) == -1){
			return -1;
		}
	}

	// Otherwise, read the inode's head_pointer_block, and call
	// the pointers_write() function, which will overwrite all necessary file's blocks
	char block[BLOCK_SIZE];
	if(storage_read_block(inode->head_pointer_block, block) == -1){
		return -1;
	}
	pointer_block_t *pointers = (pointer_block_t *) block;
	pointers_write(pointers, buffer, how_many, to);
	return 0;
}

// TODO
int pointers_write(pointer_block_t *pointers, char *buffer, uint64_t how_many, uint64_t to) {
	// Block number and offset within a block for the first block
	int first_block_number = to / BLOCK_SIZE;
	int first_block_offset = to % BLOCK_SIZE;
	// Block number and offset within a block for the last block
	int last_block_number = (to + how_many) / BLOCK_SIZE;
	int last_block_offset = (to + how_many) % BLOCK_SIZE;
	
	char block[BLOCK_SIZE];
	// If everything can be done in a single block...
	if(first_block_number == last_block_number) {
		// Read the block
		storage_read_block(pointers->entry[first_block_number], block);
		// Updates the read block
		memcpy(block + first_block_offset, buffer, how_many);
		// write the block back into disk
		storage_write_block(pointers->entry[first_block_number], block);

		// All done!
		return 0;
	}

	// Otherwise, we must read from multiple blocks
	uint64_t position = 0;

	// Read/Update/Write first block
	storage_read_block(pointers->entry[first_block_number], block);
	memcpy(block + first_block_offset, buffer, BLOCK_SIZE - first_block_offset);
	storage_write_block(pointers->entry[first_block_number], block);

	// Read/Update/Write intermediate blocks
	position += (BLOCK_SIZE - first_block_offset);

	for(int i = first_block_number + 1; i <= last_block_number - 1; i++) {
		storage_read_block(pointers->entry[i], block);
		memcpy(block, buffer + position, BLOCK_SIZE);
		storage_write_block(pointers->entry[i], block);

		position += BLOCK_SIZE;
	}

	// Read/Update/Write last block
	storage_read_block(pointers->entry[last_block_number], block);
	memcpy(block, buffer + position, last_block_offset);
	storage_write_block(pointers->entry[last_block_number], block);

	return 0;
}
