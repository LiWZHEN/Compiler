#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "classes.h"
#include "node.h"

class Statements final : public Node {
public:
  Statements(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class Statement final : public Node {
public:
  Statement(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class LetStatement final : public Node {
public:
  LetStatement(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class ExpressionStatement final : public Node {
public:
  ExpressionStatement(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

#endif