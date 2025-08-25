#include "path.h"

PathInExpression::PathInExpression(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // PathExprSegment
    AddChild(type_path_expr_segment);
    // (:: PathExprSegment)?
    if (ptr < tokens.size() && tokens[ptr].GetStr() == "::") {
      // ::
      AddChild(type_punctuation);
      // PathExprSegment
      if (ptr >= tokens.size()) {
        ThrowErr(type_path_in_expression, "");
      }
      AddChild(type_path_expr_segment);
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

PathExprSegment::PathExprSegment(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {
  const std::string t = token_.GetStr();
  if (t == "Self" || t == "self") {
    return;
  }
  if (token_.GetType() != IDENTIFIER_OR_KEYWORD) {
    --ptr;
    ThrowErr(type_path_expr_segment, "Expect identifier or keyword.");
  }
  if (t == "as" || t == "break" || t == "const" || t == "continue" || t == "crate"
      || t == "dyn" || t == "else" || t == "enum" || t == "false" || t == "fn"
      || t == "for" || t == "if" || t == "impl" || t == "in" || t == "let"
      || t == "loop" || t == "match" || t == "mod" || t == "move" || t == "mut"
      || t == "ref" || t == "return" || t == "static" || t == "struct" || t == "super"
      || t == "trait" || t == "true" || t == "type" || t == "unsafe" || t == "use"
      || t == "where" || t == "while" || t == "abstract" || t == "become" || t == "box"
      || t == "do" || t == "final" || t == "macro" || t == "override" || t == "priv"
      || t == "typeof" || t == "unsized" || t == "virtual" || t == "yield"
      || t == "try" || t == "gen") {
    --ptr;
    ThrowErr(type_path_expr_segment, "Expect identifier.");
  }
}