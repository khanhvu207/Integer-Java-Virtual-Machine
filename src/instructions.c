#include "instructions.h"
#include "state.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>

extern ijvm_t machine;

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
	memcpy(args, machine.text+machine.pc+1, SHORT_SIZE);

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
	args=NULL;
}

inline void ILOAD(bool WIDE){
	machine.isWIDE = false; //reset isWide flag
	unsigned short offset;
	if (WIDE){
		byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
		memcpy(args, machine.text+machine.pc+1, SHORT_SIZE);
		offset = BytestoUnsignedShort(args);
		free(args);
		args=NULL;
	}
	else{
		offset = machine.text[machine.pc + 1];
	}

	if (machine._stack_->lv == 0 && machine._stack_->Array[machine._stack_->lv] == OBJREF) // lv is at the bottom of the stack -> load from main()'s variables frame
		loadMainVar(offset);
	else push(machine._stack_->Array[machine._stack_->lv + 1 + offset]);
	dprintf("ILOAD lv=%d, idx=%d, value=%d\n", machine._stack_->lv, offset, top());

	machine.pc += WIDE ? 3 : 2;
}

inline void ISTORE(bool WIDE){
	machine.isWIDE = false; //reset isWide flag
	unsigned short offset;
	if (WIDE){
		byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
		memcpy(args, machine.text+machine.pc+1, SHORT_SIZE);
		offset = BytestoUnsignedShort(args);
		free(args);
		args=NULL;
	}
	else{
		offset = machine.text[machine.pc + 1];
	}

	word_t top1 = pop();
	if (machine._stack_->lv == 0 && machine._stack_->Array[machine._stack_->lv] == OBJREF) //lv is at the bottom of the stack ->	store to main()'s variables frame
		storeMainVar(offset, top1);
	else machine._stack_->Array[machine._stack_->lv + 1 + offset] = top1;
	dprintf("ISTORE idx=%d, value=%d\n", offset, top1);

	machine.pc += WIDE ? 3 : 2;
}

inline void IINC(bool WIDE){
	machine.isWIDE = false;
	unsigned short offset;
	short inc;
	if (WIDE){
		byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
		memcpy(args, machine.text+machine.pc+1, SHORT_SIZE);
		offset = BytestoUnsignedShort(args);
		memcpy(args, machine.text+machine.pc+3, SHORT_SIZE);
		inc = BytestoShort(args);
		free(args);
		args=NULL;
	}
	else{
		offset = machine.text[machine.pc + 1];
		inc = (int8_t)machine.text[machine.pc + 2]; //cast to signed byte
	}

	if (machine._stack_->lv == 0 && machine._stack_->Array[machine._stack_->lv] == OBJREF) machine._stack_->mainLocalVar[offset] += inc;
	else machine._stack_->Array[machine._stack_->lv + 1 + offset] += inc;
	dprintf("IINC var_id=%d, value=%d\n", offset, inc);

	machine.pc += WIDE ? 5 : 3;
}

inline void INVOKEVIRTUAL(){
	word_t caller_pc = machine.pc;
	word_t caller_lv = machine._stack_->lv;
	byte_t* args = malloc(SHORT_SIZE * sizeof(byte_t));
	memcpy(args, machine.text+machine.pc+1, SHORT_SIZE);
	
	//get new program counter from CONSTANT_POOL
	unsigned short method_idx = BytestoUnsignedShort(args);
	machine.pc = machine.constant[method_idx];

	//get # of args
	memcpy(args, machine.text+machine.pc, SHORT_SIZE);
	unsigned short num_args = BytestoUnsignedShort(args);

	//get # of local_vars
	memcpy(args, machine.text+machine.pc+2, SHORT_SIZE);
	unsigned short num_local = BytestoUnsignedShort(args); 

	//push function's args to the stack
	word_t* func_args = malloc(sizeof(word_t) * num_args);
	for(int i = 0; i < num_args; ++i) func_args[i] = pop();

	machine._stack_->lv = ++machine._stack_->sp;
	for(int i = num_args - 1; i >= 0; --i) push(func_args[i]);
	

	machine._stack_->sp += num_local;
	if (machine._stack_->sp + 1 == machine._stack_->capacity){
		machine._stack_->capacity *= 2;
		machine._stack_->Array = realloc(machine._stack_->Array, machine._stack_->capacity * sizeof(word_t));
		fprintf(stderr, "Stack expanded\n");
	}

	//push caller_lv and caller_pc on top of the stack, set link_ptr points to caller_pc
	push(caller_pc);
	machine._stack_->Array[machine._stack_->lv] = machine._stack_->sp;
	push(caller_lv);
	machine.pc += 4;

	free(args);
	free(func_args);
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
}