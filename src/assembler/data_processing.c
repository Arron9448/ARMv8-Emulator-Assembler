#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "defs.h"

#define DP_SFBIT_POS 31
#define DP_RD_START 0
#define DP_OPC_START 29
#define DP_RN_START 5
#define SHIFT_OPLEN 3

// Immediate Data Processing
#define DPI_BASE 0x10000000
#define DPI_OPI_START 23
#define DPI_IMM12_START 10 // for arithmetic
#define DPI_IMM16_START 5 // for logical

// Register Data Processing
#define DPR_BASE 0x0a000000
#define DPR_RM_START 16
#define DPR_SHIFT_START 22 // for arithemtic/logical
#define DPR_IMM6_START 10 // for arithemtic/logical

// Shift function and opcodes
#define DP_SHIFT_LSL 0b0
#define DP_SHIFT_LSR 0b1
#define DP_SHIFT_ASR 0b10
#define DP_SHIFT_ROR 0b11
static uint32_t getShiftNum(char* operand) {
    assert(operand != NULL);
    if (strcmp(operand, "lsl") == 0) {
        return DP_SHIFT_LSL;
    } else if (strcmp(operand, "lsr") == 0) {
        return DP_SHIFT_LSR;
    } else if (strcmp(operand, "asr") == 0) {
        return DP_SHIFT_ASR;
    } else if (strcmp(operand, "ror") == 0) {
        return DP_SHIFT_ROR;
    }
    return 0;
}


/*
ARITHMETIC
*/

#define DPI_ARITHEMTIC_OPI 0b010
#define DPR_ARITHMETICBIT_POS 24
#define DP_ADD_OPC 0b0
#define DP_ADDS_OPC 0b1
#define DP_SUB_OPC 0b10
#define DP_SUBS_OPC 0b11
#define DPI_SHBIT_POS 22 // for arithmetic

static uint32_t arithmeticInstructions(char* arg1, char* arg2, char* arg3, char* arg4) {
    uint32_t instr = (getRegNum(arg1) << DP_RD_START) | (getRegNum(arg2) << DP_RN_START) | (is64BitReg(arg1) << DP_SFBIT_POS);

    if (isRegister(arg3)) {
        // Data processing register
        instr |= DPR_BASE | (getRegNum(arg3) << DPR_RM_START) | (1 << DPR_ARITHMETICBIT_POS);
    
        // Optional shift
        if (strcmp(arg4, "") != 0) {
            instr |= (getShiftNum(strndup(arg4, SHIFT_OPLEN)) << DPR_SHIFT_START); 
            arg4 += SHIFT_OPLEN;
            instr |= (getImmediate(arg4) << DPR_IMM6_START);
        }

    } else {
        // Data processing immediate
        instr |= DPI_BASE | (DPI_ARITHEMTIC_OPI << DPI_OPI_START) | (getImmediate(arg3) << DPI_IMM12_START);

        // Optional shift
        if (strcmp(arg4, "") != 0) {
            arg4 += SHIFT_OPLEN;
            if (getImmediate(arg4) != 0) {
                instr |= (1 << DPI_SHBIT_POS);
            }
        }
    }

    return instr;
}

uint32_t add(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return arithmeticInstructions(arg1, arg2, arg3, arg4) | (DP_ADD_OPC << DP_OPC_START);
}

uint32_t adds(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return arithmeticInstructions(arg1, arg2, arg3, arg4) | (DP_ADDS_OPC << DP_OPC_START);
}

uint32_t sub(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return arithmeticInstructions(arg1, arg2, arg3, arg4) | (DP_SUB_OPC << DP_OPC_START);
}

uint32_t subs(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return arithmeticInstructions(arg1, arg2, arg3, arg4) | (DP_SUBS_OPC << DP_OPC_START);
}

uint32_t cmp(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return subs(getZeroReg(arg1), arg1, arg2, arg3, address);
}

uint32_t cmn(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return adds(getZeroReg(arg1), arg1, arg2, arg3, address);
}

uint32_t neg(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return sub(arg1, getZeroReg(arg1), arg2, arg3, address);
}

uint32_t negs(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return subs(arg1, getZeroReg(arg1), arg2, arg3, address);
}

/*
LOGICAL
*/

#define DPR_AND_OPC 0b0
#define DPR_ORR_OPC 0b1
#define DPR_EOR_OPC 0b10
#define DPR_ANDS_OPC 0b11
#define DPR_NBIT_POS 21

static uint32_t logicalInstructions(char* arg1, char* arg2, char* arg3, char* arg4) {
    uint32_t instr = DPR_BASE | (is64BitReg(arg1) << DP_SFBIT_POS) | (getRegNum(arg1) << DP_RD_START) |
    (getRegNum(arg2) << DP_RN_START) | (getRegNum(arg3) << DPR_RM_START);

    // Optional shift
    // Extra checks to avoid broken halt codes
    if (strcmp(arg4, "") != 0 && (strcmp(arg1, "x0") != 0 || strcmp(arg2, "x0") != 0 || strcmp(arg3, "x0") != 0)) {
            instr |= (getShiftNum(strndup(arg4, SHIFT_OPLEN)) << DPR_SHIFT_START); 
            arg4 += SHIFT_OPLEN;
            fprintf(stderr, "arg4 is %s", arg4);
            instr |= (getImmediate(arg4) << DPR_IMM6_START);
        }

    return instr;
}

uint32_t and(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return logicalInstructions(arg1, arg2, arg3, arg4) | (DPR_AND_OPC << DP_OPC_START);
}

uint32_t bic(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return and(arg1, arg2, arg3, arg4, address) | (1 << DPR_NBIT_POS);
}

uint32_t ands(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return logicalInstructions(arg1, arg2, arg3, arg4) | (DPR_ANDS_OPC << DP_OPC_START);
}

uint32_t bics(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return ands(arg1, arg2, arg3, arg4, address) | (1 << DPR_NBIT_POS);
}

uint32_t eor(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return logicalInstructions(arg1, arg2, arg3, arg4) | (DPR_EOR_OPC << DP_OPC_START);
}

uint32_t eon(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return eor(arg1, arg2, arg3, arg4, address) | (1 << DPR_NBIT_POS);
}

uint32_t orr(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return logicalInstructions(arg1, arg2, arg3, arg4) | (DPR_ORR_OPC << DP_OPC_START);
}

uint32_t orn(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return orr(arg1, arg2, arg3, arg4, address) | (1 << DPR_NBIT_POS);
}

uint32_t tst(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return ands(getZeroReg(arg1), arg1, arg2, arg3, address);
}

uint32_t mov(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return orr(arg1, getZeroReg(arg1), arg2, arg3, address);
}

uint32_t mvn(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return orr(arg1, getZeroReg(arg1), arg2, arg3, address);
}

/*
WIDE MOVE
*/

#define DPI_WIDEMOV_OPI 0b101
#define DPI_MOVK_OPC 0b11
#define DPI_MOVZ_OPC 0b10
#define DPI_MOVN_OPC 0b00
#define DPI_HW_START 21 // for wide move
#define DPI_HW_OFFSET 16

static uint32_t wideMoveInstructions(char* arg1, char* arg2, char* arg3, char* arg4) {
    uint32_t instr = DPI_BASE | (is64BitReg(arg1) << DP_SFBIT_POS) | (getRegNum(arg1) << DP_RD_START) |
    (DPI_WIDEMOV_OPI << DPI_OPI_START) | (getImmediate(arg2) << DPI_IMM16_START);

    // If optional hw shift is given
    if (strcmp(arg3, "") != 0) {
        // Add shift value
        arg3 += SHIFT_OPLEN;
        instr |= ((getImmediate(arg3) / DPI_HW_OFFSET) << DPI_HW_START);
    }

    return instr;
}

uint32_t movk(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return wideMoveInstructions(arg1, arg2, arg3, arg4) | (DPI_MOVK_OPC << DP_OPC_START);
}

uint32_t movn(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return wideMoveInstructions(arg1, arg2, arg3, arg4) | (DPI_MOVN_OPC << DP_OPC_START);
}

uint32_t movz(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return wideMoveInstructions(arg1, arg2, arg3, arg4) | (DPI_MOVZ_OPC << DP_OPC_START);
}

/*
MULTIPLY
*/

#define DPR_MUL_BASE 0x1b000000
#define DPR_RA_START 10 // for multiply
#define DPR_XBIT_POS 15 // for multiply

uint32_t madd(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return DPR_MUL_BASE | getRegNum(arg1) | (getRegNum(arg2) << DP_RN_START) |
    (getRegNum(arg3) << DPR_RM_START) | (getRegNum(arg4) << DPR_RA_START) | (is64BitReg(arg1) << DP_SFBIT_POS);
}

uint32_t msub(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return madd(arg1, arg2, arg3, arg4, address) | (1 << DPR_XBIT_POS);
}

uint32_t mul(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return madd(arg1, arg2, arg3, getZeroReg(arg1), address);
}

uint32_t mneg(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return msub(arg1, arg2, arg3, getZeroReg(arg1), address);
}
