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

	machine.pc = machine.invk_cnt = 0;
	machine.isWIDE = false;
	machine.binary = malloc(MAX_MEMORY * sizeof(byte_t));
	machine.binary_size = 0;
	while (machine.binary_size < MAX_MEMORY) {
		word_t inputByte = fgetc(fp);
		if (inputByte == EOF) break;
		machine.binary[machine.binary_size++] = inputByte;
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
	if (machine.binary!=NULL) free(machine.binary);
	if (machine.file_header!=NULL) free(machine.file_header);
	if (machine.constant!=NULL) free(machine.constant);
	if (machine.text!=NULL) free(machine.text);
	if (machine._stack_->Array!=NULL) free(machine._stack_->Array);
	if (machine._stack_->mainLocalVar!=NULL) free(machine._stack_->mainLocalVar);
	if (machine._stack_!=NULL) free(machine._stack_);
	machine.inp = NULL;
	machine.out = NULL;
}

void run()
{
	while (machine.pc < machine.text_size) {
		if (!step()) break;
	}
}
