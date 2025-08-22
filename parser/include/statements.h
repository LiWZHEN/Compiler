#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "classes.h"
#include "node.h"

class Statements : public Node {
public:
  Statements(const std::vector<Token> &tokens, int &ptr);
};

#endif