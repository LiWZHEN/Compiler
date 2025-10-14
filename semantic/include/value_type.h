#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include "visitor_frame.h"

class ValueTypeVisitor final : public Visitor {
public:
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
  void Visit(ReferencePattern *reference_pattern_ptr) override;
  void Visit(IdentifierPattern *identifier_pattern_ptr) override;
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
private:
  bool is_reading_type_ = false;
  Node *type_owner_ = nullptr;
  std::vector<ScopeNodeContent> wrapping_structs_;
};

#endif