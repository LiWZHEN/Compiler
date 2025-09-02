#include "enumerations.h"

EnumVariants::EnumVariants(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // Identifier
    if (tokens_[ptr_].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_enum_variants, "Expect EnumVariant.");
    }
    AddChild(type_identifier);
    while (ptr_ < tokens_.size() && tokens_[ptr_].GetStr() == ",") {
      // ,
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size() || tokens_[ptr_].GetType() != IDENTIFIER_OR_KEYWORD) {
        break;
      }
      const int size_before_trying_identifier = static_cast<int>(children_.size()),
          ptr_before_trying_identifier = ptr_;
      try {
        // Identifier
        AddChild(type_identifier);
      } catch (...) {
        Restore(size_before_trying_identifier, ptr_before_trying_identifier);
        std::cerr << "EnumVariants: Successfully handle identifier try failure.\n";
        break;
      }
    }
  } catch (...) {
    Restore(0, ptr_before_try);
  }
}

std::string EnumVariants::GetNodeLabel() const {
  return "EnumVariants";
}

void EnumVariants::Accept(Visitor *visitor) {
  visitor->Visit(this);
}
