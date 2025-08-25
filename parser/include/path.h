#ifndef PATH_H
#define PATH_H

#include "classes.h"
#include "node.h"

class PathInExpression : public Node {
public:
  PathInExpression(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

class PathExprSegment : public LeafNode {
public:
  PathExprSegment(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

#endif