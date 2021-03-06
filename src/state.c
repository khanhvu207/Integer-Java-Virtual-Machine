#include "state.h"

extern ijvm_t machine;

void printMachineBinary() {
	#define COLUMNS 8

	dprintf("\n");
	dprintf("%d bytes in total.\n", machine.binary_size);
	for(int i = 0; i < machine.binary_size; ++i) {
		dprintf("%02x ", machine.binary[i] & 0xff);
		if (i > 0 && (i+1) % COLUMNS == 0)
			dprintf("\n");
	}
	
	dprintf("\n");
	#undef COLUMNS
}

bool checkFileHeader() {
	byte_t* header = malloc(HEADER_SIZE * sizeof(byte_t));

	strncpy((char*)header, (const char*)machine.binary, HEADER_SIZE);

	word_t num = BytestoInt(header);
	free(header);
	if (num != MAGIC_NUMBER) return false;
	fprintf(stderr, "Starting IJVM...\n");
	return true;
}

void initializeConstant() {
	byte_t* Array = malloc(BLOCK_SIZE * sizeof(byte_t));
	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine.binary[BLOCK_SIZE + i];
	machine.constant_origin = BytestoInt(Array);

	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine.binary[BLOCK_SIZE * 2 + i];
	machine.constant_size = BytestoInt(Array) / BLOCK_SIZE;
	dprintf("# of constants: %d\n", machine.constant_size);

	machine.constant = malloc(machine.constant_size * sizeof(word_t));

	for(int i = 0, chunk = 0; i < machine.constant_size;  ++i, chunk += BLOCK_SIZE){
		machine.constant[i] = BytestoInt(machine.binary + BLOCK_SIZE * 3 + chunk);
		dprintf("%d ", machine.constant[i]);
	}	
	dprintf("\n");
	free(Array);
}

void initializeText() {
	int startByte = BLOCK_SIZE * 3 + machine.constant_size * BLOCK_SIZE;
	byte_t* Array = malloc(BLOCK_SIZE * sizeof(byte_t));
	
	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine.binary[startByte + i];
	machine.text_origin = BytestoInt(Array);

	for (int i = 0; i < BLOCK_SIZE; ++i) 
		Array[i] = machine.binary[startByte + BLOCK_SIZE + i];
	machine.text_size = BytestoInt(Array);

	machine.text = malloc(machine.text_size * sizeof(byte_t));

	for (int i = 0; i < machine.text_size; ++i)
		machine.text[i] = machine.binary[startByte + BLOCK_SIZE * 2 + i];

	free(Array);
	// #ifdef DEBUG
	// 	dprintf("\n---TEXT extracted from the binary---\n");
	// 	for (int i = 0; i < machine._text_size_; ++i)
	// 		dprintf("%02x ", machine._text_[i]);
	// 	dprintf("\n");
	// #endif
}

void initializeStack(){
	machine._stack_ = malloc(sizeof(Stack));
	machine._stack_->capacity = machine.text_size;
	machine._stack_->Array = malloc(sizeof(word_t) * machine._stack_->capacity);
	machine._stack_->frameLocalVar = malloc(MAX_FRAME * sizeof(word_t*));
	for (int i = 0; i < MAX_FRAME; ++i)
		machine._stack_->frameLocalVar[i] = malloc(MAX_LV * sizeof(word_t));
	machine._stack_->frameLocalVar[0] = malloc(LOCAL_MAX * sizeof(word_t));
	machine._stack_->frameIndex = 0;
	machine._stack_->sp = -1;
	machine._stack_->lv = 0;
}

void initializeHeap(){
	machine._heap_ = malloc(sizeof(Heap));
	machine._heap_->heap_size = INIT_ARRAY_SLOT;
	machine._heap_->array_ptr = malloc(INIT_ARRAY_SLOT * sizeof(word_t*));
	machine._heap_->used = calloc(INIT_ARRAY_SLOT, sizeof(bool)); // calloc will set the memory to FALSE by default
	machine._heap_->array_size = calloc(INIT_ARRAY_SLOT, sizeof(word_t));
	for (int i = 0; i < INIT_ARRAY_SLOT; ++i) machine._heap_->array_ptr[i] = NULL;

	// Frames's tracker for garbage collection
	machine._heap_->trackerCap = INIT_GC;
	machine._heap_->currentFrame = -1;
	machine._heap_->countArray = malloc(INIT_GC * sizeof(word_t));
	machine._heap_->listSize = malloc(INIT_GC * sizeof(word_t));
	machine._heap_->arrayrefList = malloc(INIT_GC * sizeof(word_t*));
	for (int i = 0; i < INIT_GC; ++i) machine._heap_->arrayrefList[i] = NULL;

	// initialize tracker for main frame
	machine._heap_->currentFrame++;
	machine._heap_->countArray[machine._heap_->currentFrame] = 0;
	machine._heap_->listSize[machine._heap_->currentFrame] = 1;
	machine._heap_->arrayrefList[machine._heap_->currentFrame] = malloc(machine._heap_->listSize[machine._heap_->currentFrame] * sizeof(word_t));
}