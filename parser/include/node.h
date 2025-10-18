#ifndef NODE_H
#define NODE_H

#include <vector>
#include <unordered_set>
#include <map>
#include <memory>
#include "classes.h"
#include "token.h"

enum NodeType {
  type_crate, type_item, type_function, type_struct, type_enumeration, type_constant_item,
  type_trait, type_implementation, type_keyword, type_identifier, type_punctuation,
  type_function_parameters, type_function_return_type, type_block_expression, type_self_param,
  type_function_param, type_shorthand_self, type_type, type_pattern, type_reference_pattern,
  type_identifier_pattern, type_path_in_expression, type_path_expr_segment, type_reference_type,
  type_array_type, type_type_path, type_unit_type, type_expression, type_statements, type_statement,
  type_let_statement, type_expression_statement, type_struct_expr_fields, type_struct_expr_field,
  type_char_literal, type_string_literal, type_raw_string_literal, type_c_string_literal,
  type_raw_c_string_literal, type_integer_literal, type_struct_fields, type_struct_field,
  type_enum_variants, type_associated_item
};

enum BasicType {
  unknown_type, bool_type, i32_type, u32_type, isize_type, usize_type, char_type, str_type,
  string_type, unit_type, array_type, struct_type, enumeration_type, pointer_type, never_type
};

struct IntegratedType {
  BasicType basic_type;
  bool is_const;
  bool is_mutable;
  // int
  bool is_int;
  bool type_completed;
  std::unordered_set<BasicType> possible_types;
  // array
  long long size;
  // array / pointer
  std::shared_ptr<IntegratedType> element_type;
  // struct / enum
  Node *struct_node;
  IntegratedType() {
    basic_type = unknown_type;
    is_const = false;
    is_mutable = false;
    is_int = false;
    type_completed = false;
    possible_types.insert(i32_type);
    possible_types.insert(u32_type);
    possible_types.insert(isize_type);
    possible_types.insert(usize_type);
    size = 0;
    struct_node = nullptr;
  }
  IntegratedType(const BasicType basic_type, const bool is_const, const bool is_mutable, const bool is_int,
      const bool type_completed, const int size) : basic_type(basic_type), is_const(is_const), is_mutable(is_mutable),
      is_int(is_int), type_completed(type_completed), size(size) {
    possible_types.insert(i32_type);
    possible_types.insert(u32_type);
    possible_types.insert(isize_type);
    possible_types.insert(usize_type);
    struct_node = nullptr;
  }

  void RemovePossibility(const BasicType to_be_removed) {
    if (!possible_types.contains(to_be_removed)) {
      return;
    }
    possible_types.erase(to_be_removed);
    if (is_int && possible_types.empty()) {
      std::cerr << "Error: No valid int type!\n";
      throw "";
    }
    if (!possible_types.contains(basic_type)) {
      basic_type = *possible_types.begin();
    }
  }
};

struct Value {
  // integer / bool
  long long int_value = 0; // bool: 0->false, 1->true
  // array
  std::vector<Value> array_values;
  // string / char / str
  std::string str_value;
  // struct
  std::map<std::string, Value> struct_values;
  // enumeration
  Node *enum_value = nullptr;
  // pointer
  Node *pointer_value = nullptr;
  bool operator==(const Value &other) const;
};

struct ScopeNode;
struct ScopeNodeContent;

class Node {
public:
  Node(const std::vector<Token> &tokens, int &ptr);
  virtual ~Node();
  [[nodiscard]] int GetChildrenNum() const;
  [[nodiscard]] std::vector<Node *> const &GetChildrenPtr() const;
  [[nodiscard]] std::vector<NodeType> const &GetChildrenType() const;
  [[nodiscard]] virtual std::string GetStruct(const std::string &prefix, bool isLast) const;
  [[nodiscard]] virtual std::string GetNodeLabel() const;
  virtual void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) = 0;
protected:
  void AddChild(NodeType node_type);
  void ThrowErr(NodeType node_type, const std::string &info) const;
  void Restore(int size_before_try, int ptr_before_try);
  virtual void Accept(Visitor *visitor) = 0;
  friend class SymbolVisitor;
  friend class ValueTypeVisitor;
  friend struct Value;
  std::vector<Node *> children_;
  std::vector<NodeType> type_;
  const std::vector<Token> &tokens_;
  int &ptr_;
  std::shared_ptr<ScopeNode> scope_node_ = nullptr;
  std::shared_ptr<IntegratedType> integrated_type_ = nullptr;
  Value value_;
};

class LeafNode : public Node {
public:
  LeafNode(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] Token const &GetContent() const;
  [[nodiscard]] std::string GetNodeLabel() const override;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override = 0;
protected:
  void Accept(Visitor *visitor) override = 0;
  Token token_;
};

class Crate final : public Node {
public:
  Crate(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

class Item final : public Node {
public:
  Item(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

#endif //NODE_H