#ifndef MODULE_H
#define MODULE_H

#include "classes.h"
#include "node.h"

class Keyword: public LeafNode {
public:
  Keyword(const std::vector<Token> &tokens, int &ptr);
};

class Identifier : public LeafNode {
public:
  Identifier(const std::vector<Token> &tokens, int &ptr);
};

class Punctuation : public LeafNode {
public:
  Punctuation(const std::vector<Token> &tokens, int &ptr);
};

#endif