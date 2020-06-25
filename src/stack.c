#include "stack.h"
#include "state.h"
#include <assert.h>
#include <stdlib.h>

extern ijvm_t machine;

inline void checkEmpty(){
	if(machine._stack_->sp < 0){
		dprintf("The stack is currently empty!\n");
		assert(false);
	}
}

word_t top(){	
	checkEmpty();
	return machine._stack_->Array[machine._stack_->sp];
}

word_t pop(){
	word_t res = top();
	machine._stack_->sp--;
	return res;
}

word_t stackSize(){
	return machine._stack_->sp+1;
}

inline void push(word_t new_top){
	if (machine._stack_->sp + 1 == machine._stack_->capacity){
		machine._stack_->capacity *= 2;
		machine._stack_->Array = realloc(machine._stack_->Array, machine._stack_->capacity * sizeof(word_t));
		dprintf("Stack expanded\n");
	}
	machine._stack_->Array[++machine._stack_->sp] = new_top;
}