#include "structs.h"
#include "scope.h"

StructFields::StructFields(const std::vector<Token> &tokens, int &ptr): Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // StructField
    AddChild(type_struct_field);
    while (ptr_ < tokens_.size() && tokens_[ptr_].GetStr() == ",") {
      // ,
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size()) {
        break;
      }
      const int size_before_trying_struct_field = static_cast<int>(children_.size()),
          ptr_before_trying_struct_field = ptr_;
      // StructField
      try {
        AddChild(type_struct_field);
      } catch (...) {
        Restore(size_before_trying_struct_field, ptr_before_trying_struct_field);
        // std::cerr << "StructFields: Successfully handle the struct field trying failure.\n";
        break;
      }
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

StructField::StructField(const std::vector<Token> &tokens, int &ptr): Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // Identifier
    if (tokens_[ptr_].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_struct_field, "Expect Identifier.");
    }
    AddChild(type_identifier);
    // :
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_struct_field, "");
    }
    if (tokens_[ptr_].GetStr() != ":") {
      ThrowErr(type_struct_field, "Expect \":\".");
    }
    AddChild(type_punctuation);
    // Type
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_struct_field, "");
    }
    AddChild(type_type);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

std::string StructFields::GetNodeLabel() const {
  return "StructFields";
}

std::string StructField::GetNodeLabel() const {
  return "StructField";
}

void StructFields::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void StructField::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void StructFields::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  for (int i = 0; i < children_.size(); ++i) {
    if (type_[i] != type_struct_field) {
      continue;
    }
    children_[i]->AddSymbol(nullptr, false, false, false,
        true, target_node, {children_[i], type_struct_field});
  }
}
void StructField::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  children_[0]->AddSymbol(target_scope, need_type_add, need_value_add, associated_item_add,
      field_item_add, target_node, node_info);
}