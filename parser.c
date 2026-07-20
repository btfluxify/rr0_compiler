#include "parser.h"
#include "lexer.h"
#include "symbol.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void advanceParser(Parser *parser){
  parser->currentToken = nextToken(parser->lexer);
}

int getOperatorPrecedence(TokenType type){
  if(type == add || type == sub) return 1;
  if(type == mul || type == _div) return 2;
  return 0;
}

ASTNode* createASTNode(NodeType type){
  ASTNode* node = malloc(sizeof(ASTNode));
  
  if(node == NULL) {
    perror("Parser Error: Failed to allocate AST node");
    exit(EXIT_FAILURE);
  }

  node->type = type;

  if(type == node_binary_expr){
    node->data.binary_expr.left = NULL;
    node->data.binary_expr.right = NULL;
  } else if(type == node_return_stmt){
    node->data.return_stmt.expression = NULL;
  } else if(type == node_unary){
    node->data.unary.operand = NULL;
  }

  node->next = NULL;

  return node;
} 

void freeASTNode(ASTNode* node){
  if(node == NULL) return;

  if(node->type == node_binary_expr){
    freeASTNode(node->data.binary_expr.left);
    freeASTNode(node->data.binary_expr.right);
  } else if(node->type == node_return_stmt){
    freeASTNode(node->data.return_stmt.expression);
  } else if(node->type == node_unary){
    freeASTNode(node->data.unary.operand);
  }

  free(node);
}

ASTNode* parseExpressionClimb(Parser *parser, int minPrecedence){
  ASTNode *left = parseUnary(parser);
  if(left == NULL) return NULL;

  while(1){
    int precedence = getOperatorPrecedence(parser->currentToken.type);

    if(precedence == 0 || precedence < minPrecedence){
      break;
    }

    char op = parser->currentToken.value[0];
    advanceParser(parser);

    ASTNode* right = parseExpressionClimb(parser, precedence + 1);
    if(right == NULL) return NULL;

    ASTNode* binaryNode = createASTNode(node_binary_expr);
    binaryNode->data.binary_expr.op = op;
    binaryNode->data.binary_expr.left = left;
    binaryNode->data.binary_expr.right = right;

    left = binaryNode;

  }

  return left;
}

ASTNode* parseExpression(Parser *parser){
  return parseExpressionClimb(parser, 0);
}

ASTNode* parseUnary(Parser *parser){
  if(parser->currentToken.type == add){
    advanceParser(parser);

    ASTNode* operand = parseUnary(parser);
    ASTNode* node = createASTNode(node_unary);

    node->data.unary.op = op_positive;
    node->data.unary.operand = operand;

    return node;
  }

  if(parser->currentToken.type == sub){
    advanceParser(parser);

    ASTNode* operand = parseUnary(parser);
    ASTNode* node = createASTNode(node_unary);

    node->data.unary.op = op_negative;
    node->data.unary.operand = operand;

    return node;
  }

  return parsePrimary(parser);
}

ASTNode* parsePrimary(Parser *parser){
  if(parser->currentToken.type == int_lit){
    ASTNode* node = createASTNode(node_int_lit);

    node->data.int_lit.value = atoi(parser->currentToken.value);

    advanceParser(parser);

    return node;

  }

  if(parser->currentToken.type == identifier){
    return parseIdentifier(parser);
  } 


  if(parser->currentToken.type == open){
    advanceParser(parser);

    ASTNode* expr = parseExpression(parser);

    if(parser->currentToken.type != close){
      printf("Syntax Error: Expected closing parenthesis ')'\n");
      return NULL;
    }
    advanceParser(parser);

    return expr;
  }

  return NULL;
}

ASTNode* parseDeclaration(Parser *parser){
  int size = parser->currentToken.type;

  if(
     parser->currentToken.type != i8_keyword &&
     parser->currentToken.type != i16_keyword &&
     parser->currentToken.type != i32_keyword &&
     parser->currentToken.type != i64_keyword &&
     parser->currentToken.type != u8_keyword &&
     parser->currentToken.type != u16_keyword &&
     parser->currentToken.type != u32_keyword &&
     parser->currentToken.type != u64_keyword
    )
  {
    printf("Syntax Error: Expected '<signed?><bytes>' keyword!\n");
    return NULL;
  }
  advanceParser(parser); //eat int

  if(parser->currentToken.type != identifier){
    printf("Syntax Error: Expected an identifier after 'int'!\n");
    return NULL;
  }

  char* varName = strdup(parser->currentToken.value);
  if(size == i8_keyword || size == u8_keyword){
    parser->currentStackOffset += 1;
  }
  else if(size == i16_keyword || size == u16_keyword){
    parser->currentStackOffset += 2;
  }
  else if(size == i32_keyword || size == u32_keyword){
    parser->currentStackOffset += 4; //stack stuff 16bytes
  }
  else if(size == i64_keyword || size == u64_keyword){
    parser->currentStackOffset += 8;
  }

  if(!insertSymbol(parser->currentScope, varName, parser->currentStackOffset)){
    printf("Semantic Error: Variable '%s' is already declared in this scope!\n", varName);
    return NULL;
  }

  ASTNode* node = createASTNode(node_int_decla);
  node->data.variable_dcla.name = parser->currentToken.value;
  
  node->data.variable_dcla.stackOffset = parser->currentStackOffset;

  advanceParser(parser); //eat variale

  if(parser->currentToken.type != assign){
    printf("Syntax Error: Expected an '=' after identifier!\n");
    return NULL;
  }
  advanceParser(parser); //eat =

  ASTNode* valueExpr = parseExpression(parser);
  if(valueExpr == NULL){
    printf("Syntax Error: Invalid expression after '='\n!");
    return NULL;
  }
  node->data.variable_dcla.value = valueExpr;

  if(parser->currentToken.type != semi){
    printf("Syntax Error: Expected ';' at end of statement!\n");
    return NULL;
  }
  advanceParser(parser); //eat ;

  return node;
}

ASTNode* parseIdentifier(Parser *parser){
  if (parser->currentToken.type != identifier) {
    printf("Syntax Error: Expected identifier!\n");
    return NULL;
  }

  char *varName = parser->currentToken.value;

  Symbol *sym = lookupSymbol(parser->currentScope, parser->currentToken.value);
  if (sym == NULL) {
    printf("Semantic Error: Variable '%s' used before declaration!\n", varName);
    return NULL;
  }

  ASTNode *node = createASTNode(node_identifier);
  node->data.identifier.name = strdup(varName);
  
  node->data.identifier.stackOffset = sym->stackOffset;

  advanceParser(parser); 
  return node;
}

ASTNode* parseComment(Parser *parser){
  if(parser->currentToken.type != comment){
    printf("Syntax Error: Expected comment!\n");
    return NULL;
  }

  while(parser->currentToken.type != semi && parser->currentToken.type != eof){
    advanceParser(parser);
  }

  if(parser->currentToken.type == eof){
    printf("Syntax Error: Unclosed comment block! Missing ';' at EOF\n");
    return NULL;
  }
  advanceParser(parser);

  ASTNode* commentNode = createASTNode(node_comment);
  commentNode->data.comment.comment = strdup(parser->currentToken.value);

  return commentNode;
}

ASTNode* parseStatement(Parser *parser){
  if(parser->currentToken.type == _return){
    advanceParser(parser);

    ASTNode* returnedExpr = parseExpression(parser);

    if(parser->currentToken.type != semi){
      printf("Syntax Error: Expected ';'!\n");
      return NULL;
    }
    advanceParser(parser);

    ASTNode* returnNode = createASTNode(node_return_stmt);
    returnNode->data.return_stmt.expression = returnedExpr;

    return returnNode;
  }

  if(
     parser->currentToken.type == i8_keyword ||
     parser->currentToken.type == i16_keyword ||
     parser->currentToken.type == i32_keyword ||
     parser->currentToken.type == i64_keyword ||
     parser->currentToken.type == u8_keyword ||
     parser->currentToken.type == u16_keyword ||
     parser->currentToken.type == u32_keyword ||
     parser->currentToken.type == u64_keyword
    ) 
  {
    return parseDeclaration(parser);
  }

  if(parser->currentToken.type == comment){
    return parseComment(parser);
  }

  return NULL;
}

ASTNode* parseProgram(Parser *parser) {
  ASTNode *head = NULL;
  ASTNode *tail = NULL;
  while (parser->currentToken.type != eof) {
    ASTNode *stmt = parseStatement(parser);
    
    if (stmt == NULL) { 
      printf("Debug Error: parseStatement failed on token '%s' (type %d)\n", parser->currentToken.value, parser->currentToken.type);
      return NULL; 
    }

    stmt->next = NULL; 

    if (head == NULL) {
      head = stmt; 
      tail = stmt;
    } else {
      tail->next = stmt; 
      tail = stmt;       
    }
  }

  return head; 
}


