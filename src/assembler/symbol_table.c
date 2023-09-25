#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include "utils.h"
#include "defs.h"

// Creates new symbol table.
symbol_table* newSymbolTable() {
    symbol_table* st = (symbol_table*) malloc (sizeof(symbol_table));
    st->first = NULL;
    st->size = 0;
    return st;
}

// Checks whether a given label is already in the table.
bool hasLabel(symbol_table* st, char* label) {
    symbol* sym = st->first;

	while (sym != NULL) {
		if (strcmp(sym->label, label) == 0) {
			return true;
		}
		sym = sym->next;
	}

    return false;
}

// Check if token is a label
bool isLabel(char* token) {
	return strstr(token, ":");
}

// Add symbol to start of symbol table
void addSymbol(symbol_table* st, uint64_t address, char* label) {
    assert(st->size < MAX_LABELS);

	symbol* sym = malloc(sizeof(symbol));
    assert(sym != NULL);
	sym->label = strdup(label);
	sym->address = address;
	sym->next = st->first;
	st->first = sym;
	st->size++;
}

// Get address in symbol table from label, returns -1 otherwise. Note return int is signed.
int32_t getAddress(symbol_table* st, char* label) {
	symbol* sym = st->first;

	while (sym != NULL) {
		if (strncmp(sym->label, label, strlen(sym->label)) == 0) {
			return sym->address;
		}
		sym = sym->next;
	}

	fprintf(stderr, "Label (%s) not in symbol table.\n", label);
    exit(EXIT_FAILURE);
}

// Takes read files and adds any labels and corresponding addresses to symbol table.
uint8_t populateSymbolTable(char buffer[MAX_LINES][MAX_CHARS_IN_LINE], symbol_table* st, uint8_t numRead) {
	int numInstr = 0;

	for (int i = 0; i < numRead; i++) {
		if (isLabel(buffer[i])) {
			char* label = strdup(buffer[i]);
			// Remove colon since when labels are called don't have colon.
			removeWhitespace(label);
			label[strlen(label) - 2] = '\0';
			addSymbol(st, numInstr * INSTRUCTION_SIZE, label);
		} else {
			// Labels are not instructions (but take up an extra space in assembly code);
			if (!isBlankLine(buffer[i])){
				numInstr++;
			}
		}
	}

	return numInstr;
}
