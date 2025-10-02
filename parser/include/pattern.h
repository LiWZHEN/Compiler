#ifndef PATTERN_H
#define PATTERN_H

#include "classes.h"
#include "node.h"

class Pattern final : public Node {
public:
  Pattern(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class LiteralPattern final : public Node {
public:
  LiteralPattern(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class IdentifierPattern final : public Node {
public:
  IdentifierPattern(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class ReferencePattern final : public Node {
public:
  ReferencePattern(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

#endif