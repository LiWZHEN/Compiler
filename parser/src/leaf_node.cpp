#include "leaf_node.h"

Keyword::Keyword(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {}

Identifier::Identifier(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {
  const std::string &t = token_.GetStr();
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
    --ptr;
    ThrowErr(type_identifier, "Expect identifier.");
  }
}

Punctuation::Punctuation(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {}

CharLiteral::CharLiteral(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {}

StringLiteral::StringLiteral(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {}

RawStringLiteral::RawStringLiteral(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {}

CStringLiteral::CStringLiteral(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {}

RawCStringLiteral::RawCStringLiteral(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {}

IntegerLiteral::IntegerLiteral(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {}

std::string Keyword::GetNodeLabel() const {
  return "Keyword: " + token_.GetStr();
}

std::string Identifier::GetNodeLabel() const {
  return "Identifier: " + token_.GetStr();
}

std::string Punctuation::GetNodeLabel() const {
  return "Punctuation: " + token_.GetStr();
}

std::string CharLiteral::GetNodeLabel() const {
  return "CharLiteral: " + token_.GetStr();
}

std::string StringLiteral::GetNodeLabel() const {
  return "StringLiteral: " + token_.GetStr();
}

std::string RawStringLiteral::GetNodeLabel() const {
  return "RawStringLiteral: " + token_.GetStr();
}

std::string CStringLiteral::GetNodeLabel() const {
  return "CStringLiteral: " + token_.GetStr();
}

std::string RawCStringLiteral::GetNodeLabel() const {
  return "RawCStringLiteral: " + token_.GetStr();
}

std::string IntegerLiteral::GetNodeLabel() const {
  return "IntegerLiteral: " + token_.GetStr();
}