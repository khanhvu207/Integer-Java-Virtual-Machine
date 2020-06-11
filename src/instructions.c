#include "instructions.h"
#include "state.h"
#include "util.h"

extern ijvm_t machine;

inline void BIPUSH(){
	//signed-byte!
	int8_t num = machine._text_[++machine.pc];
	push(num);
	dprintf("BIPUSH %d\n", num);
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
	dprintf("ISUB %d-%d=%d\n", top1, top2, top());
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