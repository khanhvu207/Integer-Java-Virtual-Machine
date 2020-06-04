#include <ijvm.h>
#include "util.h"

ijvm_t machine = {0, false};

int init_ijvm(char *binary_file)
{
	FILE* fp;
	if (!(fp = fopen(binary_file, "rb"))) {
		dprintf("Fail to open target binary file!\n");
		return -1;
	}

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
	return 0;
}

void destroy_ijvm()
{
	free(machine._binary_);
	free(machine._file_header_);
	free(machine._constant_);
	free(machine._text_);
	// memset(&machine, 0, sizeof(ijvm_t));
}

void run()
{
	while (machine.pc < machine._text_size_) {
		if (!step()) break;
	}
}

bool step() {
	++machine.pc;
	return true;
}

int text_size() {
	return machine._text_size_;
}

byte_t* get_text() {
	return machine._text_;
}

int get_program_counter() {
	return machine.pc;
}

void set_input(FILE *fp)
{
  	machine.inp = fp;
}

void set_output(FILE *fp)
{
	machine.out = fp;
}
