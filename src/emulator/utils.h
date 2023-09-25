#include "defs.h"

// Gets instruction type given instruction.
INSTRUCTION_TYPE getInstructionType(uint32_t instruction);

// Outputs state of ARM processor into .out file.
void outputState(ARM* arm, char *file);

// Given a array of memory and a binary file, data from file will be loaded into array.
void loadBinary(uint8_t* memory, char* path);

// Returns word from byte addressable memory
uint32_t getWord(uint8_t* memory);

// Returns double word from byte addressable memory
uint64_t getDoubleWord(uint8_t* memory);

// Rotate right
uint64_t ror(uint64_t value, uint32_t shift, bool is64bit);

// Arithemtic shift right
uint64_t asr(uint64_t value, uint32_t shift, bool is64bit);

// Logical shift left
uint64_t lsl(uint64_t value, uint32_t shift, bool is64bit);
// Logical shift right
uint64_t lsr(uint64_t value, uint32_t shift, bool is64bit);

// Gets bit at kth position from n.
bool getBitAt(uint64_t n, int k);

// Gets l bits upwards starting from kth positon of n.
uint64_t getBitsAt(uint64_t n, int k, int l);

// Sets clearsize bits starting from kth position of n to new to size copysize.
uint64_t setBitsTo(uint64_t n, int k, uint64_t new, int l);

// Sign extends n at lth bit
uint64_t extendBits(uint64_t n, int l);
