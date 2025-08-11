#ifndef NODE_H
#define NODE_H

#include <vector>
#include "classes.h"
#include "token.h"

enum NodeType {
  type_crate, type_item, type_module, type_function, type_struct, type_enumeration,
  type_constant_item, type_trait, type_implementation, type_keyword, type_identifier,
  type_punctuation
};

class Node {
protected:
  std::vector<Node *> children_;
  std::vector<NodeType> type_;
};

class Crate : public Node {
public:
  Crate(const std::vector<Token> &tokens, int &ptr);
};

class Item : public Node {
public:
  Item(const std::vector<Token> &tokens, int &ptr);
};

#endif //NODE_H