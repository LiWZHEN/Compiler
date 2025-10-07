#ifndef TYPE_H
#define TYPE_H

#include "classes.h"
#include "node.h"

class Type final : public Node {
public:
  Type(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

class TypePath final : public LeafNode {
public:
  TypePath(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

class ReferenceType final : public Node {
public:
  ReferenceType(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

class ArrayType final : public Node {
public:
  ArrayType(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

class UnitType final : public Node {
public:
  UnitType(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

#endif