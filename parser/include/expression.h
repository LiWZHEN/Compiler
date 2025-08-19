#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "classes.h"
#include"node.h"

enum ExprType {
  unknown, block_expr, const_block_expr, infinite_loop_expr, predicate_loop_expr,
  if_expr, match_expr, literal_expr, path_in_expr, operator_expr, grouped_expr,
  array_expr, index_expr, struct_expr, call_expr, method_call_expr, field_expr,
  continue_expr, break_expr, return_expr, underscore_expr, lazy_boolean_expr,
  assignment_expr, compound_assignment_expr
};

class LiteralExpression : public LeafNode {
public:
  LiteralExpression(const std::vector<Token> &tokens, int &ptr);
};

class Expression : public Node {
public:
  Expression(const std::vector<Token> &tokens, int &ptr, ExprType expr_type, int min_bp);
private:
  ExprType expr_type_;
};

#endif