#ifndef STATE_H
#define STATE_H
#include "ijvm.h"

#define MAX_MEMORY 512

typedef struct ijvm_t {
	word_t pc;
	bool _machine_state_;

	byte_t* _binary_;
	word_t _binary_size_;
} ijvm_t;

#endif