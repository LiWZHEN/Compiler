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

class WildcardPattern : public Node {
public:
  WildcardPattern(const std::vector<Token> &tokens, int &ptr);
};

class ReferencePattern : public Node {
public:
  ReferencePattern(const std::vector<Token> &tokens, int &ptr);
};

class TupleStructPattern : public Node {
  TupleStructPattern(const std::vector<Token> &tokens, int &ptr);
};

class PathPattern : public Node {
  PathPattern(const std::vector<Token> &tokens, int &ptr);
};

#endif