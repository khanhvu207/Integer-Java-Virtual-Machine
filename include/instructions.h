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
void OUT();
void HALT();
void GOTO();
void IFEQ();
void IFLT();
void IF_ICMPEQ();
void LDC_W();
void ILOAD(bool WIDE);
void ISTORE(bool WIDE);
void IINC(bool WIDE);

#endif