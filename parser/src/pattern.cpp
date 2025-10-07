#include "pattern.h"
#include "scope.h"

Pattern::Pattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  const std::string next_token = tokens[ptr].GetStr();
  try {
    if (next_token == "&" || next_token == "&&") {
      AddChild(type_reference_pattern);
    } else {
      AddChild(type_identifier_pattern);
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

IdentifierPattern::IdentifierPattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // ref?
    if (tokens[ptr].GetStr() == "ref") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_identifier_pattern, "");
      }
    }
    // mut?
    if (tokens[ptr].GetStr() == "mut") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_identifier_pattern, "");
      }
    }
    // Identifier
    AddChild(type_identifier);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

ReferencePattern::ReferencePattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // &|&&
    if (tokens[ptr].GetStr() != "&" && tokens[ptr].GetStr() != "&&") {
      ThrowErr(type_reference_pattern, R"(Expect "&" or "&&".)");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_reference_pattern, "");
    }
    // mut?
    if (tokens[ptr].GetStr() == "mut") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_reference_pattern, "");
      }
    }
    // PatternWithoutRange
    AddChild(type_pattern);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

std::string Pattern::GetNodeLabel() const {
  return "Pattern";
}

std::string IdentifierPattern::GetNodeLabel() const {
  return "IdentifierPattern";
}

std::string ReferencePattern::GetNodeLabel() const {
  return "ReferencePattern";
}

void Pattern::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void IdentifierPattern::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void ReferencePattern::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Pattern::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  for (const auto it : children_) {
    it->AddSymbol(target_scope, need_type_add, need_value_add, associated_item_add,
        field_item_add, target_node, node_info);
  }
}
void IdentifierPattern::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  for (const auto it : children_) {
    it->AddSymbol(target_scope, need_type_add, need_value_add, associated_item_add,
        field_item_add, target_node, node_info);
  }
}
void ReferencePattern::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  for (const auto it : children_) {
    it->AddSymbol(target_scope, need_type_add, need_value_add, associated_item_add,
        field_item_add, target_node, node_info);
  }
}