#ifndef STRUCTS_H
#define STRUCTS_H

#include "classes.h"
#include "node.h"

class StructFields : public Node {
public:
  StructFields(const std::vector<Token> &tokens, int &ptr);
};

#endif