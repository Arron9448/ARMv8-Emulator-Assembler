#include "defs.h"
#include "utils.h"

// Break a line into tokens label, opcode and operand field(s)
// void tokenize(char* line);

// Convert a line known to be an instruction into an instruction type
void tokenizeInstruction(char* line, instruction* instr);
