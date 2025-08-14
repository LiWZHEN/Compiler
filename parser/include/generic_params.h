#ifndef GENERIC_PARAMS_H
#define GENERIC_PARAMS_H

#include "classes.h"
#include "node.h"

class GenericParam : public Node {
public:
  GenericParam(const std::vector<Token> &tokens, int &ptr);
};
#endif