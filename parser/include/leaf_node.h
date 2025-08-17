#ifndef MODULE_H
#define MODULE_H

#include "classes.h"
#include "node.h"

class Keyword: public Node {
public:
  Keyword(const std::vector<Token> &tokens, int &ptr);
};

class Identifier : public Node {
public:
  Identifier(const std::vector<Token> &tokens, int &ptr);
};

class Punctuation : public Node {
public:
  Punctuation(const std::vector<Token> &tokens, int &ptr);
};

#endif