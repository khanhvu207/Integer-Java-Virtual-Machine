#include "heap.h"
#include "state.h"
#include <assert.h>
#include <stdlib.h>

extern ijvm_t machine;

int getNewArrayID(){
	int arrayref;
	for (int i = 0; i < machine._heap_->heap_size; ++i) 
		if (!machine._heap_->used[i]){
			arrayref = i;
			break;
		}
	// If the machine couldn't find a free space (Heap full!), then call resizeHeap()
	resizeHeap();
	for (int i = 0; i < machine._heap_->heap_size; ++i) 
		if (!machine._heap_->used[i]){
			arrayref = i;
			break;
		}
	return arrayref;
}

inline void allocateNewArray(int arrayref, int arraySize){
	machine._heap_->used[arrayref] = true;
	machine._heap_->array_ptr[arrayref] = malloc(sizeof(word_t) * arraySize);
	machine._heap_->array_size[arrayref] = arraySize;
}

inline void isOutofBound(int arrayref, int index){
	if (index >= machine._heap_->array_size[arrayref]){
		dprintf("Array %d at %d: Out of bound!\n", arrayref, index);
		assert(false);
	}
}

word_t arrayLoad(int arrayref, int index){
	isOutofBound(arrayref, index);
	return machine._heap_->array_ptr[arrayref][index];
}

inline void arrayStore(int arrayref, int index, word_t val){
	isOutofBound(arrayref, index);
	machine._heap_->array_ptr[arrayref][index] = val;
}

inline void freeArray(int arrayref){
	machine._heap_->used[arrayref] = false;
	if (machine._heap_->array_ptr[arrayref]!=NULL)
		free(machine._heap_->array_ptr[arrayref]);
	machine._heap_->array_size[arrayref] = 0;
	machine._heap_->array_ptr[arrayref] = NULL;
}

inline void resizeHeap(){
	machine._heap_->heap_size *= 2;
	machine._heap_->array_ptr = realloc(machine._heap_->array_ptr, machine._heap_->heap_size * sizeof(word_t*));
	machine._heap_->array_size = realloc(machine._heap_->array_size, machine._heap_->heap_size * sizeof(word_t));
	machine._heap_->used = realloc(machine._heap_->used, machine._heap_->heap_size * sizeof(bool));
	dprintf("Heap expanded\n");
}

// ### GARBAGE COLLECTION ###

inline void triggerOnNewFrame(){
	machine._heap_->currentFrame++;
	int frameIndex = machine._heap_->currentFrame;
	machine._heap_->countArray[frameIndex] = 0;
	machine._heap_->listSize[frameIndex] = 1;
	machine._heap_->arrayrefList[frameIndex] = malloc(machine._heap_->listSize[frameIndex] * sizeof(word_t));
}

inline void trackNewArray(int arrayref){
	int frameIndex = machine._heap_->currentFrame;
	if (++machine._heap_->countArray[frameIndex] > machine._heap_->listSize[frameIndex]) resizeList();
	machine._heap_->arrayrefList[frameIndex][machine._heap_->countArray[frameIndex]-1] = arrayref;
}

inline void resizeList(){
	int frameIndex = machine._heap_->currentFrame;
	machine._heap_->listSize[frameIndex] *= 2;
	machine._heap_->arrayrefList[frameIndex] = realloc(machine._heap_->arrayrefList[frameIndex], machine._heap_->listSize[frameIndex] * sizeof(word_t));
}

inline void sweep(int frameIndex){
	if (machine._heap_->arrayrefList[frameIndex]==NULL) return;
	for (int i = 0; i < machine._heap_->countArray[frameIndex]; ++i){
		int arrayref = machine._heap_->arrayrefList[frameIndex][i];
		if(!machine._heap_->used[arrayref]) continue;
		freeArray(arrayref);
	}

	// clean up tracker arrays
	if (machine._heap_->arrayrefList[frameIndex]!=NULL) free(machine._heap_->arrayrefList[frameIndex]);
	machine._heap_->arrayrefList[frameIndex] = NULL;
	machine._heap_->countArray[frameIndex] = 0;
	machine._heap_->listSize[frameIndex] = 0;
}