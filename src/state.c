#include "state.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

void printMachineBinary(ijvm_t* machine) {
	#define COLUMNS 8

	dprintf("\n");
	dprintf("%d bytes in total.\n", machine->binary_size);
	for(int i = 0; i < machine->binary_size; ++i) {
		dprintf("%02x ", machine->binary[i] & 0xff);
		if (i > 0 && (i+1) % COLUMNS == 0)
			dprintf("\n");
	}
	
	dprintf("\n");
	#undef COLUMNS
}

bool checkFileHeader(ijvm_t* machine) {
	byte_t* header = malloc(HEADER_SIZE * sizeof(byte_t));

	strncpy((char*)header, (const char*)machine->binary, HEADER_SIZE);

	if (BytestoInt(header) != MAGIC_NUMBER) return false;
	return true;
}

void initializeConstant(ijvm_t* machine) {
	byte_t* Array = malloc(BLOCK_SIZE * sizeof(byte_t));
	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->binary[BLOCK_SIZE + i];
	machine->constant_origin = BytestoInt(Array);

	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->binary[BLOCK_SIZE * 2 + i];
	machine->constant_size = BytestoInt(Array) / BLOCK_SIZE;
	dprintf("# of constants: %d\n", machine->constant_size);

	machine->constant = malloc(machine->constant_size * sizeof(word_t));

	for(int i = 0, chunk = 0; i < machine->constant_size;  ++i, chunk += BLOCK_SIZE){
		machine->constant[i] = BytestoInt(machine->binary + BLOCK_SIZE * 3 + chunk);
		dprintf("%d ", machine->constant[i]);
	}	
	dprintf("\n");
}

void initializeText(ijvm_t* machine) {
	int startByte = BLOCK_SIZE * 3 + machine->constant_size * BLOCK_SIZE;
	byte_t* Array = malloc(BLOCK_SIZE * sizeof(byte_t));
	
	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->binary[startByte + i];
	machine->text_origin = BytestoInt(Array);

	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->binary[startByte + BLOCK_SIZE + i];
	machine->text_size = BytestoInt(Array);

	machine->text = malloc(machine->text_size * sizeof(byte_t));

	for (int i = 0; i < machine->text_size; ++i)
		machine->text[i] = machine->binary[startByte + BLOCK_SIZE * 2 + i];

	// #ifdef DEBUG
	// 	dprintf("\n---TEXT extracted from the binary---\n");
	// 	for (int i = 0; i < machine->_text_size_; ++i)
	// 		dprintf("%02x ", machine->_text_[i]);
	// 	dprintf("\n");
	// #endif
}

void initializeStack(ijvm_t* machine){
	machine->_stack_ = malloc(sizeof(Stack));
	machine->_stack_->capacity = machine->text_size;
	machine->_stack_->Array = malloc(sizeof(word_t) * machine->_stack_->capacity);
	machine->_stack_->mainLocalVar = malloc(sizeof(word_t) * LOCAL_MAX);
	machine->_stack_->sp = -1;
	machine->_stack_->lv = 0;
	push(OBJREF);
}