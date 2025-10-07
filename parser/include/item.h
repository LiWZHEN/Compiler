#ifndef ITEM_H
#define ITEM_H

#include "classes.h"
#include "node.h"
#include <unordered_map>
#include <unordered_set>

struct ScopeNodeContent;

class Function final : public Node {
public:
  Function(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  [[nodiscard]] std::string GetIdentifier() const;
  [[nodiscard]] bool IsConst() const;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

class Struct final : public Node {
public:
  Struct(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  [[nodiscard]] std::string GetIdentifier() const;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
  std::unordered_map<std::string, ScopeNodeContent> associated_items_;
  std::unordered_map<std::string, ScopeNodeContent> field_items_;
private:
  void Accept(Visitor *visitor) override;
};

class Enumeration final : public Node {
public:
  Enumeration(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  [[nodiscard]] std::string GetIdentifier() const;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
  std::unordered_set<std::string> enum_variants_;
  std::unordered_map<std::string, ScopeNodeContent> associated_items_;
private:
  void Accept(Visitor *visitor) override;
};

class ConstantItem final : public Node {
public:
  ConstantItem(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  [[nodiscard]] std::string GetIdentifier() const;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

class Trait final : public Node {
public:
  Trait(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  [[nodiscard]] std::string GetIdentifier() const;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
  std::unordered_map<std::string, ScopeNodeContent> associated_items_;
private:
  void Accept(Visitor *visitor) override;
};

class Implementation final : public Node {
public:
  Implementation(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

#endif