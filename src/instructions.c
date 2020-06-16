#include "instructions.h"
#include "state.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>

extern ijvm_t machine;

inline void BIPUSH(){
	//signed-byte!
	int8_t num = machine._text_[++machine.pc];
	push(num);
	dprintf("BIPUSH %d\n", num);
	++machine.pc;
}

inline void DUP(){
	word_t top1 = top();
	push(top1);
	dprintf("DUP %d\n", top1);
	++machine.pc;
}

inline void IADD(){
	word_t top1 = pop();
	word_t top2 = pop();
	push(top1 + top2);
	dprintf("IADD %d+%d=%d\n", top1, top2, top());
	++machine.pc;
}

inline void ISUB(){
	word_t top1 = pop();
	word_t top2 = pop();
	push(top2 - top1);
	dprintf("ISUB %d-%d=%d\n", top2, top1, top());
	++machine.pc;
}

inline void IAND(){
	word_t top1 = pop();
	word_t top2 = pop();
	push(top1 & top2);
	dprintf("IAND %d&%d=%d\n", top1, top2, top());
	++machine.pc;
}

inline void IOR(){
	word_t top1 = pop();
	word_t top2 = pop();
	push(top1 | top2);
	dprintf("IOR %d|%d=%d\n", top1, top2, top());
	++machine.pc;
}

inline void POP(){
	pop();
	++machine.pc;
}

inline void SWAP(){
	word_t top1 = pop();
	word_t top2 = pop();
	push(top1); // <- this guy goes first
	push(top2);
	dprintf("SWAP %d %d\n", top1, top2);
	++machine.pc;
}

inline void OUT(){
	word_t top1 = pop();
	if (machine.out == NULL)
		machine.out = stderr;
	dprintf("OUT %c\n", top1);
	fprintf(machine.out, "%c", top1);
	++machine.pc;
}

inline void HALT(){
	machine.pc = machine._text_size_;
}

inline void GOTO(){
	byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
	memcpy(args, machine._text_+machine.pc+1, SHORT_SIZE);
	
	//signed short!
	short offset = BytestoShort(args);
	dprintf("FROM PC=%d,", machine.pc);
	machine.pc += offset;
	dprintf("GOTO PC=%d\n", machine.pc);
}

inline void IFEQ(){
	word_t top1 = pop();
	dprintf("IFEQ->the top of stack\n");
	if (!top1) GOTO();
	else machine.pc += SHORT_SIZE + 1;
}

inline void IFLT(){
	word_t top1 = pop();
	dprintf("IFLT->the top of stack\n");
	if (top1<0) GOTO();
	else machine.pc += SHORT_SIZE + 1;
}

inline void IF_ICMPEQ(){
	word_t top1 = pop();
	word_t top2 = pop();
	dprintf("IF_ICMPEQ-> the 2 tops of stack\n");
	if(top1==top2) GOTO();
	else machine.pc += SHORT_SIZE + 1;
}

inline void LDC_W(){
	byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
	memcpy(args, machine._text_+machine.pc+1, SHORT_SIZE);

	//unsigned short!
	unsigned short idx = BytestoUnsignedShort(args);
	if (idx >= machine._constant_size_){
		dprintf("CONSTANT index out of bound!\n");
		HALT();
		return;
	}
	push(machine._constant_[idx]);
	dprintf("LDC_W %d\n", top());
	machine.pc += 3;
}

inline void ILOAD(bool WIDE){
	machine.isWIDE = false; //reset isWide flag
	unsigned short offset;
	if (WIDE){
		byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
		memcpy(args, machine._text_+machine.pc+1, SHORT_SIZE);
		offset = BytestoUnsignedShort(args);
	}
	else{
		offset = machine._text_[machine.pc + 1];
	}

	if (machine._stack_->lv == 0) // lv is at the bottom of the stack -> load from main()'s variables frame
		loadMainVar(offset);
	else push(machine._stack_->_array_[machine._stack_->lv + 1 + offset]);
	dprintf("ILOAD idx=%d, value=%d\n", offset, top());

	machine.pc += WIDE ? 3 : 2;
}

inline void ISTORE(bool WIDE){
	machine.isWIDE = false; //reset isWide flag
	unsigned short offset;
	if (WIDE){
		byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
		memcpy(args, machine._text_+machine.pc+1, SHORT_SIZE);
		offset = BytestoUnsignedShort(args);
	}
	else{
		offset = machine._text_[machine.pc + 1];
	}

	word_t top1 = pop();
	if (machine._stack_->lv == 0) //lv is at the bottom of the stack ->	store to main()'s variables frame
		storeMainVar(offset, top1);
	else machine._stack_->_array_[machine._stack_->lv + 1 + offset] = top1;
	dprintf("ISTORE idx=%d, value=%d\n", offset, top1);

	machine.pc += WIDE ? 3 : 2;
}

inline void IINC(bool WIDE){
	machine.isWIDE = false;
	unsigned short offset;
	short inc;
	if (WIDE){
		byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
		memcpy(args, machine._text_+machine.pc+1, SHORT_SIZE);
		offset = BytestoUnsignedShort(args);
		memcpy(args, machine._text_+machine.pc+3, SHORT_SIZE);
		inc = BytestoShort(args);
	}
	else{
		offset = machine._text_[machine.pc + 1];
		inc = (int8_t)machine._text_[machine.pc + 2]; //cast to signed byte
	}

	if (machine._stack_->lv == 0) machine._stack_->_mainvar_[offset] += inc;
	else machine._stack_->_array_[machine._stack_->lv + 1 + offset] += inc;
	dprintf("IINC var_id=%d, value=%d\n", offset, inc);

	machine.pc += WIDE ? 5 : 3;
}