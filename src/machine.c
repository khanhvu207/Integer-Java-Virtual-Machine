#include <ijvm.h>
#include "util.h"
#include "state.h"
#include <stdlib.h>
#include <string.h>

ijvm_t machine = {.pc = 0, .isWIDE = false};

int init_ijvm(char *binary_file)
{
	FILE* fp;
	if (!(fp = fopen(binary_file, "rb"))) {
		dprintf("Fail to open target binary file!\n");
		return -1;
	}

	machine.pc = 0;
	machine.isWIDE = false;
	machine._binary_ = malloc(MAX_MEMORY * sizeof(byte_t));
	machine._binary_size_ = 0;
	while (machine._binary_size_ < MAX_MEMORY) {
		word_t inputByte = fgetc(fp);
		if (inputByte == EOF) break;
		machine._binary_[machine._binary_size_++] = inputByte;
	}
	fclose(fp);

#ifdef DEBUG
	dprintf("Done reading the binary file!\n");
	printMachineBinary(&machine);
#endif

	if (!checkFileHeader(&machine)) {
		dprintf("Invalid file header, expect IJVM files!\n");
		return -1;
	}
	initializeConstant(&machine);
	initializeText(&machine);
	initializeStack(&machine);
	return 0;
}

void destroy_ijvm()
{
	if (machine._binary_!=NULL) free(machine._binary_);
	if (machine._file_header_!=NULL) free(machine._file_header_);
	if (machine._constant_!=NULL) free(machine._constant_);
	if (machine._text_!=NULL) free(machine._text_);
	if (machine._stack_->_array_!=NULL) free(machine._stack_->_array_);
	if (machine._stack_->_mainvar_!=NULL) free(machine._stack_->_mainvar_);
	if (machine._stack_!=NULL) free(machine._stack_);
	
	machine.inp = NULL;
	machine.out = NULL;
	memset(&machine, 0, sizeof(ijvm_t));
}

void run()
{
	while (machine.pc < machine._text_size_) {
		if (!step()) break;
	}
}
