#include <stdio.h>
#include "codegen.h"
#include "parser.h"

void codegenNode(ASTNode* node, FILE* out){
  if(node == NULL) return;

  if(node->type == node_int_lit){
    fprintf(out, "    mov eax, %d\n", node->data.int_lit.value);
    return;
  }

  else if(node->type == node_unary){
    codegenNode(node->data.unary.operand, out);
    if(node->data.unary.op == op_negative){
      fprintf(out, "    neg eax\n");
    }
    if(node->data.unary.op == op_positive){
      //we do not care
    }
  }

  else if(node->type == node_binary_expr){
    codegenNode(node->data.binary_expr.right, out);
    fprintf(out, "    push rax\n");

    codegenNode(node->data.binary_expr.left, out);
    fprintf(out, "    pop rbx\n");

    if(node->data.binary_expr.op == '+'){
      fprintf(out, "    add eax, ebx\n");
    } else if(node->data.binary_expr.op == '*'){
      fprintf(out, "    imul eax, ebx\n");
    } else if(node->data.binary_expr.op == '-'){
      fprintf(out, "    sub eax, ebx\n");
    } else if(node->data.binary_expr.op == '/'){
      fprintf(out, "    cdq\n"); //not cqo
      fprintf(out, "    idiv ebx\n");
    }
  }

  else if(node->type == node_int_decla){
    codegenNode(node->data.variable_dcla.value, out);
    fprintf(out, "    mov DWORD PTR [rbp - %d], eax\n", node->data.variable_dcla.stackOffset);
  }

  else if(node->type == node_identifier){
    fprintf(out, "    mov eax, DWORD PTR [rbp - %d]\n", node->data.identifier.stackOffset);
  }

  else if(node->type == node_return_stmt){
    codegenNode(node->data.return_stmt.expression, out);

    fprintf(out, "    leave\n");
    fprintf(out, "    ret\n");
  }

}

void generateCode(ASTNode* node) {
    FILE* out = fopen("output.s", "w");
    if (!out) {
        perror("Failed to create output assembly file");
        return;
    }
    fprintf(out, "    .intel_syntax noprefix\n");

    fprintf(out, "    .global main\n");
    fprintf(out, "main:\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    fprintf(out, "    sub rsp, 24\n");

    ASTNode* current = node;
    while (current != NULL) {       
      codegenNode(current, out);
      current = current->next;
    }

    fclose(out);
    printf("Success! Assembly generated cleanly in 'output.s'\n");
}

