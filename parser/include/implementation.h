#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H

#include "classes.h"
#include "node.h"

class TypePath : public Node {
public:
  TypePath(const std::vector<Token> &tokens, int &ptr);
};

#endif