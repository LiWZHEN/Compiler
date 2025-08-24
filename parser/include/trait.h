#ifndef TRAIT_H
#define TRAIT_H

#include "classes.h"
#include "node.h"

class AssociatedItem : public Node {
public:
  AssociatedItem(const std::vector<Token> &tokens, int &ptr);
};

#endif