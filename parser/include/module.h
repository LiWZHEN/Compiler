#ifndef MODULE_H
#define MODULE_H

#include "classes.h"
#include "node.h"

class Keyword: public Node {
public:
  Keyword(const Token &token, int &ptr);
private:
  Token token_;
};

class Identifier : public Node {
public:
  Identifier(const Token &token, int &ptr);
private:
  Token token_;
};

class Punctuation : public Node {
public:
  Punctuation(const Token &token, int &ptr);
private:
  Token token_;
};

#endif