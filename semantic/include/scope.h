#ifndef VISITOR_H
#define VISITOR_H

#include "classes.h"
#include <unordered_map>
#include <string>
#include "node.h"

struct ScopeNodeContent {
  bool is_const;
  Node *node;
  NodeType node_type;
};

struct ScopeNode {
  std::shared_ptr<ScopeNode> parent;
  std::unordered_map<std::string, ScopeNodeContent> type_namespace;
  std::unordered_map<std::string, ScopeNodeContent> value_namespace;
  explicit ScopeNode(const std::shared_ptr<ScopeNode> &parent) : parent(parent) {}
  void TypeAdd(const std::string &name, const NodeType node_type, Node *node = nullptr, const bool is_const = false) {
    if (type_namespace.contains(name)) {
      std::cerr << "The name \"" << name << "\" is already in the type namespace.\n";
      throw;
    }
    type_namespace[name] = {is_const, node, node_type};
  }
  void ValueAdd(const std::string &name, const NodeType node_type, Node *node = nullptr, const bool is_constant = false) {
    if (value_namespace.contains(name)) {
      std::cerr << "The name \"" << name << "\" is already in the value namespace.\n";
    }
    value_namespace[name] = {is_constant, node, node_type};
  }
};

class Visitor {
public:
  virtual ~Visitor() = default;
  virtual void Visit(Crate *crate_ptr) = 0;
  virtual void Visit(Item *item_ptr) = 0;
  virtual void Visit(Function *function_ptr) = 0;
  virtual void Visit(Struct *struct_ptr) = 0;
  virtual void Visit(Enumeration *enumeration_ptr) = 0;
  virtual void Visit(ConstantItem *constant_item_ptr) = 0;
  virtual void Visit(Trait *trait_ptr) = 0;
  virtual void Visit(Implementation *implementation_ptr) = 0;
  virtual void Visit(Keyword *keyword_ptr) = 0;
  virtual void Visit(Identifier *identifier_ptr) = 0;
  virtual void Visit(Punctuation *punctuation_ptr) = 0;
  virtual void Visit(FunctionParameters *function_parameters_ptr) = 0;
  virtual void Visit(FunctionReturnType *function_return_type_ptr) = 0;
  virtual void Visit(BlockExpression *block_expression_ptr) = 0;
  virtual void Visit(SelfParam *self_param_ptr) = 0;
  virtual void Visit(FunctionParam *function_param_ptr) = 0;
  virtual void Visit(ShorthandSelf *shorthand_self_ptr) = 0;
  virtual void Visit(Type *type_ptr) = 0;
  virtual void Visit(Pattern *pattern_ptr) = 0;
  virtual void Visit(WildcardPattern *wildcard_pattern_ptr) = 0;
  virtual void Visit(ReferencePattern *reference_pattern_ptr) = 0;
  virtual void Visit(IdentifierPattern *identifier_pattern_ptr) = 0;
  virtual void Visit(LiteralPattern *literal_pattern_ptr) = 0;
  virtual void Visit(PathInExpression *path_in_expression_ptr) = 0;
  virtual void Visit(LiteralExpression *literal_expression_ptr) = 0;
  virtual void Visit(PathExprSegment *path_expr_segment_ptr) = 0;
  virtual void Visit(ReferenceType *reference_type_ptr) = 0;
  virtual void Visit(ArrayType *array_type_ptr) = 0;
  virtual void Visit(TypePath *type_path_ptr) = 0;
  virtual void Visit(UnitType *unit_type_ptr) = 0;
  virtual void Visit(Expression *expression_ptr) = 0;
  virtual void Visit(Statements *statements_ptr) = 0;
  virtual void Visit(Statement *statement_ptr) = 0;
  virtual void Visit(LetStatement *let_statement_ptr) = 0;
  virtual void Visit(ExpressionStatement *expression_statement_ptr) = 0;
  virtual void Visit(StructExprFields *struct_expr_fields_ptr) = 0;
  virtual void Visit(StructExprField *struct_expr_field_ptr) = 0;
  virtual void Visit(CharLiteral *char_literal_ptr) = 0;
  virtual void Visit(StringLiteral *string_literal_ptr) = 0;
  virtual void Visit(RawStringLiteral *raw_string_literal_ptr) = 0;
  virtual void Visit(CStringLiteral *c_string_literal_ptr) = 0;
  virtual void Visit(RawCStringLiteral *raw_c_string_literal_ptr) = 0;
  virtual void Visit(IntegerLiteral *integer_literal_ptr) = 0;
  virtual void Visit(StructFields *struct_fields_ptr) = 0;
  virtual void Visit(StructField *struct_field_ptr) = 0;
  virtual void Visit(EnumVariants *enum_variants_ptr) = 0;
  virtual void Visit(AssociatedItem *associated_item_ptr) = 0;
};

class SymbolVisitor final : public Visitor {
  void Visit(Crate *crate_ptr) override;
  void Visit(Item *item_ptr) override;
  void Visit(Function *function_ptr) override;
  void Visit(Struct *struct_ptr) override;
  void Visit(Enumeration *enumeration_ptr) override;
  void Visit(ConstantItem *constant_item_ptr) override;
  void Visit(Trait *trait_ptr) override;
  void Visit(Implementation *implementation_ptr) override;
  void Visit(Keyword *keyword_ptr) override;
  void Visit(Identifier *identifier_ptr) override;
  void Visit(Punctuation *punctuation_ptr) override;
  void Visit(FunctionParameters *function_parameters_ptr) override;
  void Visit(FunctionReturnType *function_return_type_ptr) override;
  void Visit(BlockExpression *block_expression_ptr) override;
  void Visit(SelfParam *self_param_ptr) override;
  void Visit(FunctionParam *function_param_ptr) override;
  void Visit(ShorthandSelf *shorthand_self_ptr) override;
  void Visit(Type *type_ptr) override;
  void Visit(Pattern *pattern_ptr) override;
  void Visit(WildcardPattern *wildcard_pattern_ptr) override;
  void Visit(ReferencePattern *reference_pattern_ptr) override;
  void Visit(IdentifierPattern *identifier_pattern_ptr) override;
  void Visit(LiteralPattern *literal_pattern_ptr) override;
  void Visit(PathInExpression *path_in_expression_ptr) override;
  void Visit(LiteralExpression *literal_expression_ptr) override;
  void Visit(PathExprSegment *path_expr_segment_ptr) override;
  void Visit(ReferenceType *reference_type_ptr) override;
  void Visit(ArrayType *array_type_ptr) override;
  void Visit(TypePath *type_path_ptr) override;
  void Visit(UnitType *unit_type_ptr) override;
  void Visit(Expression *expression_ptr) override;
  void Visit(Statements *statements_ptr) override;
  void Visit(Statement *statement_ptr) override;
  void Visit(LetStatement *let_statement_ptr) override;
  void Visit(ExpressionStatement *expression_statement_ptr) override;
  void Visit(StructExprFields *struct_expr_fields_ptr) override;
  void Visit(StructExprField *struct_expr_field_ptr) override;
  void Visit(CharLiteral *char_literal_ptr) override;
  void Visit(StringLiteral *string_literal_ptr) override;
  void Visit(RawStringLiteral *raw_string_literal_ptr) override;
  void Visit(CStringLiteral *c_string_literal_ptr) override;
  void Visit(RawCStringLiteral *raw_c_string_literal_ptr) override;
  void Visit(IntegerLiteral *integer_literal_ptr) override;
  void Visit(StructFields *struct_fields_ptr) override;
  void Visit(StructField *struct_field_ptr) override;
  void Visit(EnumVariants *enum_variants_ptr) override;
  void Visit(AssociatedItem *associated_item_ptr) override;
public:
  void SetCurrentScope(const std::shared_ptr<ScopeNode> &new_current_scope_node);
private:
  std::shared_ptr<ScopeNode> current_scope_node_;
};

#endif