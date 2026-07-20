#ifndef LEXER_H
#define LEXER_H

typedef enum TokenType {
  _return,
  identifier,
  int_lit,
  semi,
  assign,
  add,
  sub,
  mul,
  _div,
  open,
  close,
  i8_keyword,
  i16_keyword,
  i32_keyword,
  i64_keyword,
  u8_keyword,
  u16_keyword,
  u32_keyword,
  u64_keyword,
  comment,
  unknown,
  eof
} TokenType;

typedef struct Token {
  TokenType type;
  char* value;
} Token;

typedef struct Lexer {
  char *source;
  int position;
} Lexer;

Token makeToken(TokenType type, const char* word);
Token nextToken(Lexer *lexer); 

#endif // !LEXER_H
