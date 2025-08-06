#include "token.h"
#include <iomanip>

Token::Token(type type, std::string str, int row, int column) {
  type_ = type;
  str_ = str;
  row_ = row;
  column_ = column;
}

std::ostream &operator<<(std::ostream &output, const Token &token) {
  std::string type_str;
  switch (token.type_) {
  case IDENTIFIER_OR_KEYWORD:
    type_str = "identifier or keyword";
    break;
  case CHAR_LITERAL:
    type_str = "char literal";
    break;
  case STRING_LITERAL:
    type_str = "string literal";
    break;
  case RAW_STRING_LITERAL:
    type_str = "raw string literal";
    break;
  case BYTE_LITERAL:
    type_str = "byte literal";
    break;
  case BYTE_STRING_LITERAL:
    type_str = "byte string literal";
    break;
  case RAW_BYTE_STRING_LITERAL:
    type_str = "raw byte string literal";
    break;
  case C_STRING_LITERAL:
    type_str = "c string literal";
    break;
  case RAW_C_STRING_LITERAL:
    type_str = "raw c string literal";
    break;
  case INTEGER_LITERAL:
    type_str = "integer literal";
    break;
  case FLOAT_LITERAL:
    type_str = "float literal";
    break;
  case PUNCTUATION:
    type_str = "punctuation";
    break;
  case RESERVED_TOKEN:
    type_str = "reserved token";
    break;
  default:
    type_str = "default";
    break;
  }
  output << std::setw(25) << type_str << ": " << token.str_ << " at ln " << token.row_ << ", col " << token.column_;
}