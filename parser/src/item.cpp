#include "item.h"
#include "leaf_node.h"
#include "scope.h"

Function::Function(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // const?
    if (tokens_[ptr_].GetStr() == "const") {
      AddChild(type_keyword);
    }
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_function, "");
    }
    // fn
    if (tokens_[ptr_].GetStr() != "fn") {
      ThrowErr(type_function, "Expect \"fn\".");
    }
    AddChild(type_keyword);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_function, "");
    }
    // Identifier
    if (tokens_[ptr_].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_function, "Expect Identifier.");
    }
    AddChild(type_identifier);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_function, "");
    }
    // (
    if (tokens_[ptr_].GetStr() != "(") {
      ThrowErr(type_function, "Expect \"(\".");
    }
    AddChild(type_punctuation);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_function, "");
    }
    // FunctionParameters?
    if (tokens_[ptr_].GetStr() != ")") {
      AddChild(type_function_parameters);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_function, "");
      }
    }
    // )
    if (tokens_[ptr_].GetStr() != ")") {
      ThrowErr(type_function, "Expect \")\".");
    }
    AddChild(type_punctuation);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_function, "");
    }
    // FunctionReturnType?
    if (tokens_[ptr_].GetStr() == "->") {
      AddChild(type_function_return_type);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_function, "");
      }
    }
    // ;|BlockExpression
    if (tokens_[ptr_].GetStr() == ";") {
      AddChild(type_punctuation);
    } else if (tokens_[ptr_].GetStr() == "{") {
      AddChild(type_block_expression);
    } else {
      ThrowErr(type_function, "Expect \";\" or BlockExpression.");
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

Struct::Struct(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // struct
    if (tokens_[ptr_].GetStr() != "struct") {
      ThrowErr(type_struct, "Expect \"struct\".");
    }
    AddChild(type_keyword);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_struct, "");
    }
    // Identifier
    if (tokens_[ptr_].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_struct, "Expect Identifier.");
    }
    AddChild(type_identifier);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_struct, "");
    }
    if (tokens_[ptr_].GetStr() == ";") {
      // ;
      AddChild(type_punctuation);
    } else if (tokens_[ptr_].GetStr() == "{") {
      // {StructFields?}
      // {
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_struct, "");
      }
      if (tokens_[ptr_].GetStr() != "}") {
        // StructFields
        AddChild(type_struct_fields);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_struct, "");
        }
      }
      // }
      if (tokens_[ptr_].GetStr() != "}") {
        ThrowErr(type_struct, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else {
      ThrowErr(type_struct, R"(Expect ";" or "{StructFields?}".)");
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

Enumeration::Enumeration(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // enum
    if (tokens_[ptr_].GetStr() != "enum") {
      ThrowErr(type_enumeration, "Expect \"enum\".");
    }
    AddChild(type_keyword);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_enumeration, "");
    }
    // Identifier
    if (tokens_[ptr_].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_enumeration, "Expect Identifier.");
    }
    AddChild(type_identifier);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_enumeration, "");
    }
    // {
    if (tokens_[ptr_].GetStr() != "{") {
      ThrowErr(type_enumeration, "Expect \"{\".");
    }
    AddChild(type_punctuation);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_enumeration, "");
    }
    // EnumVariants?
    if (tokens_[ptr_].GetStr() != "}") {
      // EnumVariants
      AddChild(type_enum_variants);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_enumeration, "");
      }
    }
    // }
    if (tokens_[ptr_].GetStr() != "}") {
      ThrowErr(type_enumeration, "Expect \"}\".");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

ConstantItem::ConstantItem(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // const
    if (tokens_[ptr_].GetStr() != "const") {
      ThrowErr(type_constant_item, "Expect \"const\".");
    }
    AddChild(type_keyword);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_constant_item, "");
    }
    // Identifier
    if (tokens_[ptr_].GetType() == IDENTIFIER_OR_KEYWORD) {
      AddChild(type_identifier);
    } else {
      ThrowErr(type_constant_item, "Expect Identifier.");
    }
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_constant_item, "");
    }
    // :
    if (tokens_[ptr_].GetStr() != ":") {
      ThrowErr(type_constant_item, "Expect \":\".");
    }
    AddChild(type_punctuation);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_constant_item, "");
    }
    // Type
    AddChild(type_type);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_constant_item, "");
    }
    // (= Expression)?
    if (tokens_[ptr_].GetStr() == "=") {
      // =
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_constant_item, "");
      }
      // Expression
      AddChild(type_expression);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_constant_item, "");
      }
    }
    // ;
    if (tokens_[ptr_].GetStr() != ";") {
      ThrowErr(type_constant_item, "Expect \";\".");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

Trait::Trait(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // trait
    if (tokens_[ptr_].GetStr() != "trait") {
      ThrowErr(type_trait, "Expect \"trait\".");
    }
    AddChild(type_keyword);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_trait, "");
    }
    // Identifier
    if (tokens_[ptr_].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_trait, "Expect Identifier.");
    }
    AddChild(type_identifier);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_trait, "");
    }
    // {
    if (tokens_[ptr_].GetStr() != "{") {
      ThrowErr(type_trait, "Expect \"{\".");
    }
    AddChild(type_punctuation);
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_trait, "");
    }
    // AssociatedItem*
    while (tokens_[ptr_].GetStr() != "}") {
      // AssociatedItem
      AddChild(type_associated_item);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_trait, "");
      }
    }
    // }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

Implementation::Implementation(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // impl
    if (tokens_[ptr_].GetStr() != "impl") {
      ThrowErr(type_implementation, "Expect \"impl\".");
    }
    AddChild(type_keyword);
    // (Identifier for)?
    if (ptr_ + 1 >= tokens_.size()) {
      ThrowErr(type_implementation, "Incomplete implementation type.");
    }
    if (tokens_[ptr_ + 1].GetStr() == "for") {
      // Identifier
      if (tokens_[ptr_].GetType() != IDENTIFIER_OR_KEYWORD) {
        ThrowErr(type_implementation, "Expect Identifier.");
      }
      AddChild(type_identifier);
      // for
      AddChild(type_keyword);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_implementation, "");
      }
    }
    AddChild(type_type);
    // {
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_implementation, "");
    }
    if (tokens_[ptr_].GetStr() != "{") {
      ThrowErr(type_implementation, "Expect \"{\".");
    }
    AddChild(type_punctuation);
    // AssociatedItem*
    while (ptr_ < tokens_.size() && tokens_[ptr_].GetStr() != "}") {
      AddChild(type_associated_item);
    }
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_implementation, "");
    }
    // }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

std::string Function::GetNodeLabel() const {
  return "Function";
}

std::string Struct::GetNodeLabel() const {
  return "Struct";
}

std::string Enumeration::GetNodeLabel() const {
  return "Enumeration";
}

std::string ConstantItem::GetNodeLabel() const {
  return "ConstantItem";
}

std::string Trait::GetNodeLabel() const {
  return "Trait";
}

std::string Implementation::GetNodeLabel() const {
  return "Implementation";
}

void Function::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Struct::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Enumeration::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void ConstantItem::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Trait::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Implementation::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

std::string Function::GetIdentifier() const {
  for (int i = 0; i < children_.size(); ++i) {
    if (type_[i] == type_identifier) {
      return reinterpret_cast<Identifier *>(children_[i])->GetContent().GetStr();
    }
  }
  return "";
}

std::string Struct::GetIdentifier() const {
  return reinterpret_cast<Identifier *>(children_[1])->GetContent().GetStr();
}

std::string Enumeration::GetIdentifier() const {
  return reinterpret_cast<Identifier *>(children_[1])->GetContent().GetStr();
}

std::string ConstantItem::GetIdentifier() const {
  return reinterpret_cast<Identifier *>(children_[1])->GetContent().GetStr();
}

std::string Trait::GetIdentifier() const {
  return reinterpret_cast<Identifier *>(children_[1])->GetContent().GetStr();
}

bool Function::IsConst() const {
  if (reinterpret_cast<LeafNode *>(children_[0])->GetContent().GetStr() == "fn") {
    return false;
  }
  return true;
}