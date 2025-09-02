#ifndef PATH_H
#define PATH_H

#include "classes.h"
#include "node.h"

class PathInExpression final : public Node {
public:
  PathInExpression(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class PathExprSegment final : public LeafNode {
public:
  PathExprSegment(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

#endif