#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "classes.h"
#include "node.h"

class GenericParams : public Node {
public:
  GenericParams(const std::vector<Token> &tokens, int &ptr);
};

class FunctionParameters : public Node {
public:
  FunctionParameters(const std::vector<Token> &tokens, int &ptr);
};

class FunctionReturnType : public Node {
public:
  FunctionReturnType(const std::vector<Token> &tokens, int &ptr);
};

class WhereClause : public Node {
public:
  WhereClause(const std::vector<Token> &tokens, int &ptr);
};

class BlockExpression : public Node {
public:
  BlockExpression(const std::vector<Token> &tokens, int &ptr);
};

#endif