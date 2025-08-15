#include "module.h"

Keyword::Keyword(const Token &token, int &ptr) : token_(token) {
  ++ptr;
}

Identifier::Identifier(const Token &token, int &ptr): token_(token) {
  const std::string &t = token.GetStr();
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
    std::cerr << "Cannot use keyword as identifier.\n";
    throw "";
  }
  ++ptr;
}

Punctuation::Punctuation(const Token &token, int &ptr) : token_(token) {
  ++ptr;
}

