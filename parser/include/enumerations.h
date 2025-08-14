#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

#include "classes.h"
#include "node.h"

class EnumVariants : public Node {
public:
  EnumVariants(const std::vector<Token> &tokens, int &ptr);
};

#endif