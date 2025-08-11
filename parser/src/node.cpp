#include "node.h"
#include "item.h"

Crate::Crate(const std::vector<Token> &tokens, int &ptr) {
  int cnt = 0;
  try {
    while (ptr < tokens.size()) {
      children_.push_back(nullptr);
      children_[cnt] = new Item(tokens, ptr);
      type_.push_back(type_item);
      ++cnt;
    }
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}

Item::Item(const std::vector<Token> &tokens, int &ptr) {
  int cnt = 0;
  try {
    std::string next_token = tokens[ptr].GetStr();
    if (next_token == "mod") {
      children_.push_back(nullptr);
      children_[cnt] = new Module(tokens, ptr);
      type_.push_back(type_module);
      ++cnt;
    } else if (next_token == "fn") {
      children_.push_back(nullptr);
      children_[cnt] = new Function(tokens, ptr);
      type_.push_back(type_function);
      ++cnt;
    } else if (next_token == "struct") {
      children_.push_back(nullptr);
      children_[cnt] = new Struct(tokens, ptr);
      type_.push_back(type_struct);
      ++cnt;
    } else if (next_token == "enum") {
      children_.push_back(nullptr);
      children_[cnt] = new Enumeration(tokens, ptr);
      type_.push_back(type_enumeration);
      ++cnt;
    } else if (next_token == "const") {
      children_.push_back(nullptr);
      children_[cnt] = new ConstantItem(tokens, ptr);
      type_.push_back(type_constant_item);
      ++cnt;
    } else if (next_token == "trait") {
      children_.push_back(nullptr);
      children_[cnt] = new Trait(tokens, ptr);
      type_.push_back(type_trait);
      ++cnt;
    } else if (next_token == "impl") {
      children_.push_back(nullptr);
      children_[cnt] = new Implementation(tokens, ptr);
      type_.push_back(type_implementation);
      ++cnt;
    } else {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": Item: the keyword is invalid.\n";
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
