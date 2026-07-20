#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "symbol.h"

typedef enum {
  node_int_lit,
  node_binary_expr,
  node_identifier,
  node_unary,
  node_int_decla,
  node_comment,
  node_return_stmt
} NodeType;

typedef enum {
  op_negative,
  op_positive
} UnaryOp;

typedef struct ASTNode{ //Abstract Syntax Tree
  NodeType type;
  struct ASTNode* next;
  union{
    struct {
      int value;
    } int_lit;

    struct {
      char* name;
      struct ASTNode* value;
      int stackOffset;
    } variable_dcla;

    struct {
      char* name;
      int stackOffset;
    } identifier;

    struct {
      char* comment;
    } comment;

    struct {
      UnaryOp op;
      struct ASTNode* operand;
    } unary;

    struct {
      char op;
      struct ASTNode *right;
      struct ASTNode *left;
    } binary_expr;

    struct {
      struct ASTNode *expression;
    } return_stmt;

  } data;
} ASTNode;

typedef struct Parser{
  Lexer *lexer;
  Token currentToken;
  SymbolTable *currentScope;
  int currentStackOffset;
} Parser;

void advanceParser(Parser *parser);
int getOperatorPrecedence(TokenType type);
ASTNode* parseProgram(Parser *parser);
ASTNode* createASTNode(NodeType type);
void freeASTNode(ASTNode* node);
ASTNode* parsePrimary(Parser *parser);
ASTNode* parseUnary(Parser *parser);
ASTNode* parseDeclaration(Parser *parser);
ASTNode* parseIdentifier(Parser *parser);
ASTNode* parseComment(Parser *parser);

#endif // !PARSER_H
