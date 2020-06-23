#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include "ijvm.h"

void BIPUSH();
void DUP();
void IADD();
void ISUB();
void IAND();
void IOR();
void POP();
void SWAP();
void IN();
void OUT();
void HALT();
void ERR();
void NOP();
void GOTO();
void IFEQ();
void IFLT();
void IF_ICMPEQ();
void LDC_W();
void ILOAD(bool WIDE);
void ISTORE(bool WIDE);
void IINC(bool WIDE);
void INVOKEVIRTUAL();
void IRETURN();

// HEAP
void NEWARRAY();
void IALOAD();
void IASTORE();
void GC();

#endif