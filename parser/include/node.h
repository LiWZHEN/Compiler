#ifndef NODE_H
#define NODE_H

#include <vector>
#include "classes.h"
#include "token.h"

enum NodeType {
  type_crate, type_item, type_function, type_struct, type_enumeration, type_constant_item,
  type_trait, type_implementation, type_keyword, type_identifier, type_punctuation,
  type_function_parameters, type_function_return_type, type_block_expression, type_struct_fields,
  type_enum_variants, type_type, type_type_param_bounds, type_associated_item, type_type_path,
  type_self_param, type_shorthand_self, type_typed_self, type_function_param, type_statements,
  type_pattern, type_literal_pattern, type_identifier_pattern, type_wildcard_pattern,
  type_reference_pattern, type_tuple_struct_pattern, type_literal_expression, type_path_in_expression,
  type_tuple_struct_items, type_path_expr_segment, type_reference_type, type_array_type,
  type_slice_type, type_inferred_type, type_expression, type_expression_without_block,
  type_expression_with_block
};

class Node {
public:
  Node(const std::vector<Token> &tokens, int &ptr);
  ~Node();
  [[nodiscard]] int GetChildrenNum() const;
  [[nodiscard]] std::vector<Node *> const &GetChildrenPtr() const;
  [[nodiscard]] std::vector<NodeType> const &GetChildrenType() const;
protected:
  void AddChild(NodeType node_type);
  void ThrowErr(NodeType node_type, const std::string &info);
  void Restore(int size_before_try, int ptr_before_try);
  std::vector<Node *> children_;
  std::vector<NodeType> type_;
  const std::vector<Token> &tokens_;
  int &ptr_;
};

class LeafNode : public Node {
public:
  LeafNode(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] Token const &GetContent() const;
protected:
  Token token_;
};

class Crate : public Node {
public:
  Crate(const std::vector<Token> &tokens, int &ptr);
};

class Item : public Node {
public:
  Item(const std::vector<Token> &tokens, int &ptr);
};

#endif //NODE_H