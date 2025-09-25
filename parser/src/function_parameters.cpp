#include "function_parameters.h"
#include "scope.h"

SelfParam::SelfParam(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // ShorthandSelf
    AddChild(type_shorthand_self);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

ShorthandSelf::ShorthandSelf(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // &?
    if (tokens[ptr].GetStr() == "&") {
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_shorthand_self, "");
      }
    }
    // mut?
    if (tokens[ptr].GetStr() == "mut") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_shorthand_self, "");
      }
    }
    // self
    if (tokens[ptr].GetStr() != "self") {
      ThrowErr(type_shorthand_self, "Expect \"self\".");
    }
    AddChild(type_keyword);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

FunctionParam::FunctionParam(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // PatternNoTopAlt
    AddChild(type_pattern);
    if (ptr >= tokens.size()) {
      ThrowErr(type_function_param, "");
    }
    // :
    if (tokens[ptr].GetStr() != ":") {
      ThrowErr(type_function_param, "Expect \":\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_function_param, "");
    }
    AddChild(type_type);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

std::string SelfParam::GetNodeLabel() const {
  return "SelfParam";
}

std::string ShorthandSelf::GetNodeLabel() const {
  return "ShorthandSelf";
}

std::string FunctionParam::GetNodeLabel() const {
  return "FunctionParam";
}

void SelfParam::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void ShorthandSelf::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void FunctionParam::Accept(Visitor *visitor) {
  visitor->Visit(this);
}