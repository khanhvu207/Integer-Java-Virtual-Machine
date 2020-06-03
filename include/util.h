#ifndef UTIL_H
#define UTIL_H
#include "ijvm.h"
#include "state.h"
#include <stdlib.h>
#include <string.h>

/* debug print */
#ifdef DEBUG

#define dprintf(...) \
    fprintf(stderr,  __VA_ARGS__)
#else
#define dprintf(...)
#endif

word_t HexBytestoInt(byte_t* bytes);
void printMachineBinary(ijvm_t* machine);
bool checkFileHeader(ijvm_t* machine);
void initializeConstant(ijvm_t* machine);
void initializeText(ijvm_t* machine);

#endif
