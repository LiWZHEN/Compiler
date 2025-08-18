#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "classes.h"
#include"node.h"

class LiteralExpression : public LeafNode {
public:
  LiteralExpression(const std::vector<Token> &tokens, int &ptr);
};

#endif