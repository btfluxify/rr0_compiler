#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "sema.h"    
#include "codegen.h"
#include "helper.h"
#include "symbol.h"

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("Usage: %s <filename> [flag]\n", argv[0]);
    return EXIT_FAILURE;
  }

  char* fileContents = readFile(argv[1]); 
  if(fileContents == NULL){
    printf("Compiler Error: Error reading file %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  Parser parser;

  parser.lexer = malloc(sizeof(Lexer));

  parser.lexer->source = fileContents;
  parser.lexer->position = 0;
  
  parser.currentScope = createScope(NULL);
  parser.currentStackOffset = 0;

  parser.currentToken = nextToken(parser.lexer);

  ASTNode* rootAST = parseProgram(&parser);

  free(fileContents);

  if(rootAST == NULL){
    printf("Compilation Aborted: Syntax compilation failed\n");
    return EXIT_FAILURE;
  }

  if(!analyzeAST(rootAST)){
    printf("Compilation Aborted: Semantic verification checks failed\n");
    return EXIT_FAILURE;
  }

  generateCode(rootAST);

  freeASTNode(rootAST);

  int result = system("gcc output.s -o rr0.out");

  if(result == 0 && argc >= 3 && strcmp(argv[2], "--perserve-output") == 0){
    //remove("output.s");
    printf("Perserved output.s\n");
  } else if(result == 0 && argc < 3){
    remove("output.s");
  } else{
    printf("Error: Failed to build the binary\n");
    remove("output.s");
    return EXIT_FAILURE;
  }

  free(parser.lexer);
  return EXIT_SUCCESS;
}
