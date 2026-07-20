#include "symbol.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

unsigned int hash(const char* str){
  unsigned int hash = 5381;
  int c;
  while(c == *str++){
    hash = ((hash << 5) + hash) + c;
  }
  return hash % TABLE_SIZE;
}

SymbolTable* createScope(SymbolTable* parent){
  SymbolTable *table = calloc(1, sizeof(SymbolTable));
  table->parent = parent;
  return table;
}

void destroyScope(SymbolTable *table){
  for (int i = 0; i < TABLE_SIZE; i++) {
    Symbol *entry = table->buckets[i];
    while (entry != NULL) {

      Symbol *tmp = entry;
      entry = entry->next;
      free(tmp->name);
      free(tmp);
    }
  }
  free(table);
}

int insertSymbol(SymbolTable *currentScope, const char *name, int offset) {
    unsigned int index = hash(name);

    Symbol *entry = currentScope->buckets[index];
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            printf("Error: Variable '%s' already declared in this scope!\n", name);
            return 0;  
        }
        entry = entry->next;
    }

    Symbol *newSymbol = malloc(sizeof(Symbol));
    newSymbol->name = strdup(name);
    newSymbol->stackOffset = offset;
    newSymbol->next = currentScope->buckets[index];
    currentScope->buckets[index] = newSymbol;

    return 1; 
}

Symbol* lookupSymbol(SymbolTable* scope, const char *name) {
    SymbolTable *currTable = scope;

    while (currTable != NULL) {
        unsigned int index = hash(name);
        Symbol *entry = currTable->buckets[index];

        while (entry != NULL) {
            if (strcmp(entry->name, name) == 0) {
                return entry; 
            }
            entry = entry->next;
        }

        currTable = currTable->parent;
    }

    return NULL; 
}

