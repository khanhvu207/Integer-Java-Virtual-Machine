#include "ijvm.h"
#include "util.h"
#include "state.h"
#include "instructions.h"
#include <stdlib.h>
#include <string.h>

ijvm_t machine = {.pc = 0, .isWIDE = false};
extern char GUIbuffer[50000];

int init_ijvm(char *binary_file)
{
	FILE* fp;
	if (!(fp = fopen(binary_file, "rb"))) {
		dprintf("Fail to open target binary file!\n");
		return -1;
	}

	machine.pc = machine.invk_cnt = 0;
	machine.isWIDE = false;
	machine.isRunning = true;
	machine.binary = malloc(MAX_MEMORY * sizeof(byte_t));
	machine.binary_size = 0;
	memset(GUIbuffer, 0, 50000 * sizeof(char));
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

	if (!checkFileHeader()) {
		dprintf("Invalid file header, expect IJVM files!\n");
		return -1;
	}
	initializeConstant();
	initializeText();
	initializeStack();
	initializeHeap();
	return 0;
}

void destroy_ijvm()
{
	if (!machine.isRunning) return;
	dprintf("Destroying the IJVM...\n");
	machine.isRunning = false;

	// ### FREE TEXT MEM ###
	if (machine.binary!=NULL) free(machine.binary);
	if (machine.file_header!=NULL) free(machine.file_header);
	if (machine.constant!=NULL) free(machine.constant);
	if (machine.text!=NULL) free(machine.text);
	
	// ### FREE STACK MEM ###
	if (machine._stack_->Array!=NULL) free(machine._stack_->Array);
	if (machine._stack_->mainLocalVar!=NULL) free(machine._stack_->mainLocalVar);
	if (machine._stack_!=NULL) free(machine._stack_);

	// ### FREE HEAP MEM ###
	for (int i = 0; i < machine._heap_->heap_size; ++i) 
		if (machine._heap_->used[i]) freeArray(i);
	if (machine._heap_->array_ptr!=NULL) free(machine._heap_->array_ptr);
	if (machine._heap_->used!=NULL) free(machine._heap_->used);
	if (machine._heap_->array_size!=NULL) free(machine._heap_->array_size);
	
	for (int i = 0; i < machine._heap_->trackerCap; ++i)
		if (machine._heap_->arrayrefList[i]!=NULL) free(machine._heap_->arrayrefList[i]);
	if (machine._heap_->arrayrefList!=NULL) free(machine._heap_->arrayrefList);
	if (machine._heap_->countArray!=NULL) free(machine._heap_->countArray);
	if (machine._heap_->listSize!=NULL) free(machine._heap_->listSize);
	if (machine._heap_!=NULL) free(machine._heap_);

	machine.inp = NULL;
	machine.out = NULL;
	dprintf("Finish!\n");
}

void run()
{
	while (machine.pc < machine.text_size) {
		if (!step()) break;
	}
}

bool step() {
	if(machine.pc >= machine.text_size) return false;
	switch(machine.text[machine.pc]){
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
		// ### HEAP ###
		case OP_NEWARRAY:
			NEWARRAY();
			break;
		case OP_IASTORE:
			IASTORE();
			break;
		case OP_IALOAD:
			IALOAD();
			break;
		case OP_GC:
			GC();
			break;
		// ############
		default:
			dprintf("Couldn't find the corresponding OP_CODE:%02x\n", machine.text[machine.pc]);
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
	return machine._stack_->Array;
}

int text_size() {
	return machine.text_size;
}

byte_t* get_text() {
	return machine.text;
}

int get_program_counter() {
	return machine.pc;
}

word_t get_local_variable(int i){
	if (machine._stack_->lv == 0) return machine._stack_->mainLocalVar[i];
	else return machine._stack_->Array[machine._stack_->lv + 1 + i];
}

byte_t get_instruction(){
	return machine.text[machine.pc];
}

bool finished(){
	return machine.pc == machine.text_size;
}

void set_input(FILE *fp)
{
  	machine.inp = fp;
}

void set_output(FILE *fp)
{
	machine.out = fp;
}