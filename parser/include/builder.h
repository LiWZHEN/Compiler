#ifndef BUILDER_H
#define BUILDER_H

#include <vector>
#include "classes.h"
#include "token.h"
#include "node.h"

class Builder {
public:
  Builder() = delete;
  Builder(const std::vector<Token> &tokens);
  Node *GetTree();
private:
  const std::vector<Token> &tokens_;
  int ptr_ = 0;
};

#endif //BUILDER_H