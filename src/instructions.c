#include "instructions.h"
#include "state.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>

extern ijvm_t machine;
char GUIbuffer[50000];

inline void BIPUSH(){
	//signed-byte!
	int8_t num = machine.text[++machine.pc];
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
	dprintf("POP \n");
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

inline void IN(){
	if (machine.inp == NULL)
		machine.inp = stdin;
	word_t readChar = fgetc(machine.inp);
	if (readChar== EOF) push(0);
	else push(readChar); 
	dprintf("IN %c\n", readChar);
	++machine.pc;
}

inline void OUT(){
	word_t top1 = pop();
	if (machine.out == NULL)
		machine.out = stderr;
	dprintf("OUT %c\n", top1);
	fprintf(machine.out, "%c", top1);
	++machine.pc;
	strcat(GUIbuffer, (char*)&top1);
}

inline void NOP(){
	dprintf("NOP\n");
	++machine.pc;
}

inline void HALT(){
	machine.pc = machine.text_size;
}

inline void ERR(){
	if (machine.out == NULL)
		machine.out = stderr;
	fprintf(machine.out, "Error!\n");
	HALT();
}

inline void GOTO(){
	byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
	memcpy(args, machine.text+machine.pc+1, SHORT_SIZE);
	
	//signed short!
	short offset = BytestoShort(args);
	dprintf("FROM PC=%d,", machine.pc);
	machine.pc += offset;
	dprintf("GOTO PC=%d\n", machine.pc);
	free(args);
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
	args[0] = machine.text[machine.pc+1];
	args[1] = machine.text[machine.pc+2];

	//unsigned short!
	unsigned short idx = BytestoUnsignedShort(args);
	if (idx >= machine.constant_size){
		dprintf("CONSTANT index out of bound!\n");
		HALT();
		return;
	}
	push(machine.constant[idx]);
	dprintf("LDC_W %d\n", top());
	machine.pc += 3;
	free(args);
}

inline void ILOAD(bool WIDE){
	machine.isWIDE = false; //reset isWide flag
	unsigned short offset;
	if (WIDE){
		byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
		args[0] = machine.text[machine.pc+1];
		args[1] = machine.text[machine.pc+2];
		offset = BytestoUnsignedShort(args);
		free(args);
	}
	else{
		offset = machine.text[machine.pc + 1];
	}
	word_t val = machine._stack_->frameLocalVar[machine._stack_->frameIndex][offset];
	push(val);
	dprintf("ILOAD idx=%d, value=%d\n", offset, top());
	machine.pc += WIDE ? 3 : 2;
}

inline void ISTORE(bool WIDE){
	machine.isWIDE = false; //reset isWide flag
	unsigned short offset;
	if (WIDE){
		byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
		args[0] = machine.text[machine.pc+1];
		args[1] = machine.text[machine.pc+2];
		offset = BytestoUnsignedShort(args);
		free(args);
	}
	else{
		offset = machine.text[machine.pc + 1];
	}
	word_t top1 = pop();
	machine._stack_->frameLocalVar[machine._stack_->frameIndex][offset] = top1;
	dprintf("ISTORE idx=%d, value=%d\n", offset, top1);
	machine.pc += WIDE ? 3 : 2;
}

inline void IINC(bool WIDE){
	machine.isWIDE = false;
	unsigned short offset;
	short inc;
	if (WIDE){
		byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
		args[0] = machine.text[machine.pc+1];
		args[1] = machine.text[machine.pc+2];
		offset = BytestoUnsignedShort(args);
		args[0] = machine.text[machine.pc+3];
		args[1] = machine.text[machine.pc+4];
		inc = BytestoShort(args);
		free(args);
	}
	else{
		offset = machine.text[machine.pc + 1];
		inc = (int8_t)machine.text[machine.pc + 2]; //cast to signed byte
	}
	machine._stack_->frameLocalVar[machine._stack_->frameIndex][offset] += inc;
	dprintf("IINC var_id=%d, value=%d\n", offset, inc);
	machine.pc += WIDE ? 5 : 3;
}

inline void INVOKEVIRTUAL(){
	word_t caller_pc = machine.pc;
	word_t caller_lv = machine._stack_->lv;
	machine._stack_->frameIndex++;
	byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
	args[0] = machine.text[machine.pc+1];
	args[1] = machine.text[machine.pc+2];
	
	//get new program counter from CONSTANT_POOL
	unsigned short method_idx = BytestoUnsignedShort(args);
	machine.pc = machine.constant[method_idx];

	//get # of args
	args[0] = machine.text[machine.pc];
	args[1] = machine.text[machine.pc+1];
	unsigned short num_args = BytestoUnsignedShort(args);

	//get # of local_vars
	args[0] = machine.text[machine.pc+2];
	args[1] = machine.text[machine.pc+3];
	unsigned short num_local = BytestoUnsignedShort(args); 

	//push function's args to the stack
	word_t* func_args = malloc(sizeof(word_t) * num_args);
	for(int i = 0; i < num_args; ++i) func_args[i] = pop();
	machine._stack_->sp++;
	machine._stack_->lv = machine._stack_->sp;
	for (int i = 0; i < num_args; ++i)
		machine._stack_->frameLocalVar[machine._stack_->frameIndex][i] = func_args[num_args - i - 1];
	
	//push caller_lv and caller_pc on top of the stack, set link_ptr points to caller_pc
	push(caller_pc);
	machine._stack_->Array[machine._stack_->lv] = machine._stack_->sp;
	push(caller_lv);
	machine.pc += 4;

	free(args);
	free(func_args);
	triggerOnNewFrame();
	dprintf("INVOKEVIRTUAL caller_pc=%d, num_args=%d, num_local=%d\n", caller_pc, num_args, num_local);
}

inline void IRETURN(){
	word_t result = pop();
	machine.pc = machine._stack_->Array[machine._stack_->Array[machine._stack_->lv]]; //pc = caller_pc
	machine._stack_->sp = machine._stack_->lv - 1; // sp = lv - 1
	machine._stack_->lv = machine._stack_->Array[machine._stack_->Array[machine._stack_->lv] + 1]; // lv = caller_lv
	push(result); // push result on top of the stack
	dprintf("IRETURN %d, machine.pc=%d\n", result, machine.pc);
	machine.pc += 3;
	sweep(machine._heap_->currentFrame);
	machine._heap_->currentFrame--;
	machine._stack_->frameIndex--;
}

inline void NEWARRAY(){
	word_t newArraySize = pop();
	word_t arrayref = getNewArrayID();
	
	allocateNewArray(arrayref, newArraySize);
	push(arrayref);
	trackNewArray(arrayref);
	machine.pc++;
}

inline void IASTORE(){
	word_t arrayref = pop();
	word_t index = pop();
	word_t val = pop();

	arrayStore(arrayref, index, val);
	machine.pc++;
}

inline void IALOAD(){
	word_t arrayref = pop();
	word_t index = pop();

	push(arrayLoad(arrayref, index));
	machine.pc++;
}

inline void GC(){
	for (int i = machine._heap_->currentFrame + 1; i < machine._heap_->trackerCap; ++i)
		sweep(i);
	machine.pc++;
}

