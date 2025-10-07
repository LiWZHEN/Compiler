#include "function_parameters.h"
#include "scope.h"

SelfParam::SelfParam(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // ShorthandSelf
    AddChild(type_shorthand_self);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

ShorthandSelf::ShorthandSelf(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // &?
    if (tokens[ptr].GetStr() == "&") {
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_shorthand_self, "");
      }
    }
    // mut?
    if (tokens[ptr].GetStr() == "mut") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_shorthand_self, "");
      }
    }
    // self
    if (tokens[ptr].GetStr() != "self") {
      ThrowErr(type_shorthand_self, "Expect \"self\".");
    }
    AddChild(type_keyword);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

FunctionParam::FunctionParam(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // PatternNoTopAlt
    AddChild(type_pattern);
    if (ptr >= tokens.size()) {
      ThrowErr(type_function_param, "");
    }
    // :
    if (tokens[ptr].GetStr() != ":") {
      ThrowErr(type_function_param, "Expect \":\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_function_param, "");
    }
    AddChild(type_type);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

std::string SelfParam::GetNodeLabel() const {
  return "SelfParam";
}

std::string ShorthandSelf::GetNodeLabel() const {
  return "ShorthandSelf";
}

std::string FunctionParam::GetNodeLabel() const {
  return "FunctionParam";
}

void SelfParam::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void ShorthandSelf::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void FunctionParam::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void SelfParam::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  if (need_type_add) {
    target_scope->TypeAdd("self", node_info);
  }
  if (need_value_add) {
    target_scope->ValueAdd("self", node_info);
  }
}
void ShorthandSelf::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {}
void FunctionParam::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  for (int i = 0; i < children_.size(); ++i) {
    if (type_[i] != type_pattern) {
      continue;
    }
    children_[i]->AddSymbol(target_scope, need_type_add, need_value_add, associated_item_add,
        field_item_add, target_node, node_info);
  }
}