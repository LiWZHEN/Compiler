#ifndef STRUCTS_H
#define STRUCTS_H

#include "classes.h"
#include "node.h"

class StructFields final : public Node {
public:
  StructFields(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class StructField final : public Node {
public:
  StructField(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

#endif