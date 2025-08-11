#include "item.h"
#include "module.h"

Module::Module(const std::vector<Token> &tokens, int &ptr) {
  int cnt = 0;
  try {
    // mod
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
    std::string next_token = tokens[ptr].GetStr();
    if (next_token == ";") {
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
    } else if (next_token == "{") {
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Module: file ends before the module is completed.\n";
        throw "";
      }
      next_token = tokens[ptr].GetStr();
      while (next_token != "}") {
        children_.push_back(nullptr);
        children_[cnt] = new Item(tokens, ptr);
        type_.push_back(type_item);
        ++cnt;
        if (ptr >= tokens.size()) {
          std::cerr << "Module: file ends before the module is completed.\n";
          throw "";
        }
        next_token = tokens[ptr].GetStr();
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
  int cnt = 0;
  try {
    // fn
    children_.push_back(nullptr);
    children_[cnt] = new Keyword(tokens[ptr], ptr);
    type_.push_back(type_keyword);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "Function: file ends before the module is completed.\n";
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
      std::cerr << "Function: file ends before the module is completed.\n";
      throw "";
    }
    std::string next_token = tokens[ptr].GetStr();
    if (next_token == "<") {
      // GenericParams
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "Function: file ends before the module is completed.\n";
        throw "";
      }
      next_token = tokens[ptr].GetStr();

    }
    if (ptr >= tokens.size()) {
      std::cerr << "Function: file ends before the module is completed.\n";
      throw "";
    }
    next_token = tokens[ptr].GetStr();
    if (next_token == "(") {

    } else {

    }
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}
