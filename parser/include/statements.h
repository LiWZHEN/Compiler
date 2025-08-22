#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "classes.h"
#include "node.h"

class Statements : public Node {
public:
  Statements(const std::vector<Token> &tokens, int &ptr);
};

class Statement : public Node {
public:
  Statement(const std::vector<Token> &tokens, int &ptr);
};

class LetStatement : public Node {
public:
  LetStatement(const std::vector<Token> &tokens, int &ptr);
};

class ExpressionStatement : public Node {
public:
  ExpressionStatement(const std::vector<Token> &tokens, int &ptr);
};

#endif