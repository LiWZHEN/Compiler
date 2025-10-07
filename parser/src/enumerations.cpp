#include "enumerations.h"
#include "scope.h"

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

void EnumVariants::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  for (int i = 0; i < children_.size(); ++i) {
    if (type_[i] == type_punctuation) {
      continue;
    }
    children_[i]->AddSymbol(target_scope, need_type_add, need_value_add, associated_item_add,
        true, target_node, {children_[i], type_identifier});
  }
}