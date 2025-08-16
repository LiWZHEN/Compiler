#include "functions.h"

/*GenericParams::GenericParams(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // <
    if (tokens[ptr].GetStr() != "<") {
      ThrowErr(type_generic_params, "Expect \"<\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_generic_params, "");
    }
    if (tokens[ptr].GetStr() != ">") {
      AddChild(type_generic_param);
      if (ptr >= tokens.size()) {
        ThrowErr(type_generic_params, "");
      }
      while (true) {
        if (tokens[ptr].GetStr() != ",") {
          break;
        }
        AddChild(type_punctuation);
        if (ptr >= tokens.size()) {
          ThrowErr(type_generic_params, "");
        }
        if (tokens[ptr].GetStr() == ">") {
          break;
        }
        AddChild(type_generic_param);
        if (ptr >= tokens.size()) {
          ThrowErr(type_generic_params, "");
        }
      }
    }
    if (tokens[ptr].GetStr() != ">") {
      ThrowErr(type_generic_params, "Expect \">\".");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}*/

FunctionParameters::FunctionParameters(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    try {
      // (SelfParam,)?FunctionParam(,FunctionParam)*,?
      try {
        // SelfParam,
        AddChild(type_self_param);
        if (ptr >= tokens.size()) {
          ThrowErr(type_function_parameters, "");
        }
        if (tokens[ptr].GetStr() != ",") {
          ThrowErr(type_function_parameters, "Expect \",\".");
        }
        AddChild(type_punctuation);
        if (ptr >= tokens.size()) {
          ThrowErr(type_function_parameters, "");
        }
      } catch (...) {
        Restore(0, ptr_before_try);
        std::cerr << "FunctionParameters: Successfully handle the failure.\n";
      }
      // FunctionParam
      AddChild(type_function_param);
      // (,FunctionParam)*,?
      while (ptr < tokens.size()) {
        if (tokens[ptr].GetStr() != ",") {
          return;
        }
        AddChild(type_punctuation);
        if (ptr >= tokens.size()) {
          return;
        }
        const int size_before_trying_function_param = static_cast<int>(children_.size()),
            ptr_before_trying_function_param = ptr;
        try {
          AddChild(type_function_param);
        } catch (...) {
          Restore(size_before_trying_function_param, ptr_before_trying_function_param);
          std::cerr << "FunctionParameters: Successfully handle the try-failure.\n";
          return;
        }
      }
    } catch (...) {
      Restore(0, ptr_before_try);
      std::cerr << "FunctionParameters: Successfully handle the failure.\n";
      // SelfParam,?
      AddChild(type_self_param);
      if (ptr < tokens.size() && tokens[ptr].GetStr() == ",") {
        AddChild(type_punctuation);
      }
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

FunctionReturnType::FunctionReturnType(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    if (tokens[ptr].GetStr() != "->") {
      ThrowErr(type_function_return_type, "Expect \"->\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_function_return_type, "");
    }
    AddChild(type_type);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

/*WhereClause::WhereClause(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
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
    Restore(0, ptr_before_try);
    throw "";
  }
}*/

BlockExpression::BlockExpression(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // {
    if (tokens[ptr].GetStr() != "{") {
      ThrowErr(type_block_expression, "Expect \"{\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_block_expression, "");
    }
    // Statements?
    if (tokens[ptr].GetStr() != "}") {
      // Statements
      AddChild(type_statements);
    }
    // }
    if (ptr >= tokens.size()) {
      ThrowErr(type_block_expression, "");
    }
    if (tokens[ptr].GetStr() != "}") {
      ThrowErr(type_block_expression, "Expect \"}\".");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}
