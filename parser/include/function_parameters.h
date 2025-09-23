#ifndef FUNCTION_PARAMETERS_H
#define FUNCTION_PARAMETERS_H

#include "classes.h"
#include "node.h"

class SelfParam final : public Node {
public:
  SelfParam(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class ShorthandSelf final : public Node {
public:
  ShorthandSelf(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class FunctionParam final : public Node {
public:
  FunctionParam(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

#endif