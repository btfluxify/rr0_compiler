#include <stdio.h>
#include "sema.h"
#include "parser.h"

bool analyzeAST(ASTNode* node){
  if(node == NULL) return true;

  if(node->type == node_return_stmt){
    return analyzeAST(node->data.return_stmt.expression);
  }

  if(node->type == node_binary_expr){
    if(!analyzeAST(node->data.binary_expr.left)) return false;
    if(!analyzeAST(node->data.binary_expr.right)) return false;

    if(node->data.binary_expr.op == '/'){
      ASTNode* rightNode = node->data.binary_expr.right;

      if(rightNode->type == node_int_lit && rightNode->data.int_lit.value == 0){
        printf("Semantic Compiler Error: Divsion by zero detected!\n");
        return false;
      }
    }
    return true;
  }

  if(node->type == node_int_lit){
    return true;
  }

  return true;
}

