#ifndef MAX_WORDS_IN_LINE

// Parser Constants
#define MAX_WORDS_IN_LINE 5
#define MAX_OPERANDS 4
#define MAX_CHARS_IN_LINE 128 // Arbitrary choice
#define MAX_LABELS 128 // Arbitrary Choice
#define MAX_LINES 128 // Arbitrary Choice
#define DENARY_BASE 10
#define HEX_BASE 16

// Instruction Constants
#define INSTRUCTION_SIZE 4 // in bytes
#define ZR_INDEX 31
#define OPERAND_WZR "wzr"
#define OPERAND_XZR "xzr"
#define MASK_OFFSET 2
#define SIMM19_LEN 19
#define SIMM9_LEN 9
#define SIMM26_LEN 26
#define IMM12_LEN 12

//Memory Constants
#define BYTES_IN_64BIT 8
#define BYTES_IN_32BIT 4

// Structure for symbol table
// Has to be defined this way since symbol is self-referential
typedef struct symbol symbol;
struct symbol {
    char* label;
    uint64_t address;
    symbol* next; // linked list structure
 };

typedef struct {
    symbol* first;
    uint8_t size; // max size < MAX_LABELS
} symbol_table;

// Structure for assembler file instructions: operation mneumonic and up to four operands
typedef struct {
    char* opcode;
    char* operands[MAX_OPERANDS];
} instruction;

#endif
