
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <stdlib.h>

#include "host.h"
#include "machine.h"
#include "resource.h"
#include "cache.h"
#include "context.h"
#include "ptrace.h"
#include "bpred.h"
#include "resource.h"
#include "syscall.h"
#include "instManip.h"

#define WORDSIZE    8


/*
** Get the width of Load/Store data type as seen by the memory
*/
int GetMemAccWidth(enum md_opcode op)
{
    switch(op) {
		/* Return Size of Byte */
	case LDBU:
	case STB:
		return 1;

		/* Return Size of Half Word (Word as in alpha manual) */
	case LDWU:
	case STW:
		return 2;

		/* Return Size of Word (Long Word as in alpha manual) */
	case LDL:
	case LDL_L:
	case STL:
	case STL_C:
	case LDF:
	case LDS:
	case STF:
	case STS:
		return 4;

		/* Return Size of Quad Word */
	case LDQ:
	case LDQ_L:
	case LDQ_U:
	case STQ:
	case STQ_C:
	case STQ_U:
	case LDG:
	case LDT:
	case STG:
	case STT:
		return 8;

		/* Not a load or store instruction */
	default:
		printf("Fatal: GetMemAccWidth cannot deal with op : %d!\n", op);
		panic("GetMemAccWidth cannot deal with op : %d!\n", op);
		return 0;
	}
}

/*
** If this instruction is a floating point instruction
*/
bool_t isFPMemAcc(enum md_opcode op)
{
	switch(op) {
	case LDF: 
	case LDG: 
	case LDS: 
	case LDT: 
	case STF: 
	case STG: 
	case STS: 
	case STT:
		return TRUE;
	default:
		return FALSE;
	}
}


/*
** Format Memory data to Register data
*/
void formatDataM2R(enum md_opcode op, qword_t memData, qword_t *regData)
{
	switch(op) {
		/* Return As it is. No formating required */
	case LDBU:
	case LDWU:
	case LDQ:
	case LDQ_L:
	case LDQ_U:
	case LDT:
	case STB:
	case STW:
	case STQ:
	case STQ_C:
	case STQ_U:
	case STT:
		*regData = memData;
		return;

		/* Return signed qword */
	case LDL:
	case LDL_L:
	case STL:
	case STL_C:
		{
			sword_t tempData = (sword_t) memData;
			*regData = (qword_t) ((sqword_t) tempData); 
			return;
		}

		/* Special formating for LDS memory data */
	case LDS:
	case STS:
		{
			sqword_t longhold, e1, e2;
			longhold = (word_t) memData;

			e1 = longhold & 0x40000000;
			e2 = (longhold >> 23) & ULL(0x7f);
			if (e1) {	
				if (e2 == ULL(0x3f800000))	
					e2 = ULL(0x7ff);	
				else
					e2 |= ULL(0x400);
			}
			else {	
				if (e2 == 0)
					e2 = 0;
				else
					e2 |= ULL(0x380);
			}

			*regData = (qword_t) ((longhold & ULL(0x80000000)) << 32 | e2 << 52 | (longhold & ULL(0x7fffff)) << 29);
			return;
		}

		/* These load instructions currently not supported */
	case LDF:
	case LDG:
	case STF:
	case STG:
		printf("Fatal: formatDataM2R cannot deal with op - LDF and LDG: %d!\n", op);
		panic("formatDataM2R cannot deal with op - LDF and LDG: %d!\n", op);
		*regData = memData;
		return;

		/* Not a load or store instruction */
	default:
		printf("Fatal: formatDataM2R cannot deal with op : %d!\n", op);
		panic("formatDataM2R cannot deal with op : %d!\n", op);
		*regData = memData;
		return;
	}
}


/*
** Format Register data to Memory data
*/
void formatDataR2M(enum md_opcode op, qword_t regData, qword_t *memData)
{
	switch(op) {
		/* Return as it is */
	case LDBU:
	case LDWU:
	case LDQ:
	case LDQ_L:
	case LDQ_U:
	case LDT:
	case STB:
	case STW:
	case STQ:
	case STQ_C:
	case STQ_U:
	case STT:
		*memData = regData;
		return;

		/* Format modifications required */
	case LDS:
	case STS:
		{
			sqword_t longhold;	
			sword_t inthold;

			longhold = regData;

			inthold = (((longhold >> 32) & ULL(0xc0000000)) | ((longhold >> 29) & ULL(0x3fffffff)));

			*memData = (qword_t) inthold;
			return;
		}

		/* Format modifications required */
	case LDL:
	case LDL_L:
	case STL:
	case STL_C:
		{
			word_t src;
			src = (word_t)(regData & ULL(0xffffffff));
			*memData = (qword_t) src;
			return;
		}


		/* These store instructions currently not supported */
	case LDF:
	case LDG:
	case STF:
	case STG:
		printf("Fatal: formatDataR2M cannot deal with STF and STG op : %d!\n", op);
		panic("formatDataR2M cannot deal with STF and STG op : %d!\n", op);
		*memData = regData;
		return;

		/* Not a load or store instruction */
	default:
		printf("Fatal: formatDataR2M cannot deal with op : %d!\n", op);
		panic("formatDataR2M cannot deal with op : %d!\n", op);
		*memData = regData;
		return;
	}
}

/*
** Get register value of load/store instruction
*/
void getRegValue(enum md_opcode op, context *current, int ra, qword_t *data)
{
	switch(op) {
	case LDBU:
	case STB:
	case LDWU:
	case STW:
	case LDL:
	case LDL_L:
	case STL:
	case STL_C:
	case LDQ:
	case LDQ_U:
	case LDQ_L:
	case STQ:
	case STQ_C:
	case STQ_U:
		if(current->spec_mode)
			*data = current->spec_regs_R[ra];
		else
			*data = current->regs.regs_R[ra];
		return;

	case LDS:
	case STS:
	case LDT:
	case STT:
		if(current->spec_mode)
			*data = current->spec_regs_F.q[ra];
		else
			*data = current->regs.regs_F.q[ra];
		return;

	case LDG:
	case STG:
	case LDF:
	case STF:
		printf("Fatal: getRegValue cannot deal with op : %d!\n", op);
		panic("getRegValue cannot deal with op : %d!\n", op);
		return;

		/* Not a load or store instruction */
	default:
		printf("Fatal: getRegValue cannot deal with op : %d!\n", op);
		panic("getRegValue cannot deal with op : %d!\n", op);
		return;
	}
}
