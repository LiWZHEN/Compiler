#include "expression.h"

LiteralExpression::LiteralExpression(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {
  const std::string next_token = token_.GetStr();
  const type next_type = token_.GetType();
  if (next_type != CHAR_LITERAL && next_type != STRING_LITERAL && next_type != RAW_STRING_LITERAL && next_type != C_STRING_LITERAL
      && next_type != RAW_C_STRING_LITERAL && next_type != INTEGER_LITERAL && next_token != "true" && next_token != "false") {
    --ptr;
    ThrowErr(type_literal_expression, "Invalid literal expression.");
  }
}
