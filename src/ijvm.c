#include "state.h"
#include "util.h"
#include "instructions.h"

extern ijvm_t machine;

bool step() {
	switch(machine._text_[machine.pc]){
		case OP_BIPUSH:
			BIPUSH();
			break;
		case OP_IADD:
			IADD();
			break;
		case OP_ISUB:
			ISUB();
			break;
		case OP_IAND:
			IAND();
			break;
		case OP_IOR:
			IOR();
			break;
		case OP_POP:
			POP();
			break;
		case OP_SWAP:
			SWAP();
			break;
		case OP_OUT:
			OUT();
			break;
		case OP_HALT:
			HALT();
			break;
	}
	return true;
}

word_t tos(){
	return top();
}

int stack_size(){
	return stackSize();
}

word_t* get_stack(){
	return machine._stack_->_array_;
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
