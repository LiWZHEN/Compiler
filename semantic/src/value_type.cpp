#include "value_type.h"

void ValueTypeVisitor::Visit(Crate *crate_ptr) {
  
}
void ValueTypeVisitor::Visit(Item *item_ptr){}
void ValueTypeVisitor::Visit(Function *function_ptr){}
void ValueTypeVisitor::Visit(Struct *struct_ptr){}
void ValueTypeVisitor::Visit(Enumeration *enumeration_ptr){}
void ValueTypeVisitor::Visit(ConstantItem *constant_item_ptr){}
void ValueTypeVisitor::Visit(Trait *trait_ptr){}
void ValueTypeVisitor::Visit(Implementation *implementation_ptr){}
void ValueTypeVisitor::Visit(Keyword *keyword_ptr){}
void ValueTypeVisitor::Visit(Identifier *identifier_ptr){}
void ValueTypeVisitor::Visit(Punctuation *punctuation_ptr){}
void ValueTypeVisitor::Visit(FunctionParameters *function_parameters_ptr){}
void ValueTypeVisitor::Visit(FunctionReturnType *function_return_type_ptr){}
void ValueTypeVisitor::Visit(BlockExpression *block_expression_ptr){}
void ValueTypeVisitor::Visit(SelfParam *self_param_ptr){}
void ValueTypeVisitor::Visit(FunctionParam *function_param_ptr){}
void ValueTypeVisitor::Visit(ShorthandSelf *shorthand_self_ptr){}
void ValueTypeVisitor::Visit(Type *type_ptr){}
void ValueTypeVisitor::Visit(Pattern *pattern_ptr){}
void ValueTypeVisitor::Visit(ReferencePattern *reference_pattern_ptr){}
void ValueTypeVisitor::Visit(IdentifierPattern *identifier_pattern_ptr){}
void ValueTypeVisitor::Visit(LiteralPattern *literal_pattern_ptr){}
void ValueTypeVisitor::Visit(PathInExpression *path_in_expression_ptr){}
void ValueTypeVisitor::Visit(LiteralExpression *literal_expression_ptr){}
void ValueTypeVisitor::Visit(PathExprSegment *path_expr_segment_ptr){}
void ValueTypeVisitor::Visit(ReferenceType *reference_type_ptr){}
void ValueTypeVisitor::Visit(ArrayType *array_type_ptr){}
void ValueTypeVisitor::Visit(TypePath *type_path_ptr){}
void ValueTypeVisitor::Visit(UnitType *unit_type_ptr){}
void ValueTypeVisitor::Visit(Expression *expression_ptr){}
void ValueTypeVisitor::Visit(Statements *statements_ptr){}
void ValueTypeVisitor::Visit(Statement *statement_ptr){}
void ValueTypeVisitor::Visit(LetStatement *let_statement_ptr){}
void ValueTypeVisitor::Visit(ExpressionStatement *expression_statement_ptr){}
void ValueTypeVisitor::Visit(StructExprFields *struct_expr_fields_ptr){}
void ValueTypeVisitor::Visit(StructExprField *struct_expr_field_ptr){}
void ValueTypeVisitor::Visit(CharLiteral *char_literal_ptr){}
void ValueTypeVisitor::Visit(StringLiteral *string_literal_ptr){}
void ValueTypeVisitor::Visit(RawStringLiteral *raw_string_literal_ptr){}
void ValueTypeVisitor::Visit(CStringLiteral *c_string_literal_ptr){}
void ValueTypeVisitor::Visit(RawCStringLiteral *raw_c_string_literal_ptr){}
void ValueTypeVisitor::Visit(IntegerLiteral *integer_literal_ptr){}
void ValueTypeVisitor::Visit(StructFields *struct_fields_ptr){}
void ValueTypeVisitor::Visit(StructField *struct_field_ptr){}
void ValueTypeVisitor::Visit(EnumVariants *enum_variants_ptr){}
void ValueTypeVisitor::Visit(AssociatedItem *associated_item_ptr){}