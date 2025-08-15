#ifndef NODE_H
#define NODE_H

#include <vector>
#include "classes.h"
#include "token.h"

enum NodeType {
  type_crate, type_item, type_function, type_struct, type_enumeration,
  type_constant_item, type_trait, type_implementation, type_keyword, type_identifier,
  type_punctuation, type_generic_params, type_function_parameters, type_function_return_type,
  type_where_clause, type_block_expression, type_struct_fields, type_enum_variants,
  type_type, type_expression, type_type_param_bounds, type_associated_item,
  type_type_path, type_generic_param, type_self_param, type_shorthand_self, type_typed_self,
  type_function_param, type_function_param_pattern, type_where_clause_item,
  type_statements
};

class Node {
public:
  ~Node();
protected:
  void AddChild(NodeType node_type, const std::vector<Token> &tokens, int &ptr);
  void ThrowErr(NodeType node_type, const std::string &info, bool is_ending, int line, int column);
  void Restore(int size_before_try);
  std::vector<Node *> children_;
  std::vector<NodeType> type_;
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