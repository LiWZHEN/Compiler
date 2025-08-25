#ifndef TYPE_H
#define TYPE_H

#include "classes.h"
#include "node.h"

class Type : public Node {
public:
  Type(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

class TypePath : public LeafNode {
public:
  TypePath(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

class ReferenceType : public Node {
public:
  ReferenceType(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

class ArrayType : public Node {
public:
  ArrayType(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

class UnitType : public Node {
public:
  UnitType(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
};

#endif