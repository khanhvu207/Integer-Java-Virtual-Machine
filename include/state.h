#ifndef STATE_H
#define STATE_H
#include "ijvm.h"
#include "stack.h"
#include "heap.h"
#include "util.h"
#include "net.h"
#include <stdlib.h>
#include <string.h>

#define MAX_MEMORY 8096
#define HEADER_SIZE 4
#define BLOCK_SIZE 4
#define SHORT_SIZE 2
#define WORD_SIZE 4

typedef struct ijvm_t {
	word_t pc;
	bool isWIDE;
	bool isRunning;
	word_t invk_cnt;
	
	byte_t* binary;
	word_t binary_size;

	byte_t* file_header;

	word_t* constant;
	word_t constant_origin;
	word_t constant_size;

	byte_t* text;
	word_t text_origin;
	word_t text_size;

	Stack* _stack_;

	Heap* _heap_;

	FILE* inp;
	FILE* out;
} ijvm_t;

void printMachineBinary();
bool checkFileHeader();
void initializeConstant();
void initializeText();
void initializeStack();
void initializeHeap();

#endif