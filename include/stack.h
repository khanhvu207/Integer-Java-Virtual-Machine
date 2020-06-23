#ifndef STACK_H
#define STACK_H
#include "ijvm.h"

#define MAINREF 0xFFFFFFFF
#define LOCAL_MAX (1<<16)

typedef struct Stack {
	word_t sp;
	word_t lv;
	word_t* Array;
	word_t capacity;
	word_t* mainLocalVar; // main()'s local variables
} Stack;

void checkEmpty();
word_t pop();
word_t top();
word_t stackSize();
void push(word_t new_top);
void loadMainVar(unsigned short idx);
void storeMainVar(unsigned short idx, word_t val);

#endif