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
        std::cerr << "StructFields: Successfully handle the struct field trying failure.\n";
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