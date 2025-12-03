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
  std::shared_ptr<IntegratedType> type_;
  int value_id_ = 0;
};

struct IRInstruction {
  IRInstructionType instruction_type_ = unknown_;
  int result_id_ = 0; // binary operations & non-void return & alloca
                      // & load & store & get element ptr & icmp
                      // & non-void call
  BinaryOperator operator_ = add_; // binary operations
  std::shared_ptr<IntegratedType> result_type_; // binary operations & non-void return
                                                // & alloca & load & store & get element ptr
                                                // & icmp & non-void call & select
  std::shared_ptr<IntegratedType> another_type_; // select
  int operand_1_id_ = 0, operand_2_id_ = 0; // binary operations & icmp & select
  int condition_id_ = 0; // conditional branch & select
  int if_true_ = 0, if_false_ = 0; // conditional branch
  int destination_ = 0; // unconditional branch
  int pointer_ = 0; // load & store & get element ptr
  std::vector<int> indexes_; // get element ptr
  IcmpCond icmp_condition_ = equal_; // icmp
  int function_name_ = 0; // non-void call & void call
  std::vector<FunctionCallArgument> function_call_arguments_; // non-void call & void call
  IRInstruction(const IRInstructionType instruction_type, const int result_id, const BinaryOperator binary_operator,
      const std::shared_ptr<IntegratedType> &result_type, const int operand_1_id, const int operand_2_id,
      const int condition_id, const int if_true, const int if_false, const int destination, const int pointer,
      const IcmpCond icmp_condition, const int function_name, const std::shared_ptr<IntegratedType> &another_type = nullptr) : instruction_type_(instruction_type),
      result_id_(result_id), operator_(binary_operator), result_type_(result_type), another_type_(another_type),
      operand_1_id_(operand_1_id), operand_2_id_(operand_2_id), condition_id_(condition_id), if_true_(if_true),
      if_false_(if_false), destination_(destination), pointer_(pointer), icmp_condition_(icmp_condition),
      function_name_(function_name) {}
};

struct IRBlock {
  std::vector<IRInstruction> instructions_;
  void AddBinaryOperation(const std::shared_ptr<IntegratedType> &result_type, const BinaryOperator binary_operator,
      const int result_id, const int operand_1_id, const int operand_2_id) {
    instructions_.push_back(IRInstruction(binary_operations_, result_id, binary_operator,
        result_type, operand_1_id, operand_2_id, 0, 0, 0, 0,
        0, equal_, 0));
  }
  void AddConditionalBranch(const int condition_id, const int if_true_label, const int if_false_label) {
    instructions_.push_back(IRInstruction(conditional_br_, 0, add_, nullptr,
        0, 0, condition_id, if_true_label, if_false_label, 0, 0,
        equal_, 0));
  }
  void AddUnconditionalBranch(const int destination_label) {
    instructions_.push_back(IRInstruction(unconditional_br_, 0, add_, nullptr,
        0, 0, 0, 0, 0, destination_label, 0, equal_,
        0));
  }
  void AddNonVoidReturn(const std::shared_ptr<IntegratedType> &return_type, const int value_id) {
    instructions_.push_back(IRInstruction(non_void_ret_, value_id, add_, return_type,
        0, 0, 0, 0, 0, 0, 0, equal_,
        0));
  }
  void AddVoidReturn() {
    instructions_.push_back(IRInstruction(void_ret_, 0, add_, nullptr,
        0, 0, 0, 0, 0, 0, 0, equal_,
        0));
  }
  void AddAlloca(const int result_id, const std::shared_ptr<IntegratedType> &type) {
    instructions_.push_back(IRInstruction(alloca_, result_id, add_, type, 0,
        0, 0, 0, 0, 0, 0, equal_,
        0));
  }
  void AddLoad(const int result_id, const std::shared_ptr<IntegratedType> &type, const int pointer_id) {
    instructions_.push_back(IRInstruction(load_, result_id, add_, type, 0,
        0, 0, 0, 0, 0, pointer_id, equal_,
        0));
  }
  void AddStore(const std::shared_ptr<IntegratedType> &type, const int value_id, const int pointer_id) {
    instructions_.push_back(IRInstruction(store_, value_id, add_, type, 0,
        0, 0, 0, 0, 0, pointer_id, equal_,
        0));
  }
  void AddGetElementPtr(const int result_id, const std::shared_ptr<IntegratedType> &type, const int ptr_id,
      const std::vector<int> &indexes = std::vector<int>()) {
    instructions_.push_back(IRInstruction(getelementptr_, result_id, add_, type, 0,
        0, 0, 0, 0, 0, 0, equal_,
        0));
    instructions_.back().indexes_ = indexes;
  }
  void AddIcmp(const int result_id, const IcmpCond condition, const std::shared_ptr<IntegratedType> &operand_type,
      const int operand_1_id, const int operand_2_id) {
    instructions_.push_back(IRInstruction(icmp_, result_id, add_, operand_type, operand_1_id,
        operand_2_id, 0, 0, 0, 0, 0, condition, 0));
  }
  void AddNonVoidCall(const int result_id, const std::shared_ptr<IntegratedType> &result_type, const int function_id,
      const std::vector<FunctionCallArgument> &function_call_arguments = std::vector<FunctionCallArgument>()) {
    instructions_.push_back(IRInstruction(non_void_call_, result_id, add_, result_type,
        0, 0, 0, 0, 0, 0, 0, equal_,
        function_id));
    instructions_.back().function_call_arguments_ = function_call_arguments;
  }
  void AddVoidCall(const int function_id,
      const std::vector<FunctionCallArgument> &function_call_arguments = std::vector<FunctionCallArgument>()) {
    instructions_.push_back(IRInstruction(void_call_, 0, add_, nullptr, 0,
        0, 0, 0, 0, 0, 0, equal_,
        function_id));
    instructions_.back().function_call_arguments_ = function_call_arguments;
  }
  void AddSelect(const int result_id, const int condition_id, const std::shared_ptr<IntegratedType> &first_type,
      const int first_value_id, const std::shared_ptr<IntegratedType> &second_type, const int second_value_id) {
    instructions_.push_back(IRInstruction(select_, result_id, add_, first_type,
        first_value_id, second_value_id, condition_id, 0, 0,
        0, 0, equal_, 0, second_type));
  }
};

struct IRFunctionNode {
  std::vector<IRBlock> blocks_;
  std::vector<std::shared_ptr<IntegratedType>> parameter_types_;
};

struct IRStructNode {
  std::vector<std::shared_ptr<IntegratedType>> element_type_;
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
  void AddFunction();
  void AddStruct();
private:
  std::vector<IRFunctionNode> functions_;
  std::vector<IRStructNode> structs_;
  std::vector<int> wrapping_functions_;
};

#endif