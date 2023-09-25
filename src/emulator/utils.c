#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>
#include "defs.h"

// Returns word from byte addressable memory
uint32_t getWord(uint8_t* memory) {
    uint32_t value = 0;
    for (int i = 0; i < BYTES_IN_WORD; i++) {
        value += ((uint32_t)*memory) << (SIZE_OF_BYTE * i);
        memory++;
    }
    return value;
}

// Returns word from byte addressable memory
uint64_t getDoubleWord(uint8_t* memory) {
    return (uint64_t) getWord(memory) + ((uint64_t)getWord(memory + BYTES_IN_WORD) << 32);
}

// Outputs state of ARM processor into .out file.
void outputState(ARM *arm, char *file) {
    FILE* output = fopen(file, "w");

    // Output registers.
    fprintf(output, "Registers: \n");

	for (int i = 0; i < NUM_OF_GENERAL_REGISTERS; i++) {
		fprintf(output, "X%02d = %016lx\n",
			   i, arm->registers[i]);
	}

    fprintf(output, "PC = %016lx\n", arm->pc);

    // Output PSTATE
    fprintf(output, "PSTATE: ");
    fprintf(output, (arm->pstate.N) ? "N" : "-");
    fprintf(output, (arm->pstate.Z) ? "Z" : "-");
    fprintf(output, (arm->pstate.C) ? "C" : "-");
    fprintf(output, (arm->pstate.V) ? "V\n" : "-\n");

    // Output Memory
    fprintf(output, "Non-zero memory:\n");

    for (int i = 0; i < MAX_MEMORY_SIZE; i += 4) {
		if (getWord(&arm->memory[i]) != 0) {
            // Bytes are stored in little endian so have to convert.
            fprintf(output, "0x%08x: %08x\n", i, getWord(&arm->memory[i]));
		}
	}

    fclose(output);
}

// Given a array of memory and a binary file, data from file will be loaded into array.
void loadBinary(uint8_t* memory, char* path) {

    FILE* binary = fopen(path, "rb");

    // If file pointer is null then return with error.
    if (binary == NULL) {
        printf("Error in opening file.\n");
        exit(EXIT_FAILURE);
    }

    // Read data from binary until end of file. Only 1 piece of data read each time.
    int i = 0, read = 1;
    while (read) {
        read = fread(&memory[i], sizeof(uint8_t), 1, binary);
        i++;
    }

    fclose(binary);
    // assert(getWord(&memory[0]) == 0xd28001e1);
    printf("%02x", memory[1]);

    //assert(*memory == 0xe1);
    //assert(memory[1] == 0x01);
}

/*
Bitwise Operations
*/

// Masks top 32 bits to 0 if value is 32 bits.
static uint64_t wregisterMask(uint64_t value, bool is64bit) {
    if (!is64bit) {
        value &= WREGISTER_MASK;
    }
    return value;
}

// Rotate right
uint64_t ror(uint64_t value, uint32_t shift, bool is64bit) {
    assert(shift >= 0);
    int bits = is64bit ? 64 : 32;
    // If 32 bit, mask top 32 bits after shift.
    value = wregisterMask(value, is64bit);
    return (value >> shift) | (value << (bits - shift));
}

// Logical shift left
uint64_t lsl(uint64_t value, uint32_t shift, bool is64bit) {
    assert(shift >= 0);
    value <<= shift;
    // If 32 bit, mask top 32 bits after shift.
    return wregisterMask(value, is64bit);
}

uint64_t lsr(uint64_t value, uint32_t shift, bool is64bit) {
    assert(shift >= 0);
    value >>= shift;
    // If 32 bit, mask top 32 bits after shift.
    return wregisterMask(value, is64bit);
}

// Arithemtic shift right
uint64_t asr(int64_t value, uint32_t shift, bool is64bit) {
    assert(shift >= 0);

    // Cast value to signed 32 bit integer if in 32 bits.
    if (!is64bit) {
        value = (int32_t) value;
    }

    value = value < 0 ? ~(~value >> shift) : value >> shift;
    // If 32 bit, mask top 32 bits after shift.
    return wregisterMask(value, is64bit);
}

// generates binary mask of n ones.
static uint64_t generateMask(uint32_t n) {
    return ((1 << n) - 1);
}

// Gets l bits upwards starting from kth positon of n.
uint64_t getBitsAt(uint64_t n, int k, int l) {
    // In kth position we can get up to k + 1 bits
    assert(k >= 0 && l > 0);
    // Can be converted into getting bits downards by using:
    // return (n >> (k - l + 1)) & generateMask(l)
    return (n >> k) & generateMask(l);
}

// Gets bit at kth position from n.
bool getBitAt(uint64_t n, int k) {
    assert(k >= 0);
    return (n >> k) & 1;
}

// Clears l bits downwards starting from kth position of n to 0 (k is zero indexed).
static uint64_t bitClear(uint64_t n, int k, int l) {
    assert(k >= 0 && l > 0);
    return n & (~(generateMask(l) << (k - l)));
}

// Sets l bits starting from kth position of n to new.
uint64_t setBitsTo(uint64_t n, int k, uint64_t new, int l) {
    printf("%.32lx\n", n);
    uint64_t cleared = bitClear(n, k, l);
    printf("%.32lx\n", cleared);
    return cleared | new << (k - l);
}

// Sign extends n at lth bit
uint64_t extendBits(uint64_t n, int l) {
    n = n & ((1U << l) - 1);
    uint64_t m = 1U << (l - 1);
    return (n ^ m) - m;
}

// Gets instruction type given instruction.
INSTRUCTION_TYPE getInstructionType(uint32_t instruction) {
    uint32_t op0 = getBitsAt(instruction, OP0_START, OP0_LEN);

    if (instruction == HALT_CODE) {
        return HALT;
    } else if (instruction == NOP_CODE) {
        return NOP;
    } else if ((op0 & 0xe) == 0x8) {
        return DATA_PROCESSING_IMMEDIATE;
    } else if ((op0 & 0x7) == 0x5) {
        return DATA_PROCESSING_REGISTER;
    } else if ((op0 & 0x5) == 0x4) {
        return SINGLE_DATA_TRANSFER;
    } else if ((op0 & 0xe) == 0xa) {
        return BRANCH;
    } else {
        // Default case; not instruction.
        return DATA;
    }
}
