#include "type.h"

Type::Type(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    const std::string next_token = tokens_[ptr_].GetStr();
    if (next_token == "&") {
      AddChild(type_reference_type);
    } else if (next_token == "[") {
      AddChild(type_array_type);
    } else {
      AddChild(type_type_path);
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

TypePath::TypePath(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {
  const std::string t = token_.GetStr();
  if (t == "Self" || t == "self") {
    return;
  }
  if (token_.GetType() != IDENTIFIER_OR_KEYWORD) {
    --ptr_;
    ThrowErr(type_type_path, "Expect identifier or keyword.");
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
    --ptr_;
    ThrowErr(type_type_path, "Expect identifier.");
  }
}

ReferenceType::ReferenceType(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    if (tokens_[ptr_].GetStr() != "&") {
      ThrowErr(type_reference_type, "Expect \"&\".");
    }
    AddChild(type_punctuation);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_reference_type, "");
    }
    if (tokens_[ptr_].GetStr() == "mut") {
      AddChild(type_keyword);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_reference_type, "");
      }
    }
    AddChild(type_type);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

ArrayType::ArrayType(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    if (tokens_[ptr_].GetStr() != "[") {
      ThrowErr(type_array_type, "Expect \"[\".");
    }
    AddChild(type_punctuation);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_array_type, "");
    }
    AddChild(type_type);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_array_type, "");
    }
    if (tokens_[ptr_].GetStr() != ";") {
      ThrowErr(type_array_type, "Expect \";\".");
    }
    AddChild(type_punctuation);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_array_type, "");
    }
    AddChild(type_expression);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_array_type, "");
    }
    if (tokens_[ptr_].GetStr() != "]") {
      ThrowErr(type_array_type, "Expect \"]\".");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

UnitType::UnitType(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // (
    if (tokens_[ptr_].GetStr() != "(") {
      ThrowErr(type_unit_type, "Expect \"(\".");
    }
    AddChild(type_punctuation);
    // )
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_unit_type, "");
    }
    if (tokens_[ptr_].GetStr() != ")") {
      ThrowErr(type_unit_type, "Expect \")\".");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}
