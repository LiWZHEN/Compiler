#ifndef BUILDER_H
#define BUILDER_H

#include <vector>
#include "classes.h"
#include "token.h"
#include "node.h"

class Builder {
public:
  Builder() = delete;
  explicit Builder(const std::vector<Token> &tokens);
  Crate *GetTree();
private:
  const std::vector<Token> &tokens_;
  int ptr_ = 0;
};

#endif //BUILDER_H