#include "leaf_node.h"

Keyword::Keyword(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  ++ptr;
}

Identifier::Identifier(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const std::string &t = tokens[ptr].GetStr();
  if (t == "as" || t == "break" || t == "const" || t == "continue" || t == "crate"
      || t == "dyn" || t == "else" || t == "enum" || t == "false" || t == "fn"
      || t == "for" || t == "if" || t == "impl" || t == "in" || t == "let"
      || t == "loop" || t == "match" || t == "mod" || t == "move" || t == "mut"
      || t == "ref" || t == "return" || t == "self" || t == "Self" || t == "static"
      || t == "struct" || t == "super" || t == "trait" || t == "true" || t == "type"
      || t == "unsafe" || t == "use" || t == "where" || t == "while" || t == "abstract"
      || t == "become" || t == "box" || t == "do" || t == "final" || t == "macro"
      || t == "override" || t == "priv" || t == "typeof" || t == "unsized"
      || t == "virtual" || t == "yield" || t == "try" || t == "gen") {
    ThrowErr(type_identifier, "Expect identifier.");
  }
  ++ptr;
}

Punctuation::Punctuation(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  ++ptr;
}

