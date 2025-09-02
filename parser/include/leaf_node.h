#ifndef MODULE_H
#define MODULE_H

#include "classes.h"
#include "node.h"

class Keyword final : public LeafNode {
public:
  Keyword(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class Identifier final : public LeafNode {
public:
  Identifier(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class Punctuation final : public LeafNode {
public:
  Punctuation(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class CharLiteral final : public LeafNode {
public:
  CharLiteral(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class StringLiteral final : public LeafNode {
public:
  StringLiteral(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class RawStringLiteral final : public LeafNode {
public:
  RawStringLiteral(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class CStringLiteral final : public LeafNode {
public:
  CStringLiteral(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class RawCStringLiteral final : public LeafNode {
public:
  RawCStringLiteral(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class IntegerLiteral final : public LeafNode {
public:
  IntegerLiteral(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

#endif