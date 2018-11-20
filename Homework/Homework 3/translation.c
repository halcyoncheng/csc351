#include<stdio.h>

#include "translation.h"
#include "frame.h"

#define TABLE_SIZE 512

void *table_root = NULL;
entry *root_table;

int check_table(uint64_t index, entry* table){
	if(table[index].flags == 0b000000000){
		int64_t frame_number = allocate_frame(1);
		if(frame_number == -1){
			return 0;
		}
		int64_t frame_address = (int64_t)FRAME_ADDRESS(frame_number);
		entry *child_table = (entry *) frame_address;
		table[index].address = frame_address;//TODO:is this okay, since number of bit does not match
		table[index].flags = 0b000000001;
		for(int i = 0; i < 512; i++){
			child_table[i].flags = 0b000000000;
		}
		return 1;
	}
	else if(table[index].flags == 0b000000001){
		return 1;
	}
	else{
		return 0;
	}
}

//TODO: what should we do with the parameter flags
int vm_map(uint64_t page, uint64_t frame, int number, uint16_t flags) {
	entry *next_table;
	if(table_root == NULL || number <= 0){
		// frame_init();
		int64_t frame_number = allocate_frame(1);
		if(frame_number == -1){
			return 0;
		}
		int64_t frame_address = (int64_t)FRAME_ADDRESS(frame_number);
		root_table = (entry *) frame_address;
		table_root = root_table;
		for(int i = 0; i < 512; i++){
			root_table[i].flags = 0b000000000;
		}
	}
	if(check_table((page >> 27), root_table)){
		next_table = (entry *) (uint64_t)root_table[page >> 27].address;
		if(check_table(((page << 37) >> 55), next_table)){
			next_table = (entry *) (uint64_t)next_table[((page << 37) >> 55)].address;
			if(check_table(((page << 46) >> 55), next_table)){
				next_table = (entry *) (uint64_t)next_table[((page << 46) >> 55)].address;
				int64_t frame_address = (int64_t)FRAME_ADDRESS(frame);
				int count = 0;
				for(int i = 0; i < number; i++){
					if(next_table[((page << 55) >> 55) + i].flags == 0b000000000){
						count++;
					}
				}
				if(count == number){
					next_table[((page << 55) >> 55)].address = frame_address;
					next_table[((page << 55) >> 55)].flags = 0b000000001;

					//TODO: is this okay? How we take care of the parameter number
					for(int i = 1; i < number; i++){
						page++;
						frame++;
						frame_address = (int64_t)FRAME_ADDRESS(frame);
						next_table[((page << 55) >> 55)].address = frame_address;
						next_table[((page << 55) >> 55)].flags = 0b000000001;
					}
				}
				else{
					return 0;
				}
				

			}
			else{
				return 0;
			}
		}
		else{
			return 0;
		}
	}
	else{
		return 0;
	}



	return 1;
}

int vm_unmap(uint64_t page, int number) {
	entry *next_table;
	if(table_root == NULL || number <= 0){
		return 0;
	}
	if(root_table[(page >> 27)].flags == 0b000000000){
		return 0;
	}
	else{
		next_table = (entry *) (uint64_t)root_table[(page >> 27)].address;
		if(next_table[((page << 37) >> 55)].flags == 0b000000000){
			return 0;
		}
		else{
			next_table = (entry *) (uint64_t)next_table[((page << 37) >> 55)].address;
			if(next_table[((page << 46) >> 55)].flags == 0b000000000){
				return 0;
			}
			else{
				next_table = (entry *) (uint64_t)next_table[((page << 46) >> 55)].address;
				int count = 0;
				for(int i = 0; i < number; i++){
					if(next_table[((page << 55) >> 55) + i].flags == 0b000000001){
						count++;
					}
				}
				
				if(count == number){
					next_table[((page << 55) >> 55)].address = 0ULL;
					next_table[((page << 55) >> 55)].flags = 0b000000000;

					//TODO: is this okay? How we take care of the parameter number
					for(int i = 1; i < number; i++){
						page++;
						next_table[((page << 55) >> 55)].address = 0ULL;
						next_table[((page << 55) >> 55)].flags = 0b000000000;
					}
				}
				else{
					return 0;
				}
			}
		}
	}
	return 1;
}

//TODO: where is page_low
uint64_t vm_locate(int number) {
	int count = 0;
	int flag = 0;
	uint64_t first;
	entry *second_table;
	entry *third_table;
	entry *fourth_table;
	if(table_root == NULL){
		return 0;
	}
	for(uint64_t i = 0; i < TABLE_SIZE * TABLE_SIZE * TABLE_SIZE; i++){
		if(!vm_translate(i << 12)){
			if(flag == 0){
				flag = 1;
				count++;
				first = i;
			}
			else{
				count++;
			}
			if(count == number){
				return first;
			}
		}
		else{
			if(flag == 1){
				flag = 0;
				count = 0;
			}
		}
	}
	return 0ULL;
}

uint64_t vm_translate(uint64_t virtual_address) {
	// Dummy code: there's no translation being done
	//TODO: really?
	entry *next_table;
	uint64_t physical_address;
	if(table_root == NULL){
		return 0;
	}
	else{
		if(root_table[(virtual_address << 16) >> 55].flags == 0b000000000){
			return 0;
		}
		else{
			next_table = (entry *) (uint64_t)root_table[(virtual_address << 16) >> 55].address;
			if(next_table[(virtual_address << 25) >> 55].flags == 0b000000000){
				return 0;
			}
			else{
				next_table = (entry *) (uint64_t)next_table[(virtual_address << 25) >> 55].address;
				if(next_table[(virtual_address << 34) >> 55].flags == 0b000000000){
					return 0;
				}
				else{
					next_table = (entry *) (uint64_t)next_table[(virtual_address << 34) >> 55].address;
					if(next_table[(virtual_address << 43) >> 55].flags == 0b000000000){
						return 0;
					}
					else{
						physical_address = next_table[(virtual_address << 43) >> 55].address + ((virtual_address << 52) >> 52);
					}
				}
			}
		}
	}
	return physical_address;
}
