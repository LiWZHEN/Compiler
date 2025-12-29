#include "functions.h"
#include "scope.h"

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
        // std::cerr << "FunctionParameters: Successfully handle the failure.\n";
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
          // std::cerr << "FunctionParameters: Successfully handle the try-failure.\n";
          return;
        }
      }
    } catch (...) {
      Restore(0, ptr_before_try);
      // std::cerr << "FunctionParameters: Successfully handle the failure.\n";
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

std::string FunctionParameters::GetNodeLabel() const {
  return "FunctionParameters";
}

std::string FunctionReturnType::GetNodeLabel() const {
  return "FunctionReturnType";
}

void FunctionParameters::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void FunctionReturnType::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void FunctionParameters::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {
  for (int i = 0; i < children_.size(); ++i) {
    if (type_[i] == type_punctuation) {
      continue;
    }
    children_[i]->AddSymbol(target_scope, false, true, false,
        false, target_node, {children_[i], type_[i]});
  }
}
void FunctionReturnType::AddSymbol(ScopeNode *target_scope, const bool need_type_add, const bool need_value_add,
    const bool associated_item_add, const bool field_item_add, ScopeNodeContent target_node,
    const ScopeNodeContent node_info) {}