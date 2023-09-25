#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include "utils.h"
#include "symbol_table.h"

/*
DATA TRANSFER
*/

#define SDT_BASE 0xb8000000
#define LOADLIT_BASE 0x18000000

// Shifted bases for addressing modes
#define REG_OFFSET_BASE 0x00206800
#define PRE_INDEX_BASE 0x00000c00
#define POST_INDEX_BASE 0x00000400

// Single Data Processing Constants
#define SDT_REGISTEROFFSET_BIT 10 // tests for register offset transfer type
#define SDT_UNSIGNED_OFFSET_FLAG_BIT 24
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

extern symbol_table* st;

uint32_t dataTransferInstruction(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    uint32_t instr = SDT_BASE | (is64BitReg(arg1) << SDT_SFBIT_POS) | (getRegNum(arg1) << SDT_RT_START);

    char* xn = (char*) malloc(MAX_CHARS_IN_LINE * sizeof(char));
    char* xm = (char*) malloc(MAX_CHARS_IN_LINE * sizeof(char));
    char* imm = (char*) malloc(MAX_CHARS_IN_LINE * sizeof(char));
    char* simm = (char*) malloc(MAX_CHARS_IN_LINE * sizeof(char));
    // Pre-Indexed
    if (strstr(arg2, "!")) {
        xn = strtok(arg2, ",");
        simm = strtok(NULL, "]");
        return instr | (getRegNum(xn) << SDT_XN_START) | ((getImmediate(simm) & generateMask(SIMM9_LEN)) << SDT_SIMM9_START) | PRE_INDEX_BASE; 
    }
    // Post-Indexed (3rd argument #<simm> exists)
    if (strcmp(arg3, "") != 0) {
        sscanf(arg2, "[%s]", xn);
        return instr | (getRegNum(xn) << SDT_XN_START) | ((getImmediate(arg3) & generateMask(SIMM9_LEN)) << SDT_SIMM9_START) | POST_INDEX_BASE; 
    }

    // Register Offset
    regex_t regex;
    int reg_comp_value;
    reg_comp_value = regcomp(&regex, "\[\\w+, \\w+\\]", 0);
    if (reg_comp_value != 0) {
        fprintf(stderr, "regex compilation error");
    }
    if (regexec(&regex, arg2, 0, NULL, 0) == 0 && strstr(arg2, "#") == NULL){
        xn = strtok(arg2, "[,]");
        xm = strtok(NULL, "[,]");
        return instr | (getRegNum(xn) << SDT_XN_START) | (getRegNum(xm) << SDT_XM_START) | REG_OFFSET_BASE;
    }

    // Unsigned Immediate Offset
    if (regexec(&regex, arg2, 0, NULL, 0) == 0 && strstr(arg2, "#") != NULL){
        // Add # back to start of imm
        xn = strtok(arg2, "[,]");
        imm = strtok(NULL, "[,]");
        // Determine imm12 encoding based on register bitwidth
        uint32_t imm12;
        if (is64BitReg(arg1)) {
            imm12 = calculateOffset(imm, address, IMM12_LEN) / BYTES_IN_64BIT;
        } else {
            imm12 = calculateOffset(imm, address, IMM12_LEN) / BYTES_IN_32BIT;
        }
        return instr | (getRegNum(xn) << SDT_XN_START) | (imm12 << SDT_IMM12_START) | (1 << SDT_UBIT_POS);
    }
    
    // Zero Unsigned Offset
    if (sscanf(arg2, "[%s]", xn)) {
        return instr | (getRegNum(xn) << SDT_XN_START) | (1 << SDT_UNSIGNED_OFFSET_FLAG_BIT);
    }
    // Offset operand is not of valid form
    exit(EXIT_FAILURE);
}

uint32_t ldr(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    if (strncmp(arg2, "[", 1) != 0) {
        return LOADLIT_BASE | (getRegNum(arg1) << SDT_RT_START) | ((calculateOffset(arg2, address, SIMM19_LEN) / INSTRUCTION_SIZE) << SDT_SIMM19_START) | (is64BitReg(arg1) << SDT_SFBIT_POS);
    } else {
        return dataTransferInstruction(arg1, arg2, arg3, arg4, address) | (1 << SDT_LBIT_POS);
    }
}

uint32_t str(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    // L bit is 0.
    return dataTransferInstruction(arg1, arg2, arg3, arg4, address);
}

/*
SPECIAL INSTRUCTIONS AND DIRECTIVE
*/

#define SPECIAL_NOP_CODE 0xd503201f

uint32_t nop(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    return SPECIAL_NOP_CODE;
}

uint32_t intdir(char* arg1, char* arg2, char* arg3, char* arg4, uint32_t address) {
    // Treat number as hex if it starts with "0x"
    if (strncmp(arg1, "0x", 2) == 0) {
        return (uint32_t) strtol(arg1, NULL, HEX_BASE);
    }
    return (uint32_t) strtol(arg1, NULL, DENARY_BASE);
}
