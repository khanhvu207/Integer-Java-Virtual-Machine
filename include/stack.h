#ifndef STACK_H
#define STACK_H
#include "ijvm.h"

#define LOCAL_MAX (1<<16)
#define MAX_FRAME 1000
#define MAX_LV 100

typedef struct Stack {
	word_t sp;
	word_t lv;
	word_t* Array;
	word_t capacity;
	
	word_t** frameLocalVar;
	word_t frameIndex;
} Stack;

void checkEmpty();
word_t pop();
word_t top();
word_t stackSize();
void push(word_t new_top);

#endif