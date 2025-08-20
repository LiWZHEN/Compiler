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

enum Prefix {
  not_prefix, borrow, dereference, negation
};

enum Infix {
  not_infix, brackets, small_brackets, dot, add, minus, multiply, divide, mod,
  bitwise_and, bitwise_or, bitwise_xor, left_shift, right_shift, is_equal,
  is_not_equal, is_bigger, is_smaller, is_not_smaller, is_not_bigger, logic_or,
  logic_and, type_cast, assign, add_assign, minus_assign, multiply_assign,
  divide_assign, mod_assign, bitwise_and_assign, bitwise_or_assign,
  bitwise_xor_assign, left_shift_assign, right_shift_assign
};

class LiteralExpression : public LeafNode {
public:
  LiteralExpression(const std::vector<Token> &tokens, int &ptr);
};

class Expression : public Node {
public:
  Expression(const std::vector<Token> &tokens, int &ptr, ExprType expr_type, double min_bp);
  [[nodiscard]] ExprType GetNextExprType() const;
  // [[nodiscard]] ExprType GetExprTypeForTest() const;
  [[nodiscard]] Infix GetInfixForTest() const;
private:
  Expression(Expression *lhs, Expression *rhs, Infix infix);
  ExprType expr_type_;
  // Prefix prefix_ = not_prefix;
  Infix infix_ = not_infix;
};

class StructExprField : public Node {
public:
  StructExprField(const std::vector<Token> &tokens, int &ptr);
};

class StructExprFields : public Node {
public:
  StructExprFields(const std::vector<Token> &tokens, int &ptr);
};

#endif