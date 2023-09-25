#include <stdbool.h>
#include <stdint.h>

// Register Constants
#ifndef ZR_INDEX

// Register Constants
#define REGISTER_SIZE 8 // in bytes
#define NUM_OF_GENERAL_REGISTERS 31
#define NUM_OF_REGISTERS 32 // 31 general registers with 1 special register (ZR)
#define ZR_INDEX 31 // Register 32 is zero register (starting from R0); read-only
#define WREGISTER_MASK 0xffffffff // sets top 32 bits to 0.
#define BYTE_MASK 0xff // sets all but bottom 8 bits to 0.
#define REG_INDEX_SIZE 5 // number of bits used in instructions

// Memory Constants
#define MAX_MEMORY_SIZE (1 << 21)
#define BYTES_IN_WORD 4
#define BYTES_IN_DOUBLE_WORD 8
#define SIZE_OF_BYTE 8 // in bits
#define BYTES_IN_64BIT 8
#define BYTES_IN_32BIT 4

// Instruction Constants
// Codes are in big endian
#define INSTRUCTION_SIZE 4 // in bytes
#define HALT_CODE 0x8a000000
#define NOP_CODE 0xd503201f
#define OP0_START 25
#define OP0_LEN 4
#define IMM6_LEN 6
#define SIMM9_LEN 9
#define SIMM12_LEN 12
#define IMM12_LEN 12
#define IMM16_LEN 16
#define SIMM19_LEN 19
#define SIMM26_LEN 26

// Single Data Processing Constants
#define SDT_LOADLITERAL_BIT 29 // tests for load literal transfer type
#define SDT_REGISTEROFFSET_BIT 10 // tests for register offset transfer type
#define SDT_LBIT_POS 22 // load bit
#define SDT_UBIT_POS 24 // unsigned offset bit
#define SDT_SFBIT_POS 30 // 32bit bit
#define SDT_RT_START 0
#define SDT_XN_START 5
#define SDT_IBIT_POS 11 // for pre/post index
#define SDT_SIMM9_START 12 // for pre/post index
#define SDT_XM_START 16 // for register offset
#define SDT_IMM12_START 10 // for unsigned offset
#define SDT_SIMM19_START 5 // for load literal
#define SUBS_64BIT_UMASK 0x7fffffffffffffff
#define SUBS_64BIT_LMASK 0x8000000000000000
#define SUBS_32BIT_UMASK 0x7fffffff
#define SUBS_32BIT_LMASK 0x80000000


// Immediate Data Processing Constants
#define DPI_ARITHMETIC_OPI 0x2
#define DPI_WIDEMOVE_OPI 0x5
#define DPI_MOVK_OPC 0x3
#define DPI_SFBIT 31
#define DPI_OPC_START 29
#define DPI_OPC_LEN 2
#define DPI_OPI_START 23
#define DPI_OPI_LEN 3
#define DPI_RD_START 0
#define DPI_SHBIT 22
#define DPI_IMM12_START 10 // for arithmetic
#define DPI_RN_START 5 // for arithmetic
#define DPI_IMM16_START 5 // for logical
#define DPI_HW_START 21 // for logical
#define DPI_HW_SIZE 2 // for logical
#define DPI_MOVK_OFFSET 16 // used to set bits in movk
#define DPI_SHIFT_VALUE 16

// Register Data Processing Constants
#define DPR_ARITHMETICBIT_POS 24
#define DPR_MBIT_POS 28
#define DPR_MULTIPLY_OPR 0b1000
#define DPR_RD_START 0
#define DPR_RN_START 5
#define DPR_RM_START 16
#define DPR_OPC_START 29
#define DPR_OPC_LEN 2
#define DPR_SFBIT_POS 31
#define DPR_SHIFT_START 22
#define DPR_SHIFT_LEN 2
#define DPR_NBIT_POS 21
#define DPR_RA_START 10 // for multiply
#define DPR_XBIT_POS 15 // for multiply
#define DPR_IMM6_START 10 // for arithemtic/logical

// Branch Constants
#define BR_EQ 0x0 // Equal
#define BR_NE 0x1 // Not equal
#define BR_GE 0xa // Signed greater or equal
#define BR_LT 0xb // Signed less than
#define BR_GT 0xc // Signed greater than
#define BR_LE 0xd // Signed less than or equal
#define BR_AL 0xe // Always
#define BR_SIMM26_START 0
#define BR_SIMM19_START 5
#define BR_XN_START 5
#define BR_COND_START 0
#define BR_COND_LEN 4
#define BR_DET_BITS_START 29
#define BR_DET_BITS_LEN 3
#define BR_DET_BITS_UNCOND 0b000
#define BR_DET_BITS_REG 0b110
#define BR_DET_BITS_COND 0b010

// Enum for Instruction Type
typedef enum {
    DATA_PROCESSING_IMMEDIATE,
    DATA_PROCESSING_REGISTER,
    SINGLE_DATA_TRANSFER,
    BRANCH,
    HALT,
    NOP,
    DATA
} INSTRUCTION_TYPE;

// Enum for Branch Instruction Type
typedef enum {
    UNCONDITIONAL,
    REGISTER,
    CONDITIONAL
} BRANCH_TYPE;

// Enum for Data Transfer instruction type
typedef enum {
    UNSIGNED_OFFSET,
    PRE_INDEX,
    POST_INDEX,
    REGISTER_OFFSET,
    LITERAL_ADDRESS
} TRANSFER_TYPE;

// PSTATE tracks flags triggered by last result.
// N - Negative value; Z - Zero value; C - Carry; V - Overflow.
typedef struct {
    bool N;
    bool Z;
    bool C;
    bool V;
} PSTATE;

// ARM Proccesor
// Registers are 64 bit; Memory is byte addressable (sizeof(char) = 1 byte).
typedef struct {
    uint64_t registers[NUM_OF_REGISTERS];
    uint8_t memory[MAX_MEMORY_SIZE];
    PSTATE pstate;
    uint64_t pc;
} ARM;

#endif
