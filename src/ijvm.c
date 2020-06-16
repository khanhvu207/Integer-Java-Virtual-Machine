#include "state.h"
#include "util.h"
#include "instructions.h"

extern ijvm_t machine;

bool step() {
	if(machine.pc >= machine._text_size_) return false;
	switch(machine._text_[machine.pc]){
		case OP_WIDE:
			++machine.pc;
			machine.isWIDE = true;
			step();
			break;
		case OP_BIPUSH:
			BIPUSH();
			break;
		case OP_DUP:
			DUP();
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
		case OP_IN:
			IN();
			break;
		case OP_OUT:
			OUT();
			break;
		case OP_NOP:
			NOP();
			break;
		case OP_ERR:
			ERR();
			break;
		case OP_HALT:
			HALT();
			break;
		case OP_GOTO:
			GOTO();
			break;
		case OP_IFEQ:
			IFEQ();
			break;
		case OP_IFLT:
			IFLT();
			break;
		case OP_ICMPEQ:
			IF_ICMPEQ();
			break;
		case OP_LDC_W:
			LDC_W();
			break;
		case OP_ILOAD:
			ILOAD(machine.isWIDE);
			break;
		case OP_ISTORE:
			ISTORE(machine.isWIDE);
			break;
		case OP_IINC:
			IINC(machine.isWIDE);
			break;
		case OP_INVOKEVIRTUAL:
			INVOKEVIRTUAL();
			break;
		case OP_IRETURN:
			IRETURN();
			break;
		default:
			dprintf("Couldn't find the corresponding OP_CODE:%02x\n", machine._text_[machine.pc]);
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

word_t get_local_variable(int i){
	if (machine._stack_->lv == 0) return machine._stack_->_mainvar_[i];
	else return machine._stack_->_array_[machine._stack_->lv + 1 + i];
}

byte_t get_instruction(){
	return machine._text_[machine.pc];
}

void set_input(FILE *fp)
{
  	machine.inp = fp;
}

void set_output(FILE *fp)
{
	machine.out = fp;
}
