#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "classes.h"
#include"node.h"

enum ExprType {
  unknown, block_expr, const_block_expr, infinite_loop_expr, predicate_loop_expr,
  if_expr, literal_expr, path_in_expr, operator_expr, grouped_expr, array_expr,
  index_expr, struct_expr, call_expr, method_call_expr, field_expr, continue_expr,
  break_expr, return_expr, lazy_boolean_expr, assignment_expr, compound_assignment_expr,
  prefix_expr, call_params
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
  bitwise_xor_assign, left_shift_assign, right_shift_assign, brackets_closure,
  small_brackets_closure
};

class LiteralExpression final : public LeafNode {
public:
  LiteralExpression(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class BlockExpression final : public Node {
public:
  BlockExpression(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class Expression final : public Node {
public:
  Expression(const std::vector<Token> &tokens, int &ptr, ExprType expr_type, double min_bp);
  [[nodiscard]] ExprType GetNextExprType() const;
  [[nodiscard]] ExprType GetExprType() const;
  [[nodiscard]] Infix GetInfixForTest() const;
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  Expression(const std::vector<Token> &tokens, int &ptr, Expression *lhs, Expression *rhs, Infix infix);
  void Accept(Visitor *visitor) override;
  ExprType expr_type_;
  Infix infix_ = not_infix;
};

class StructExprField final : public Node {
public:
  StructExprField(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

class StructExprFields final : public Node {
public:
  StructExprFields(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
private:
  void Accept(Visitor *visitor) override;
};

#endif