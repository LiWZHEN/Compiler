#include "functions.h"
#include "generic_params.h"
#include "module.h"
#include "function_parameters.h"
#include "constant_items.h"
#include "where_clause.h"
#include "block_expression.h"

GenericParams::GenericParams(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    // <
    if (tokens[ptr].GetStr() != "<") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": GenericParams: Expect \"<\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "GenericParams: file ends before the generic params is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() != ">") {
      children_.push_back(nullptr);
      children_[cnt] = new GenericParam(tokens, ptr);
      type_.push_back(type_generic_param);
      ++cnt;
      if (ptr >= tokens.size()) {
        std::cerr << "GenericParams: file ends before the generic params is completed.\n";
        throw "";
      }
      while (true) {
        if (tokens[ptr].GetStr() != ",") {
          break;
        }
        children_.push_back(nullptr);
        children_[cnt] = new Punctuation(tokens[ptr], ptr);
        type_.push_back(type_punctuation);
        ++cnt;
        if (ptr >= tokens.size()) {
          std::cerr << "GenericParams: file ends before the generic params is completed.\n";
          throw "";
        }
        if (tokens[ptr].GetStr() == ">") {
          break;
        }
        children_.push_back(nullptr);
        children_[cnt] = new GenericParam(tokens, ptr);
        type_.push_back(type_generic_param);
        ++cnt;
        if (ptr >= tokens.size()) {
          std::cerr << "GenericParams: file ends before the generic params is completed.\n";
          throw "";
        }
      }
    }
    if (tokens[ptr].GetStr() != ">") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": GenericParams: Expect \">\".\n";
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

FunctionParameters::FunctionParameters(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    try {
      // (SelfParam,)?FunctionParam(,FunctionParam)*,?
      try {
        // SelfParam,
        children_.push_back(nullptr);
        children_[cnt] = new SelfParam(tokens, ptr);
        type_.push_back(type_self_param);
        ++cnt;
        if (ptr >= tokens.size()) {
          std::cerr << "FunctionParameters: file ends before the function parameters is completed.\n";
          throw "";
        }
        if (tokens[ptr].GetStr() != ",") {
          std::cerr << "FunctionParameters: Expect \",\".\n";
        }
        children_.push_back(nullptr);
        children_[cnt] = new Punctuation(tokens[ptr], ptr);
        type_.push_back(type_punctuation);
        ++cnt;
        if (ptr >= tokens.size()) {
          std::cerr << "FunctionParameters: file ends before the function parameters is completed.\n";
          throw "";
        }
      } catch (...) {
        for (auto &it : children_) {
          delete it;
          it = nullptr;
        }
        children_.resize(0);
        type_.resize(0);
        cnt = 0;
        std::cerr << "FunctionParameters: Successfully handle the failure.\n";
      }
      // FunctionParam
      children_.push_back(nullptr);
      children_[cnt] = new FunctionParam(tokens, ptr);
      type_.push_back(type_function_param);
      ++cnt;
      // (,FunctionParam)*,?
      while (ptr < tokens.size()) {
        if (tokens[ptr].GetStr() != ",") {
          return;
        }
        children_.push_back(nullptr);
        children_[cnt] = new Punctuation(tokens[ptr], ptr);
        type_.push_back(type_punctuation);
        ++cnt;
        if (ptr >= tokens.size()) {
          return;
        }
        const int size_before_try = cnt;
        try {
          children_.push_back(nullptr);
          children_[cnt] = new FunctionParam(tokens, ptr);
          type_.push_back(type_function_param);
          ++cnt;
        } catch (...) {
          for (int i = size_before_try; i < children_.size(); ++i) {
            delete children_[i];
            children_[i] = nullptr;
          }
          children_.resize(size_before_try);
          type_.resize(size_before_try);
          cnt = size_before_try;
          std::cerr << "FunctionParameters: Successfully handle the try-failure.\n";
          return;
        }
      }
    } catch (...) {
      for (auto &it : children_) {
        delete it;
        it = nullptr;
      }
      children_.resize(0);
      type_.resize(0);
      cnt = 0;
      std::cerr << "FunctionParameters: Successfully handle the failure.\n";
      // SelfParam,?
      children_.push_back(nullptr);
      children_[cnt] = new SelfParam(tokens, ptr);
      type_.push_back(type_self_param);
      ++cnt;
      if (ptr < tokens.size() && tokens[ptr].GetStr() == ",") {
        children_.push_back(nullptr);
        children_[cnt] = new Punctuation(tokens[ptr], ptr);
        type_.push_back(type_punctuation);
        ++cnt;
      }
    }
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}

FunctionReturnType::FunctionReturnType(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    if (tokens[ptr].GetStr() != "->") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": FunctionReturnType: Expect \"->\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "FunctionReturnType: file ends before the function return type is completed.\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Type(tokens, ptr);
    type_.push_back(type_type);
    ++cnt;
  } catch (...) {
    for (auto &it : children_) {
      delete it;
      it = nullptr;
    }
    throw "";
  }
}

WhereClause::WhereClause(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    if (tokens[ptr].GetStr() != "where") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": WhereClause: Expect keyword \"where\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Keyword(tokens[ptr], ptr);
    type_.push_back(type_keyword);
    ++cnt;
    while (ptr < tokens.size()) {
      const int size_before_try = cnt;
      try {
        children_.push_back(nullptr);
        children_[cnt] = new WhereClauseItem(tokens, ptr);
        type_.push_back(type_where_clause_item);
        ++cnt;
      } catch (...) {
        for (int i = size_before_try; i < children_.size(); ++i) {
          delete children_[i];
          children_[i] = nullptr;
        }
        children_.resize(size_before_try);
        type_.resize(size_before_try);
        std::cerr << "WhereClause: Successfully handle the try-failure.\n";
        return;
      }
      if (ptr >= tokens.size() || tokens[ptr].GetStr() != ",") {
        return;
      }
      children_.push_back(nullptr);
      children_[cnt] = new Punctuation(tokens[ptr], ptr);
      type_.push_back(type_punctuation);
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

BlockExpression::BlockExpression(const std::vector<Token> &tokens, int &ptr) {
  try {
    int cnt = 0;
    if (tokens[ptr].GetStr() != "{") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": BlockExpression: Expect \"{\".\n";
      throw "";
    }
    children_.push_back(nullptr);
    children_[cnt] = new Punctuation(tokens[ptr], ptr);
    type_.push_back(type_punctuation);
    ++cnt;
    if (ptr >= tokens.size()) {
      std::cerr << "BlockExpression: file ends before the block expression is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() != "}") {
      children_.push_back(nullptr);
      children_[cnt] = new Statements(tokens, ptr);
      type_.push_back(type_statements);
      ++cnt;
    }
    if (ptr >= tokens.size()) {
      std::cerr << "BlockExpression: file ends before the block expression is completed.\n";
      throw "";
    }
    if (tokens[ptr].GetStr() != "}") {
      std::cerr << "line " << tokens[ptr].GetLine() << ", column " << tokens[ptr].GetColumn() << ": BlockExpression: Expect \"}\".\n";
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
