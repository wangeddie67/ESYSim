#ifndef __INSTMANIP_H__
#define __INSTMANIP_H__
#include "host.h"

int GetMemAccWidth(enum md_opcode op);
bool_t isFPMemAcc(enum md_opcode op);
void formatDataM2R(enum md_opcode op, qword_t memData, qword_t *regData);
void formatDataR2M(enum md_opcode op, qword_t regData, qword_t *memData);
void getRegValue(enum md_opcode op, context *current, int ra, qword_t *data);

#endif
