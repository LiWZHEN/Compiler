#include "item.h"
#include "module.h"
#include "functions.h"
#include "structs.h"
#include "enumerations.h"
#include "constant_items.h"
#include "trait.h"
#include "implementation.h"

Module::Module(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    // mod
    if (tokens[ptr].GetStr() != "mod") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Module: There is no keyword \"mod\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Keyword(tokens[ptr], ptr);
    type_.push_back(type_keyword);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Module: file ends before the module is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetType() != IDENTIFIER_OR_KEYWORD) {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Module: There is no identifier after keyword \"mod\".\n";
      throw "";
    }
    // IDENTIFIER
    children_.push_back(nullptr);
    children_[cnt] = new Identifier(tokens[ptr], ptr);
    type_.push_back(type_identifier);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Module: file ends before the module is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() == ";") {
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
    } else if (tokens[ptr].GetStr() == "{") {
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Module: file ends before the module is completed.\n";
        throw "";
      }
      while (tokens[ptr].GetStr() != "}") {
        children_.push_back(nullptr);
        children_[cnt] = new Item(tokens, ptr);
        type_.push_back(type_item);
        ++cnt;
        if (ptr >= tokens.size()) {
          std::cerr << "Module: file ends before the module is completed.\n";
          throw "";
        }
      }
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
    } else {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Module: There is neither \";\" nor \"{\" after identifier.\n";
      throw "";
    }
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}

Function::Function(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    // fn
    if (tokens[ptr].GetStr() != "fn") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Function: There is no keyword \"fn\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Keyword(tokens[ptr], ptr);
    type_.push_back(type_keyword);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Function: file ends before the function is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetType() != IDENTIFIER_OR_KEYWORD) {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Function: There is no identifier after keyword \"fn\".\n";
      throw "";
    }
    // IDENTIFIER
    children_.push_back(nullptr);
    children_[cnt] = new Identifier(tokens[ptr], ptr);
    type_.push_back(type_identifier);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Function: file ends before the function is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() == "<") {
      // GenericParams
      children_.push_back(nullptr);
      children_[cnt] = new GenericParams(tokens, ptr);
      type_.push_back(type_generic_params);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Function: file ends before the function is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() != "(") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Function: There is no \"(\" of a function.\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Function: file ends before the function is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() != ")") {
      children_.push_back(nullptr);
      children_[cnt] = new FunctionParameters(tokens, ptr);
      type_.push_back(type_function_parameters);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Function: file ends before the function is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() != ")") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Function: Cannot find \")\" after function parameters.\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Function: file ends before the function is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() == "->") {
      children_.push_back(nullptr);
      children_[cnt] = new FunctionReturnType(tokens, ptr);
      type_.push_back(type_function_return_type);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Function: file ends before the function is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() == "where") {
      children_.push_back(nullptr);
      children_[cnt] = new WhereClause(tokens, ptr);
      type_.push_back(type_where_clause);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Function: file ends before the function is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() == ";") {
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
    } else if (tokens[ptr].GetStr() == "{") {
      children_.push_back(nullptr);
      children_[cnt] = new BlockExpression(tokens, ptr);
      type_.push_back(type_block_expression);
      ++cnt;
    } else {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Function: There is neither \";\" nor block expression at the end of function definition.\n";
      throw "";
    }
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}

Struct::Struct(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    // struct
    if (tokens[ptr].GetStr() != "struct") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Struct: There is no keyword \"struct\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Keyword(tokens[ptr], ptr);
    type_.push_back(type_keyword);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Struct: file ends before the struct is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetType() != IDENTIFIER_OR_KEYWORD) {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Struct: There is no identifier after keyword \"struct\".\n";
      throw "";
    }
    // IDENTIFIER
    children_.push_back(nullptr);
    children_[cnt] = new Identifier(tokens[ptr], ptr);
    type_.push_back(type_identifier);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Struct: file ends before the struct is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() == "<") {
      // GenericParams
      children_.push_back(nullptr);
      children_[cnt] = new GenericParams(tokens, ptr);
      type_.push_back(type_generic_params);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Struct: file ends before the struct is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() == "where") {
      // WhereClause
      children_.push_back(nullptr);
      children_[cnt] = new WhereClause(tokens, ptr);
      type_.push_back(type_where_clause);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Struct: file ends before the struct is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() == ";") {
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
    } else if (tokens[ptr].GetStr() == "{") {
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Struct: file ends before the struct is completed.\n";
        throw "";
      }
      if (tokens[ptr].GetStr() != "}") {
        // StructFields
        children_.push_back(nullptr);
        children_[cnt] = new StructFields(tokens, ptr);
        type_.push_back(type_struct_fields);
        ++cnt;
        if (ptr >= tokens.size()) {
          std::cerr << "Struct: file ends before the struct is completed.\n";
          throw "";
        }
      }
      if (tokens[ptr].GetStr() != "}") {
        std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Struct: There is no \"}\" following the struct fields.\n";
        throw "";
      }
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
    } else {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Function: There is neither \";\" nor \"{ StructFields }\" at the end of struct definition.\n";
      throw "";
    }
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}

Enumeration::Enumeration(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    // enum
    if (tokens[ptr].GetStr() != "enum") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Enumeration: There is no keyword \"enum\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Keyword(tokens[ptr], ptr);
    type_.push_back(type_keyword);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Enumeration: file ends before the enumeration is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetType() != IDENTIFIER_OR_KEYWORD) {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Enumeration: There is no identifier after keyword \"enum\".\n";
      throw "";
    }
    // IDENTIFIER
    children_.push_back(nullptr);
    children_[cnt] = new Identifier(tokens[ptr], ptr);
    type_.push_back(type_identifier);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Enumeration: file ends before the enumeration is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() == "<") {
      // GenericParams
      children_.push_back(nullptr);
      children_[cnt] = new GenericParams(tokens, ptr);
      type_.push_back(type_generic_params);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Enumeration: file ends before the enumeration is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() == "where") {
      // where
      children_.push_back(nullptr);
      children_[cnt] = new WhereClause(tokens, ptr);
      type_.push_back(type_where_clause);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Enumeration: file ends before the enumeration is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() != "{") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Enumeration: Cannot find \"{\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Enumeration: file ends before the enumeration is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() != "}") {
      children_.push_back(nullptr);
      children_[cnt] = new EnumVariants(tokens, ptr);
      type_.push_back(type_enum_variants);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Enumeration: file ends before the enumeration is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() != "}") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Enumeration: Cannot find the \"}\" following enum variants.\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}

ConstantItem::ConstantItem(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    // const
    if (tokens[ptr].GetStr() != "const") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": ConstantItem: There is no keyword \"const\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Keyword(tokens[ptr], ptr);
    type_.push_back(type_keyword);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "ConstantItem: file ends before the constant item is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() == "_") {
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
    } else if (tokens[ptr].GetType() == IDENTIFIER_OR_KEYWORD) {
      children_.push_back(nullptr);
      children_[cnt] = new Identifier(tokens[ptr], ptr);
      type_.push_back(type_identifier);
      ++cnt;
    } else {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": ConstantItem: There is neither \"_\" or identifier following \"const\".\n";
      throw "";
    }
    if (ptr >= tokens.size()) {
      std::cerr << "ConstantItem: file ends before the constant item is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() != ":") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": ConstantItem: expect \":\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "ConstantItem: file ends before the constant item is completed.\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Type(tokens, ptr);
    type_.push_back(type_type);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "ConstantItem: file ends before the constant item is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() == "=") {
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "ConstantItem: file ends before the constant item is completed.\n";
        throw "";
      }
      children_.push_back(nullptr);
      children_[cnt] = new Expression(tokens, ptr);
      type_.push_back(type_expression);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "ConstantItem: file ends before the constant item is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() != ";") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": ConstantItem: expect \";\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}

Trait::Trait(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    // trait
    if (tokens[ptr].GetStr() != "trait") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Trait: There is no keyword \"trait\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Keyword(tokens[ptr], ptr);
    type_.push_back(type_keyword);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Trait: file ends before the trait is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetType() != IDENTIFIER_OR_KEYWORD) {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Trait: There is no identifier following the keyword \"trait\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Identifier(tokens[ptr], ptr);
    type_.push_back(type_identifier);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Trait: file ends before the trait is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() == "<") {
      // GenericParams
      children_.push_back(nullptr);
      children_[cnt] = new GenericParams(tokens, ptr);
      type_.push_back(type_generic_params);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Trait: file ends before the trait is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() == ":") {
      // : TypeParamBounds?
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Trait: file ends before the trait is completed.\n";
        throw "";
      }
      if (tokens[ptr].GetStr() != "where" && tokens[ptr].GetStr() != "{") {
        children_.push_back(nullptr);
        children_[cnt] = new TypeParamBounds(tokens, ptr);
        type_.push_back(type_type_param_bounds);
        ++cnt;
        if (ptr >= tokens.size()) {
          std::cerr << "Trait: file ends before the trait is completed.\n";
          throw "";
        }
      }
    }
    if (tokens[ptr].GetStr() == "where") {
      children_.push_back(nullptr);
      children_[cnt] = new WhereClause(tokens, ptr);
      type_.push_back(type_where_clause);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Trait: file ends before the trait is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() != "{") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Trait: Expect \"{\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Trait: file ends before the trait is completed.\n";
      throw "";
    }
    while (tokens[ptr].GetStr() != "}") {
      children_.push_back(nullptr);
      children_[cnt] = new AssociatedItem(tokens, ptr);
      type_.push_back(type_associated_item);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Trait: file ends before the trait is completed.\n";
        throw "";
      }
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}

Implementation::Implementation(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    // impl
    if (tokens[ptr].GetStr() != "impl") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Implementation: There is no keyword \"impl\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Keyword(tokens[ptr], ptr);
    type_.push_back(type_keyword);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Implementation: file ends before the implementation is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() == "<") {
      children_.push_back(nullptr);
      children_[cnt] = new GenericParams(tokens, ptr);
      type_.push_back(type_generic_params);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Implementation: file ends before the implementation is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() == "!") {
      // TraitImpl
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Implementation: file ends before the implementation is completed.\n";
        throw "";
      }
      children_.push_back(nullptr);
      children_[cnt] = new TypePath(tokens, ptr);
      type_.push_back(type_type_path);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Implementation: file ends before the implementation is completed.\n";
        throw "";
      }
      if (tokens[ptr].GetStr() != "for") {
        std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Implementation: Expect keyword \"for\".\n";
        throw "";
      }
      children_.push_back(nullptr);
      children_[cnt] = new Keyword(tokens[ptr], ptr);
      type_.push_back(type_keyword);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Implementation: file ends before the implementation is completed.\n";
        throw "";
      }
    } else {
      int size_before_try = cnt;
      try {
        // try to match the TraitImpl type
        children_.push_back(nullptr);
        children_[cnt] = new TypePath(tokens, ptr);
        type_.push_back(type_type_path);
        ++cnt;
        if (ptr >= tokens.size()) {
          std::cerr << "Implementation: file ends before the implementation is completed.\n";
          throw "";
        }
        if (tokens[ptr].GetStr() != "for") {
          std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Implementation: Expect keyword \"for\".\n";
          throw "";
        }
        children_.push_back(nullptr);
        children_[cnt] = new Keyword(tokens[ptr], ptr);
        type_.push_back(type_keyword);
        ++cnt;
        if (ptr >= tokens.size()) {
          std::cerr << "Implementation: file ends before the implementation is completed.\n";
          throw "";
        }
      } catch (...) {
        // failure: cannot match TraitImpl type
        for (int i = size_before_try; i < children_.size(); ++i) {
          delete children_[i];
          children_[i] = nullptr;
        }
        children_.resize(size_before_try);
        type_.resize(size_before_try);
        cnt = size_before_try;
        std::cerr << "Implementation: Successfully handle the failed try of matching TraitImpl.\n";
      }
    }
    children_.push_back(nullptr);
    children_[cnt] = new Type(tokens, ptr);
    type_.push_back(type_type);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Implementation: file ends before the implementation is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() == "where") {
      children_.push_back(nullptr);
      children_[cnt] = new WhereClause(tokens, ptr);
      type_.push_back(type_where_clause);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Implementation: file ends before the implementation is completed.\n";
        throw "";
      }
    }
    if (tokens[ptr].GetStr() != "{") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Implementation: Expect \"{\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Implementation: file ends before the implementation is completed.\n";
      throw "";
    }
    while (tokens[ptr].GetStr() != "}") {
      children_.push_back(nullptr);
      children_[cnt] = new AssociatedItem(tokens, ptr);
      type_.push_back(type_associated_item);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Implementation: file ends before the implementation is completed.\n";
        throw "";
      }
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}
