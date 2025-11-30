#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "visitor_frame.h"
#include "node.h"

enum IRInstructionType {
  unknown_, binary_operations_, conditional_br_, unconditional_br_,
  non_void_ret_, void_ret_, alloca_, load_, store_, getelementptr_,
  icmp_, non_void_call_, void_call_, phi_, select_
};

enum BinaryOperator {
  add_, sub_, mul_, sdiv_, srem_, shl_, ashr_, and_, or_, xor_
};

enum IcmpCond {
  equal_, not_equal_, unsigned_greater_than_, unsigned_greater_equal_,
  unsigned_less_than_, unsigned_less_equal_, signed_greater_than_,
  signed_greater_equal_, signed_less_than_, signed_less_equal_
};

struct FunctionCallArgument {
  IntegratedType type_;
  int value_id_ = 0;
};

struct IRInstruction {
  IRInstructionType instruction_type_ = unknown_;
  int result_id_ = 0; // binary operations & non-void return & alloca
                      // & load & store & get element ptr & icmp
                      // & non-void call
  BinaryOperator operator_ = add_; // binary operations
  IntegratedType result_type_; // binary operations & non-void return
                               // & alloca & load & store & get element ptr
                               // & icmp & non-void call
  int operand_1_id_ = 0, operand_2_id_ = 0; // binary operations & icmp &
  int condition_id_ = 0; // conditional branch
  int if_true_ = 0, if_false_ = 0; // conditional branch
  int destination_ = 0; // unconditional branch
  int pointer_ = 0; // load & store & get element ptr
  std::vector<int> indexes_; // get element ptr
  IcmpCond icmp_condition_ = equal_; // icmp
  int function_name_ = 0; // non-void call & void call
  std::vector<FunctionCallArgument> function_call_arguments_; // non-void call & void call
};

struct IRBlock {
  int block_id_;
  std::vector<IRInstruction> instructions_;
};

struct IRFunctionNode {
  int function_id_;
  std::vector<IRBlock> blocks_;
};

struct IRStructNode {
  int struct_id_;
  std::vector<IntegratedType> element_type_;
};

class IRVisitor final : public Visitor {
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
  std::vector<IRFunctionNode> functions_;
  std::vector<IRStructNode> structs_;
};

#endif