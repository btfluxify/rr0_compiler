#ifndef SYMBOL_H
#define SYMBOL_H

#include "lexer.h"

#define TABLE_SIZE 31

typedef struct Symbol {
  char* name;
  TokenType type;
  int stackOffset;
  struct Symbol *next;
} Symbol;

typedef struct SymbolTable{
  Symbol *buckets[TABLE_SIZE];
  struct SymbolTable *parent;
} SymbolTable;

unsigned int hash(const char* str);
SymbolTable* createScope(SymbolTable *parent);
void destroyScope(SymbolTable *table);
int insertSymbol(SymbolTable *currentScope, const char *name, int offset);
Symbol* lookupSymbol(SymbolTable* scope, const char *name);

#endif // !SYMBOL_H
