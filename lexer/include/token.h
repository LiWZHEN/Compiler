#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

enum type {DEFAULT, IDENTIFIER_OR_KEYWORD, CHAR_LITERAL, STRING_LITERAL, RAW_STRING_LITERAL,
    BYTE_LITERAL, BYTE_STRING_LITERAL, RAW_BYTE_STRING_LITERAL, C_STRING_LITERAL,
    RAW_C_STRING_LITERAL, INTEGER_LITERAL, FLOAT_LITERAL, PUNCTUATION, RESERVED_TOKEN};

class Token {
  type type_;
  std::string str_;
  int row_, column_;
public:
  Token() = default;
  Token(type type, std::string str, int row, int column);
  friend std::ostream &operator<<(std::ostream &output, const Token &token);
};

#endif