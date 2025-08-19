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

class CharLiteral : public LeafNode {
public:
  CharLiteral(const std::vector<Token> &tokens, int &ptr);
};

class StringLiteral : public LeafNode {
public:
  StringLiteral(const std::vector<Token> &tokens, int &ptr);
};

class RawStringLiteral : public LeafNode {
public:
  RawStringLiteral(const std::vector<Token> &tokens, int &ptr);
};

class CStringLiteral : public LeafNode {
public:
  CStringLiteral(const std::vector<Token> &tokens, int &ptr);
};

class RawCStringLiteral : public LeafNode {
public:
  RawCStringLiteral(const std::vector<Token> &tokens, int &ptr);
};

class IntegerLiteral : public LeafNode {
public:
  IntegerLiteral(const std::vector<Token> &tokens, int &ptr);
};

#endif