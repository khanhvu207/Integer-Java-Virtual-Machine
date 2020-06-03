#include <stdlib.h>
#include <ijvm.h>
#include "util.h"
#include "state.h"

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

	return 0;
}

void destroy_ijvm()
{
  // Reset IJVM state
}

void run()
{
  // Step while you can
}

void set_input(FILE *fp)
{
  // TODO: implement me
}

void set_output(FILE *fp)
{
  // TODO: implement me
}
