#include "state.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

void printMachineBinary(ijvm_t* machine) {
	#define COLUMNS 8

	dprintf("\n");
	dprintf("%d bytes in total.\n", machine->_binary_size_);
	for(int i = 0; i < machine->_binary_size_; ++i) {
		dprintf("%02x ", machine->_binary_[i] & 0xff);
		if (i > 0 && (i+1) % COLUMNS == 0)
			dprintf("\n");
	}
	
	dprintf("\n");
	#undef COLUMNS
}

bool checkFileHeader(ijvm_t* machine) {
	byte_t* header = malloc(HEADER_SIZE * sizeof(byte_t));

	strncpy((char*)header, (const char*)machine->_binary_, HEADER_SIZE);

	if (BytestoInt(header) != MAGIC_NUMBER) return false;
	return true;
}

void initializeConstant(ijvm_t* machine) {
	byte_t* Array = malloc(BLOCK_SIZE * sizeof(byte_t));
	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->_binary_[BLOCK_SIZE + i];
	machine->_constant_origin_ = BytestoInt(Array);

	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->_binary_[BLOCK_SIZE * 2 + i];
	machine->_constant_size_ = BytestoInt(Array) / BLOCK_SIZE;

	machine->_constant_ = malloc(machine->_constant_size_ * sizeof(word_t));

	for(int i = 0, chunk = 0; i < machine->_constant_size_;  ++i, chunk += BLOCK_SIZE){
		machine->_constant_[i] = BytestoInt(machine->_binary_ + BLOCK_SIZE * 3 + chunk);
	}	
}

void initializeText(ijvm_t* machine) {
	int startByte = BLOCK_SIZE * 3 + machine->_constant_size_ * BLOCK_SIZE;
	byte_t* Array = malloc(BLOCK_SIZE * sizeof(byte_t));
	
	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->_binary_[startByte + i];
	machine->_text_origin_ = BytestoInt(Array);

	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->_binary_[startByte + BLOCK_SIZE + i];
	machine->_text_size_ = BytestoInt(Array);

	machine->_text_ = malloc(machine->_text_size_ * sizeof(byte_t));

	for (int i = 0; i < machine->_text_size_; ++i)
		machine->_text_[i] = machine->_binary_[startByte + BLOCK_SIZE * 2 + i];

	// #ifdef DEBUG
	// 	dprintf("\n---TEXT extracted from the binary---\n");
	// 	for (int i = 0; i < machine->_text_size_; ++i)
	// 		dprintf("%02x ", machine->_text_[i]);
	// 	dprintf("\n");
	// #endif
}

void initializeStack(ijvm_t* machine){
	machine->_stack_ = malloc(sizeof(Stack));
	machine->_stack_->_capacity_ = machine->_stack_->_mainvar_size_ = machine->_text_size_;
	machine->_stack_->_array_ = malloc(sizeof(word_t) * machine->_stack_->_capacity_);
	machine->_stack_->_mainvar_ = malloc(sizeof(word_t) * machine->_stack_->_mainvar_size_);
	machine->_stack_->sp = -1;
	machine->_stack_->lv = 0;
}