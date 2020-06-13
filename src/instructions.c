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
	fprintf(machine.out, "%c\n", top1);
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