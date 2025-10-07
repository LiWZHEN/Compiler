#include "trait.h"
#include "scope.h"

AssociatedItem::AssociatedItem(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    const std::string next_token = tokens_[ptr_].GetStr();
    if (next_token == "fn") {
      AddChild(type_function);
    } else if (next_token == "const") {
      if (ptr_ + 1 >= tokens_.size()) {
        ThrowErr(type_associated_item, "Expect ConstantItem or Function.");
      }
      if (tokens_[ptr_ + 1].GetStr() == "fn") {
        AddChild(type_function);
      } else {
        AddChild(type_constant_item);
      }
    } else {
      ThrowErr(type_associated_item, "Expect ConstantItem or Function.");
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

std::string AssociatedItem::GetNodeLabel() const {
  return "AssociatedItem";
}

void AssociatedItem::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void AssociatedItem::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  for (int i = 0; i < children_.size(); ++i) {
    children_[i]->AddSymbol(nullptr, false, false, true,
        false, target_node, {children_[i], type_[i]});
  }
}