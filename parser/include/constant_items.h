#ifndef CONSTANT_ITEMS_H
#define CONSTANT_ITEMS_H

#include "classes.h"
#include "node.h"

class Type : public Node {
public:
  Type(const std::vector<Token> &tokens, int &ptr);
};

class Expression : public Node {
public:
  Expression(const std::vector<Token> &tokens, int &ptr);
};

#endif