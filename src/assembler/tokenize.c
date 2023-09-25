#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "defs.h"
#include "utils.h"

// Convert a line known to be an instruction into an instruction type
void tokenizeInstruction(char* line, instruction* instr) {
    char* saveptr;
    char* token;

    // Get first token from line known to be instruction
    token = strtok_r(line, " ", &saveptr); 
    // If first token is null then return with error
    assert(token != NULL);

    // Obtain opcode from first token
    instr->opcode = token;

    // Get second token (Required by loop structure) and initialise operand array builder
    token = strtok_r(NULL, ",\n", &saveptr);

    // Add operand tokens to operand array builder until end of line
    int i = 0;
    while (token != NULL) {
        // Make copy of token for modification that does not interfere with strtok
        char* formatted_token = strdup(token);
        removeWhitespace(formatted_token);
        // Deal with in-operand delimiter cases
        if (formatted_token[0] == '[' && token[strlen(token) - 1] != ']') {
            // Merge succesive tokens contained within a set of square brakcets
            strcat(formatted_token, ",");
            token = strtok_r(NULL, ",\n", &saveptr);
            char* next_formatted_token = strdup(token);
            removeWhitespace(next_formatted_token);
            char* concat_token = (char*) malloc(strlen(formatted_token) + strlen(next_formatted_token));
            strcat(concat_token, formatted_token);
            strcat(concat_token, next_formatted_token);
            instr->operands[i] = concat_token;
        } else {
            instr->operands[i] = formatted_token;
        }
        token = strtok_r(NULL, ",\n", &saveptr);
        i++;
    }

    for (int j = i; j < MAX_OPERANDS; j++) {
        instr->operands[j] = "";
    }
}