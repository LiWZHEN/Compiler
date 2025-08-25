#ifndef PATTERN_H
#define PATTERN_H

#include "classes.h"
#include "node.h"

class Pattern : public Node {
public:
  Pattern(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

class LiteralPattern : public Node {
public:
  LiteralPattern(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

class IdentifierPattern : public Node {
public:
  IdentifierPattern(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

class WildcardPattern : public LeafNode {
public:
  WildcardPattern(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

class ReferencePattern : public Node {
public:
  ReferencePattern(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

#endif