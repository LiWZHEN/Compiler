#include "leaf_node.h"
#include "scope.h"
#include "item.h"

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

void Keyword::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Identifier::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Punctuation::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void CharLiteral::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void StringLiteral::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void RawStringLiteral::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void CStringLiteral::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void RawCStringLiteral::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void IntegerLiteral::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Keyword::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {}
void Identifier::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  const std::string identifier_name = this->GetContent().GetStr();
  if (target_node.node_type == type_enumeration) {
    const auto enum_ptr = dynamic_cast<Enumeration *>(target_node.node);
    if (enum_ptr->enum_variants_.contains(identifier_name)) {
      // std::cerr << "Error: The name '" << identifier_name << "' is defined multiple times.\n";
      throw "";
    }
    const int new_element_index = static_cast<int>(enum_ptr->enum_variants_.size());
    enum_ptr->enum_variants_[identifier_name] = new_element_index;
  } else if (target_node.node_type == type_struct) {
    const auto struct_ptr = dynamic_cast<Struct *>(target_node.node);
    if (struct_ptr->field_items_.contains(identifier_name)) {
      // std::cerr << "Error: Field '" << identifier_name << "' is already declared.\n";
      throw "";
    }
    struct_ptr->field_items_[identifier_name] = node_info;
  } else {
    if (need_type_add) {
      target_scope->TypeAdd(identifier_name, node_info);
    }
    if (need_value_add) {
      target_scope->ValueAdd(identifier_name, node_info);
    }
  }
}
void Punctuation::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
   const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
   const ScopeNodeContent node_info) {}
void CharLiteral::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
   const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
   const ScopeNodeContent node_info) {}
void StringLiteral::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
   const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
   const ScopeNodeContent node_info) {}
void RawStringLiteral::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
   const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
   const ScopeNodeContent node_info) {}
void CStringLiteral::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
   const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
   const ScopeNodeContent node_info) {}
void RawCStringLiteral::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
   const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
   const ScopeNodeContent node_info) {}
void IntegerLiteral::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
   const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
   const ScopeNodeContent node_info) {}