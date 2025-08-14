#ifndef TRAIT_H
#define TRAIT_H

#include "classes.h"
#include "node.h"

class TypeParamBounds : public Node {
public:
  TypeParamBounds(const std::vector<Token> &tokens, int &ptr);
};

class AssociatedItem : public Node {
public:
  AssociatedItem(const std::vector<Token> &tokens, int &ptr);
};

#endif