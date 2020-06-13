#ifndef STATE_H
#define STATE_H
#include "ijvm.h"
#include "stack.h"

#define MAX_MEMORY 512
#define HEADER_SIZE 4
#define BLOCK_SIZE 4
#define SHORT_SIZE 2
#define WORD_SIZE 4

typedef struct ijvm_t {
	word_t pc;

	byte_t* _binary_;
	word_t _binary_size_;

	byte_t* _file_header_;

	word_t* _constant_;
	word_t _constant_origin_;
	word_t _constant_size_;

	byte_t* _text_;
	word_t _text_origin_;
	word_t _text_size_;

	Stack* _stack_;

	FILE* inp;
	FILE* out;
} ijvm_t;

void printMachineBinary(ijvm_t* machine);
bool checkFileHeader(ijvm_t* machine);
void initializeConstant(ijvm_t* machine);
void initializeText(ijvm_t* machine);
void initializeStack(ijvm_t* machine);

#endif