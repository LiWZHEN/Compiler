#include "functions.h"

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