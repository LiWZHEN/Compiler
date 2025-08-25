#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "classes.h"
#include "node.h"

class FunctionParameters : public Node {
public:
  FunctionParameters(const std::vector<Token> &tokens, int &ptr);
};

class FunctionReturnType : public Node {
public:
  FunctionReturnType(const std::vector<Token> &tokens, int &ptr);
};

#endif