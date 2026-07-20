#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> //debug

#include "lexer.h"

struct Token makeToken(enum TokenType type, const char* word){
  struct Token token;

  token.type = type;
  token.value = malloc(strlen(word) + 1);
  if(token.value == NULL) { return token; }

  strcpy(token.value, word);
  
  return token;
}

char peekCurr(Lexer *lexer){
  if(lexer->source[lexer->position] == '\0'){
    return '\0';
  }

  return lexer->source[lexer->position + 0];
}

char peekNext(Lexer *lexer){
  if(lexer->source[lexer->position] == '\0'){
    return '\0';
  }

  return lexer->source[lexer->position + 1];
}

char advanceChar(Lexer *lexer){
  char c = lexer->source[lexer->position];
  if(c != '\0'){
    lexer->position++;
  }
  return c;
}

Token nextToken(Lexer *lexer){
  while(peekCurr(lexer) != '\0' && isspace(peekCurr(lexer)) ){
    advanceChar(lexer); //consumes all whitespace <space> <tab> etc..
  }

  char c  = lexer->source[lexer->position];

  if(c == '\0') return makeToken(eof, "EOF"); //consumes \0 null terminator
  if(c == '=') { lexer->position++; return makeToken(assign, "="); }
  if(c == ';') { lexer->position++; return makeToken(semi, ";"); } 
  if(c == '+') { lexer->position++; return makeToken(add, "+"); }
  if(c == '-') { lexer->position++; return makeToken(sub, "-"); }
  if(c == '*') { lexer->position++; return makeToken(mul, "*"); }
  if(c == '/') { lexer->position++; return makeToken(_div, "/"); }
  if(c == '(') { lexer->position++; return makeToken(open, "("); }
  if(c == ')') { lexer->position++; return makeToken(close, ")"); }

  if(isdigit(c)){
    int startPos = lexer->position;

    while(isdigit(peekCurr(lexer))){
      advanceChar(lexer);
    }

    int len = lexer->position - startPos;

    char buf[len + 1];
    strncpy(buf, &lexer->source[startPos], len);
    buf[len] = '\0';

    return makeToken(int_lit, buf);
  }

  if(isalnum(c) || c == '_'){
    int startPos = lexer->position;

    while(isalnum(peekCurr(lexer)) || peekCurr(lexer) == '_'){
      advanceChar(lexer);
    }

    int len = lexer->position - startPos;

    char buf[len + 1];
    strncpy(buf, &lexer->source[startPos], len);
    buf[len] = '\0';

    if(strcmp(buf, "return") == 0) return makeToken(_return, buf);
    
    //signed int with space i<bytes>
    if(strcmp(buf, "i8") == 0) return makeToken(i8_keyword, buf);
    if(strcmp(buf, "i16") == 0) return makeToken(i16_keyword, buf);
    if(strcmp(buf, "i32") == 0) return makeToken(i32_keyword, buf);
    if(strcmp(buf, "i64") == 0) return makeToken(i64_keyword, buf);

    //unsigned int with space u<bytes>
    if(strcmp(buf, "u8") == 0) return makeToken(i64_keyword, buf);
    if(strcmp(buf, "u16") == 0) return makeToken(i64_keyword, buf);
    if(strcmp(buf, "u32") == 0) return makeToken(i64_keyword, buf);
    if(strcmp(buf, "u64") == 0) return makeToken(i64_keyword, buf);

    if(strcmp(buf, "comment") == 0) return makeToken(comment, buf);

    return makeToken(identifier, buf);
  }

  advanceChar(lexer);
  char unCh[2] = {c, '\0'}; //unknown character
  return makeToken(unknown, unCh);
}

