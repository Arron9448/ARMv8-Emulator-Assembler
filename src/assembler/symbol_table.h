#include <stdint.h>
#include <stdbool.h>
#include "defs.h"

// Creates new symbol table.
symbol_table* newSymbolTable();

// Checks whether a given label is already in the table.
bool hasLabel(symbol_table* st, char* label);

// Returns the address associated with a label or -1 if it is not present. (Note this int is signed)
int32_t getAddress(symbol_table* table, char* label);

// Add symbol to start of symbol table
void addSymbol(symbol_table* st, uint64_t address, char* label) ;

// Check if token is a label
bool isLabel(char* token);
