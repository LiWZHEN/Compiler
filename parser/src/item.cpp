#include "item.h"

Function::Function(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // const?
    if (tokens[ptr].GetStr() == "const") {
      AddChild(type_keyword);
    }
    if (ptr >= tokens.size()) {
      ThrowErr(type_function, "");
    }
    // fn
    if (tokens[ptr].GetStr() != "fn") {
      ThrowErr(type_function, "Expect \"fn\".");
    }
    AddChild(type_keyword);
    if (ptr >= tokens.size()) {
      ThrowErr(type_function, "");
    }
    // IDENTIFIER
    if (tokens[ptr].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_function, "Expect IDENTIFIER.");
    }
    AddChild(type_identifier);
    if (ptr >= tokens.size()) {
      ThrowErr(type_function, "");
    }
    // (
    if (tokens[ptr].GetStr() != "(") {
      ThrowErr(type_function, "Expect \"(\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_function, "");
    }
    // FunctionParameters?
    if (tokens[ptr].GetStr() != ")") {
      AddChild(type_function_parameters);
      if (ptr >= tokens.size()) {
        ThrowErr(type_function, "");
      }
    }
    // )
    if (tokens[ptr].GetStr() != ")") {
      ThrowErr(type_function, "Expect \")\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_function, "");
    }
    // FunctionReturnType?
    if (tokens[ptr].GetStr() == "->") {
      AddChild(type_function_return_type);
      if (ptr >= tokens.size()) {
        ThrowErr(type_function, "");
      }
    }
    // ;|BlockExpression
    if (tokens[ptr].GetStr() == ";") {
      AddChild(type_punctuation);
    } else if (tokens[ptr].GetStr() == "{") {
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
    if (tokens[ptr].GetStr() != "struct") {
      ThrowErr(type_struct, "Expect \"struct\".");
    }
    AddChild(type_keyword);
    if (ptr >= tokens.size()) {
      ThrowErr(type_struct, "");
    }
    // IDENTIFIER
    if (tokens[ptr].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_struct, "Expect IDENTIFIER.");
    }
    AddChild(type_identifier);
    if (ptr >= tokens.size()) {
      ThrowErr(type_struct, "");
    }
    if (tokens[ptr].GetStr() == ";") {
      // ;
      AddChild(type_punctuation);
    } else if (tokens[ptr].GetStr() == "{") {
      // {StructFields?}
      // {
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_struct, "");
      }
      if (tokens[ptr].GetStr() != "}") {
        // StructFields
        AddChild(type_struct_fields);
        if (ptr >= tokens.size()) {
          ThrowErr(type_struct, "");
        }
      }
      // }
      if (tokens[ptr].GetStr() != "}") {
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
    if (tokens[ptr].GetStr() != "enum") {
      ThrowErr(type_enumeration, "Expect \"enum\".");
    }
    AddChild(type_keyword);
    if (ptr >= tokens.size()) {
      ThrowErr(type_enumeration, "");
    }
    // IDENTIFIER
    if (tokens[ptr].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_enumeration, "Expect IDENTIFIER.");
    }
    AddChild(type_identifier);
    if (ptr >= tokens.size()) {
      ThrowErr(type_enumeration, "");
    }
    // {
    if (tokens[ptr].GetStr() != "{") {
      ThrowErr(type_enumeration, "Expect \"{\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_enumeration, "");
    }
    // EnumVariants?
    if (tokens[ptr].GetStr() != "}") {
      // EnumVariants
      AddChild(type_enum_variants);
      if (ptr >= tokens.size()) {
        ThrowErr(type_enumeration, "");
      }
    }
    // }
    if (tokens[ptr].GetStr() != "}") {
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
    if (tokens[ptr].GetStr() != "const") {
      ThrowErr(type_constant_item, "Expect \"const\".");
    }
    AddChild(type_keyword);
    if (ptr >= tokens.size()) {
      ThrowErr(type_constant_item, "");
    }
    // IDENTIFIER
    if (tokens[ptr].GetType() == IDENTIFIER_OR_KEYWORD) {
      AddChild(type_identifier);
    } else {
      ThrowErr(type_constant_item, "Expect IDENTIFIER.");
    }
    if (ptr >= tokens.size()) {
      ThrowErr(type_constant_item, "");
    }
    // :
    if (tokens[ptr].GetStr() != ":") {
      ThrowErr(type_constant_item, "Expect \":\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_constant_item, "");
    }
    // Type
    AddChild(type_type);
    if (ptr >= tokens.size()) {
      ThrowErr(type_constant_item, "");
    }
    // (=Expression)?
    if (tokens[ptr].GetStr() == "=") {
      // =
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_constant_item, "");
      }
      // Expression
      AddChild(type_expression);
      if (ptr >= tokens.size()) {
        ThrowErr(type_constant_item, "");
      }
    }
    // ;
    if (tokens[ptr].GetStr() != ";") {
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
    if (tokens[ptr].GetStr() != "trait") {
      ThrowErr(type_trait, "Expect \"trait\".");
    }
    AddChild(type_keyword);
    if (ptr >= tokens.size()) {
      ThrowErr(type_trait, "");
    }
    // IDENTIFIER
    if (tokens[ptr].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_trait, "Expect IDENTIFIER.");
    }
    AddChild(type_identifier);
    if (ptr >= tokens.size()) {
      ThrowErr(type_trait, "");
    }
    // {
    if (tokens[ptr].GetStr() != "{") {
      ThrowErr(type_trait, "Expect \"{\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_trait, "");
    }
    // AssociatedItem*
    while (tokens[ptr].GetStr() != "}") {
      // AssociatedItem
      AddChild(type_associated_item);
      if (ptr >= tokens.size()) {
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
    if (tokens[ptr].GetStr() != "impl") {
      ThrowErr(type_implementation, "Expect \"impl\".");
    }
    AddChild(type_keyword);
    if (ptr >= tokens.size()) {
      ThrowErr(type_implementation, "");
    }
    if (ptr + 1 >= tokens.size()) {
      ThrowErr(type_implementation, "Incomplete implementation type.");
    }
    if (tokens[ptr + 1].GetStr() == "for") {
      // TraitImpl
      AddChild(type_identifier);
      AddChild(type_keyword);
    }
    if (ptr >= tokens.size()) {
      ThrowErr(type_implementation, "");
    }
    AddChild(type_type);
    if (ptr >= tokens.size()) {
      ThrowErr(type_implementation, "");
    }
    AddChild(type_punctuation);
    while (ptr < tokens.size() && tokens[ptr].GetStr() != "}") {
      AddChild(type_associated_item);
    }
    if (ptr >= tokens.size()) {
      ThrowErr(type_implementation, "");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}
