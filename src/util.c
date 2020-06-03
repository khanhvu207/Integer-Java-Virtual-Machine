#include "util.h"

word_t HexBytestoInt(byte_t* bytes) {
	return  ((bytes[0]&0xFF)<<24) | ((bytes[1]&0xFF)<<16) | ((bytes[2]&0xFF)<<8) | (bytes[3]&0xFF);
}

void printMachineBinary(ijvm_t* machine) {
	#define COLUMNS 8

	dprintf("%d bytes in total.\n", machine->_binary_size_);
	for(int i = 0; i < machine->_binary_size_; ++i) {
		dprintf("%02x ", machine->_binary_[i] & 0xff);
		if (i % COLUMNS == 0)
			dprintf("\n");
	}
	
	dprintf("\n");
	#undef COLUMNS
}