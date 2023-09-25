#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "defs.h"
#include "symbol_table.h"

extern symbol_table* st;

// Writes n instructions from array into binary file
void writeBinary(char* path, uint32_t* instructions, int n) {
	// Creating the output file
	FILE* output = fopen(path, "wb");

	// Check if file has been loaded into memory
	if(output == NULL) {
		fprintf(stderr, "Error opening file at %s\n", path);
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < n; i++) {
		fwrite(&(instructions[i]), sizeof(uint32_t), 1, output);
	}

	fclose(output);
}

// Reads each line of file into buffer with maximum of MAX_LINES lines. Returns number of lines read.
uint8_t readFile(char buffer[MAX_LINES][MAX_CHARS_IN_LINE], char* path) {

	// Input file
	FILE* input = fopen(path, "r");

	// Verifying the input
	if (input == NULL) {
		printf("ERROR: Cannot open file: %s\n", path);
		exit(EXIT_FAILURE);
	}

	// Read content of input file
	uint8_t i = 0;
	while(fgets(buffer[i], MAX_CHARS_IN_LINE, input)) {
		i++;
	}

	fclose(input);
	return i;
}

// Checks if operand is a 64bit register
bool is64BitReg(char* operand) {
	assert(operand != NULL && strlen(operand) > 0);
	// 64 bit registers are of the form "x##"
	return strncmp(operand, "x", 1) == 0;
}

// Returns string for correct zero register
char* getZeroReg(char* operand) {
	return is64BitReg(operand) ? OPERAND_XZR : OPERAND_WZR;
}

// Returns register number from operand
uint32_t getRegNum(char* operand) {
	assert(operand != NULL);

	char* reg = strdup(operand);

	if (strncmp(reg, "[", 1) == 0) {
		reg++;
	}

	if (strncmp(reg, "xzr", 3) == 0 || strncmp(reg, "wzr", 3) == 0) {
		return ZR_INDEX;
	}

	// reg++ moves pointer past w or x index for registers.
	reg++;
	return (uint32_t) atoi(reg);
}


// Returns unsigned literal number for operand starting with #
uint32_t getImmediate(char* operand) {
	assert(operand != NULL);

	// Go past #
	operand++;

	if (strncmp(operand, "0x", 2) == 0) {
        return (uint32_t) strtol(operand, NULL, HEX_BASE);
    }
    return (uint32_t) strtol(operand, NULL, DENARY_BASE);
}

// generates binary mask of n ones.
uint64_t generateMask(uint32_t n) {
    return ((1 << n) - 1);
}

// Returns the hash used to find the instruction function given the opcode.
uint8_t hash(char* t) {

	// Manually check for 'b' and 'br' since they are not long enough to has using the hash function.
	// This avoid accessing memory which is inconsitent (and would break the hash function).
	if (strncmp(t, "nop", 3) == 0) {
		return 53;
	} else if (strlen(t) == 1) {
        return 11;
    } else if (strlen(t) == 2) {
        return 13;
    }

	return (710 * (t[0] ^ t[1]) ^ 21 * (t[2] ^ t[3])) % 59;
}

// check if line is blank
bool isBlankLine(char *line) {
	assert(line != NULL);
	return strcmp(line, "\n") == 0 || strcmp(line, "\0") == 0 || strcmp(line, "\r\n") == 0; 
}

// Returns if given operand is a register.
// If returns false, operand is either label or immediate address.
bool isRegister(char *operand) {
	return (strncmp(operand, "x", 1) == 0 || strncmp(operand, "w", 1) == 0);
}

// Returns if given operand is an immediate value.
bool isImmediate(char *operand) {
	return (strncmp(operand, "#", 1) == 0);
}

// Calculates of the offset between an operand and the line number of the instruction. 
uint32_t calculateOffset(char* operand, uint8_t lineaddress, uint8_t len) {
	uint32_t offset;

    if (isImmediate(operand)) {
        // treat as immediate value
		offset = getImmediate(operand);
    } else {
		// treat as label
		offset = getAddress(st, operand) - lineaddress;	
	}

	// Truncates offset to desired length so it can be bitshifted
	return offset & generateMask(len + MASK_OFFSET) ;
}

// Remvoes all lines that return true when passed into fn.
uint8_t skipBlanks (char buffer[MAX_LINES][MAX_CHARS_IN_LINE], int numRead) {
	int read_line = 0;
	for (int write_line = 0; write_line < numRead; write_line++){
		if (strlen(buffer[write_line]) != 1) {
			for (int k = 0; k < MAX_CHARS_IN_LINE; k++){
				buffer[read_line][k] = buffer[write_line][k];
			}
			read_line++;
		}
		}	
	for (int i = read_line; i < numRead; i++){
		buffer[i][0] = '\0';
	}
	return read_line;	
}

// Removes all whitespace from input string
void removeWhitespace(char* str) {
    int count = 0;
	
    for (int i = 0; str[i]; i++) {
        if (str[i] != ' ') {
            str[count++] = str[i];
		}
	}	
    str[count] = '\0';
}

// Removes potential leading and trailing whitespace
void trimWhitespace(char* str) {
	int i;
    int begin = 0;
    int end = strlen(str) - 1;

    while (isspace((unsigned char) str[begin]))
        begin++;

    while ((end >= begin) && isspace((unsigned char) str[end]))
        end--;

    for (i = begin; i <= end; i++)
        str[i - begin] = str[i];

    str[i - begin] = '\0';
}
