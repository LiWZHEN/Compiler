#include "module.h"

Keyword::Keyword(const Token &token, int &ptr) : token_(token) {
  ++ptr;
}

Identifier::Identifier(const Token &token, int &ptr): token_(token) {
  const std::string &t = token.GetStr();
  if (t == "as" || t == "break" || t == "const" || t == "continue" || t == "crate"
      || t == "else" || t == "enum" || t == "extern" || t == "false" || t == "fn"
      || t == "for" || t == "if" || t == "impl" || t == "in" || t == "let" || t == "loop"
      || t == "match" || t == "mod" || t == "move" || t == "mut" || t == "pub"
      || t == "ref" || t == "return" || t == "self" || t == "Self" || t == "static"
      || t == "struct" || t == "super" || t == "trait" || t == "true" || t == "type"
      || t == "unsafe" || t == "use" || t == "where" || t == "while") {
    std::cerr << "Cannot use keyword as identifier.\n";
    throw "";
  }
  ++ptr;
}

Punctuation::Punctuation(const Token &token, int &ptr) : token_(token) {
  ++ptr;
}

