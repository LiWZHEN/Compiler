#ifndef ITEM_H
#define ITEM_H

#include "classes.h"
#include "node.h"

class Function : public Node {
public:
  Function(const std::vector<Token> &tokens, int &ptr);
};

class Struct : public Node {
public:
  Struct(const std::vector<Token> &tokens, int &ptr);
};

class Enumeration : public Node {
public:
  Enumeration(const std::vector<Token> &tokens, int &ptr);
};

class ConstantItem : public Node {
public:
  ConstantItem(const std::vector<Token> &tokens, int &ptr);
};

class Trait : public Node {
public:
  Trait(const std::vector<Token> &tokens, int &ptr);
};

class Implementation : public Node {
public:
  Implementation(const std::vector<Token> &tokens, int &ptr);
};

#endif