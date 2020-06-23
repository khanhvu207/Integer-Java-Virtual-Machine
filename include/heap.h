#ifndef HEAP_H
#define HEAP_H
#include "ijvm.h"

#define INIT_ARRAY_SLOT 1000
#define INIT_GC 100

#define OP_NEWARRAY         ((byte_t) 0xD1)
#define OP_IALOAD           ((byte_t) 0xD2)
#define OP_IASTORE          ((byte_t) 0xD3)
#define OP_GC               ((byte_t) 0xD4)

typedef struct Heap {
	word_t** array_ptr;
	word_t* array_size;
	word_t heap_size;
	bool* used;

	word_t trackerCap;
	word_t currentFrame;
	word_t* countArray;
	word_t* listSize;
	word_t** arrayrefList;
} Heap;

int getNewArrayID();
void allocateNewArray(int arrayref, int arraySize);
void isOutofBound(int arrayref, int index);
word_t arrayLoad(int arrayref, int index);
void arrayStore(int arrayref, int index, word_t val);
void freeArray(int arrayref);
void resizeHeap();

// Garbage collection
void triggerOnNewFrame();
void trackNewArray(int arrayref);
void resizeList();
void sweep(int frameIndex);

#endif