#ifndef PATTERN_H
#define PATTERN_H

#include "classes.h"
#include "node.h"

class Pattern : public Node {
public:
  Pattern(const std::vector<Token> &tokens, int &ptr);
};

class LiteralPattern : public Node {
public:
  LiteralPattern(const std::vector<Token> &tokens, int &ptr);
};

class IdentifierPattern : public Node {
public:
  IdentifierPattern(const std::vector<Token> &tokens, int &ptr);
};

class WildcardPattern : public LeafNode {
public:
  WildcardPattern(const std::vector<Token> &tokens, int &ptr);
};

class ReferencePattern : public Node {
public:
  ReferencePattern(const std::vector<Token> &tokens, int &ptr);
};

#endif