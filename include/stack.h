#ifndef STACK_H
#define STACK_H
#include "ijvm.h"

typedef struct Stack {
	word_t sp;
	word_t* _array_;
	word_t _capacity_;
} Stack;

void checkEmpty();
word_t pop();
word_t top();
word_t stackSize();
void push(word_t new_top);

#endif