#include "expression.h"

LiteralExpression::LiteralExpression(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr), token_(tokens[ptr]) {
  const std::string next_token = tokens[ptr].GetStr();
  const type next_type = tokens[ptr].GetType();
  if (next_type == CHAR_LITERAL || next_type == STRING_LITERAL || next_type == RAW_STRING_LITERAL || next_type == C_STRING_LITERAL
      || next_type == RAW_C_STRING_LITERAL || next_type == INTEGER_LITERAL || next_token == "true" || next_token == "false") {

  }
}
