#ifndef STATE_H
#define STATE_H
#include "ijvm.h"

#define MAX_MEMORY 512
#define HEADER_SIZE 4
#define BLOCK_SIZE 4

typedef struct ijvm_t {
	word_t pc;
	bool _machine_state_;

	byte_t* _binary_;
	word_t _binary_size_;

	byte_t* _file_header_;

	byte_t* _constant_;
	word_t _constant_origin_;
	word_t _constant_size_;

	byte_t* _text_;
	word_t _text_origin_;
	word_t _text_size_;

	FILE* inp;
	FILE* out;
} ijvm_t;

#endif