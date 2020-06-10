#ifndef UTIL_H
#define UTIL_H
#include "ijvm.h"
// #include "state.h"

/* debug print */
#ifdef DEBUG

#define dprintf(...) \
    fprintf(stderr,  __VA_ARGS__)
#else
#define dprintf(...)
#endif

word_t HexBytestoInt(byte_t* bytes);

#endif
