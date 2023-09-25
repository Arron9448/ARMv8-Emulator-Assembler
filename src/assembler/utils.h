#include <stdint.h>
#include <stdbool.h>
#include "defs.h"

// Writes n instructions from array into binary file
void writeBinary(char* path, uint32_t* instructions, int n);

// Reads each line of file into buffer with maximum of MAX_LINES lines. Returns number of lines read.
uint8_t readFile(char buffer[MAX_LINES][MAX_CHARS_IN_LINE], char* path);

// Takes read files and adds any labels and corresponding addresses to symbol table.
uint8_t populateSymbolTable(char buffer[MAX_LINES][MAX_CHARS_IN_LINE], symbol_table* st, uint8_t numRead);

// Returns the hash used to find the instruction function given the opcode.
uint8_t hash(char* t);

// Checks if operand is a 64bit register
bool is64BitReg(char* operand);

// Returns string for correct zero register
char* getZeroReg(char* operand);

// Returns register number from operand
uint32_t getRegNum(char* operand);

// Returns literal number for operand starting with #
uint32_t getImmediate(char* operand);

// check if line is blank
bool isBlankLine(char *line);

// Returns if given operand is a register.
// If returns false, assume that it is a literal.
bool isRegister(char *operand);

// Returns if given operand is an immediate value.
bool isImmediate(char *operand);

// Remvoes all lines that return true when passedi nto fn.
uint8_t skipBlanks(char buffer[MAX_LINES][MAX_CHARS_IN_LINE], int numRead);

// Removes all whitespace from input string
void removeWhitespace(char* str);

// Removes potential leading and trailing whitespace
void trimWhitespace(char* str);

// Calculates of the offset between an operand and the line number of the instruction. 
uint32_t calculateOffset(char* operand, uint8_t lineaddress, uint8_t len);

// generates binary mask of n ones.
uint64_t generateMask(uint32_t n);