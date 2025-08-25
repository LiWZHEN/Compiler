#ifndef FUNCTION_PARAMETERS_H
#define FUNCTION_PARAMETERS_H

#include "classes.h"
#include "node.h"

class SelfParam : public Node {
public:
  SelfParam(const std::vector<Token> &tokens, int &ptr);
};

class ShorthandSelf : public Node {
public:
  ShorthandSelf(const std::vector<Token> &tokens, int &ptr);
};

class TypedSelf : public Node {
public:
  TypedSelf(const std::vector<Token> &tokens, int &ptr);
};

class FunctionParam : public Node {
public:
  FunctionParam(const std::vector<Token> &tokens, int &ptr);
};
#endif