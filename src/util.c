#include "util.h"

word_t HexBytestoInt(byte_t* bytes) {
	return  ((bytes[0]&0xFF)<<24) | ((bytes[1]&0xFF)<<16) | ((bytes[2]&0xFF)<<8) | (bytes[3]&0xFF);
}

void printMachineBinary(ijvm_t* machine) {
	#define COLUMNS 8

	dprintf("\n");
	dprintf("%d bytes in total.\n", machine->_binary_size_);
	for(int i = 0; i < machine->_binary_size_; ++i) {
		dprintf("%02x ", machine->_binary_[i] & 0xff);
		if (i > 0 && i % COLUMNS == 0)
			dprintf("\n");
	}
	
	dprintf("\n");
	#undef COLUMNS
}

bool checkFileHeader(ijvm_t* machine) {
	byte_t* header = malloc(HEADER_SIZE * sizeof(byte_t));

	strncpy((char*)header, (const char*)machine->_binary_, HEADER_SIZE);

	if (HexBytestoInt(header) != MAGIC_NUMBER) return false;
	return true;
}

void initializeConstant(ijvm_t* machine) {
	byte_t* Array = malloc(BLOCK_SIZE * sizeof(byte_t));
	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->_binary_[BLOCK_SIZE + i];
	machine->_constant_origin_ = HexBytestoInt(Array);

	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->_binary_[BLOCK_SIZE * 2 + i];
	machine->_constant_size_ = HexBytestoInt(Array);

	machine->_constant_ = malloc(machine->_constant_size_ * sizeof(byte_t));

	for (int i = 0; i < machine->_constant_size_; ++i)
		machine->_constant_[i] = machine->_binary_[BLOCK_SIZE * 3 + i];
}

void initializeText(ijvm_t* machine) {
	int startByte = BLOCK_SIZE * 3 + machine->_constant_size_;
	byte_t* Array = malloc(BLOCK_SIZE * sizeof(byte_t));
	
	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->_binary_[startByte + i];
	machine->_text_origin_ = HexBytestoInt(Array);

	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine->_binary_[startByte + BLOCK_SIZE + i];
	machine->_text_size_ = HexBytestoInt(Array);

	machine->_text_ = malloc(machine->_text_size_ * sizeof(byte_t));

	for (int i = 0; i < machine->_text_size_; ++i)
		machine->_text_[i] = machine->_binary_[startByte + BLOCK_SIZE * 2 + i];

	#ifdef DEBUG
		dprintf("\n---TEXT extracted from the binary---\n");
		for (int i = 0; i < machine->_text_size_; ++i)
			dprintf("%02x ", machine->_text_[i]);
		dprintf("\n");
	#endif
}