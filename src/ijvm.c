#include "state.h"
#include "util.h"

extern ijvm_t machine;

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
