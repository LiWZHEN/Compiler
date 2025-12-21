#include "IR_generator.h"
#include "item.h"
#include "expression.h"
#include "statements.h"

void Throw(const std::string &err_info) {
  std::cerr << "[IR Error] " << err_info << '\n';
  throw "";
}

void IRVisitor::AddFunction() {
  functions_.push_back(IRFunctionNode());
}
void IRVisitor::AddStruct() {
  structs_.push_back(IRStructNode());
}

void IRVisitor::Visit(Trait *trait_ptr) {}
void IRVisitor::Visit(Implementation *implementation_ptr) {}
void IRVisitor::Visit(Enumeration *enumeration_ptr) {}
void IRVisitor::Visit(Keyword *keyword_ptr) {}
void IRVisitor::Visit(Identifier *identifier_ptr) {}
void IRVisitor::Visit(Punctuation *punctuation_ptr) {}
void IRVisitor::Visit(FunctionReturnType *function_return_type_ptr) {}
void IRVisitor::Visit(RawStringLiteral *raw_string_literal_ptr) {}
void IRVisitor::Visit(CStringLiteral *c_string_literal_ptr) {}
void IRVisitor::Visit(RawCStringLiteral *raw_c_string_literal_ptr) {}
void IRVisitor::Visit(StructFields *struct_fields_ptr) {}
void IRVisitor::Visit(StructField *struct_field_ptr) {}
void IRVisitor::Visit(EnumVariants *enum_variants_ptr) {}
void IRVisitor::Visit(AssociatedItem *associated_item_ptr) {}
void IRVisitor::Visit(Type *type_ptr) {}
void IRVisitor::Visit(Pattern *pattern_ptr) {}
void IRVisitor::Visit(ReferencePattern *reference_pattern_ptr) {}
void IRVisitor::Visit(IdentifierPattern *identifier_pattern_ptr) {}
void IRVisitor::Visit(ReferenceType *reference_type_ptr) {}
void IRVisitor::Visit(ArrayType *array_type_ptr) {}
void IRVisitor::Visit(UnitType *unit_type_ptr) {}
void IRVisitor::Visit(FunctionParameters *function_parameters_ptr) {}
void IRVisitor::Visit(SelfParam *self_param_ptr) {}
void IRVisitor::Visit(FunctionParam *function_param_ptr) {}
void IRVisitor::Visit(ShorthandSelf *shorthand_self_ptr) {}
void IRVisitor::Visit(ConstantItem *constant_item_ptr) {}
void IRVisitor::Visit(TypePath *type_path_ptr) {}
void IRVisitor::Visit(PathInExpression *path_in_expression_ptr) {}
void IRVisitor::Visit(PathExprSegment *path_expr_segment_ptr) {}
void IRVisitor::Visit(StructExprFields *struct_expr_fields_ptr) {}
void IRVisitor::Visit(StructExprField *struct_expr_field_ptr) {}
void IRVisitor::Visit(CharLiteral *char_literal_ptr) {}
void IRVisitor::Visit(IntegerLiteral *integer_literal_ptr) {}
void IRVisitor::Visit(Crate *crate_ptr) {
  auto scope_ptr_ = crate_ptr->scope_node_;
  // collect and declare the structs and functions
  for (const auto &it : scope_ptr_->type_namespace) {
    if (it.second.node_type == type_struct) {
      it.second.node->IR_ID_ = static_cast<int>(structs_.size());
      AddStruct();
      for (const auto &associated_item :
          dynamic_cast<Struct *>(it.second.node)->associated_items_) {
        if (associated_item.second.node_type == type_function) {
          associated_item.second.node->IR_ID_ = static_cast<int>(functions_.size());
          AddFunction();
        }
      }
    } else if (it.second.node_type == type_function) {
      it.second.node->IR_ID_ = static_cast<int>(functions_.size());
      AddFunction();
    }
  }
  for (const auto &it : crate_ptr->children_) {
    if (it->type_[0] == struct_type) {
      it->Accept(this);
    }
  }
  for (const auto &it : crate_ptr->children_) {
    it->Accept(this);
  }
}
void IRVisitor::Visit(Item *item_ptr) {
  item_ptr->children_[0]->Accept(this);
}
void IRVisitor::Visit(Struct *struct_ptr) {
  auto &struct_elements = structs_[struct_ptr->IR_ID_].element_type_;
  if (struct_elements.empty()) {
    for (const auto &it : struct_ptr->field_items_) {
      const auto element_type = it.second.node->integrated_type_;
      const int element_index = static_cast<int>(struct_elements.size());
      struct_elements.push_back(element_type);
      struct_ptr->field_item_index_[it.first] = element_index;
    }
  } else {
    for (const auto &associated_item :
      struct_ptr->associated_items_) {
      if (associated_item.second.node_type == type_function) {
        associated_item.second.node->Accept(this);
      }
    }
  }
}
void IRVisitor::Visit(Function *function_ptr) {
  const int function_id = function_ptr->IR_ID_;
  wrapping_functions_.push_back(function_id);
  block_stack_.push_back(0);
  for (int i = 0; i < function_ptr->children_.size(); ++i) {
    if (function_ptr->type_[i] == type_function_parameters) {
      auto &function_parameter_types = functions_[function_id].parameter_types_;
      for (int j = 0; j < function_ptr->children_[i]->children_.size(); ++j) {
        if (function_ptr->children_[i]->type_[j] == type_self_param
            || function_ptr->children_[i]->type_[j] == type_function_param) {
          function_ptr->children_[i]->children_[j]->IR_ID_ = static_cast<int>(function_parameter_types.size());
          function_parameter_types.push_back(function_ptr->children_[i]->children_[j]->integrated_type_);
        }
      }
    } else if (function_ptr->type_[i] == type_block_expression) {
      auto scope_ptr_ = function_ptr->children_[i]->scope_node_;
      // collect and declare the struct and function
      for (const auto &it : scope_ptr_->type_namespace) {
        if (it.second.node_type == type_struct) {
          it.second.node->IR_ID_ = static_cast<int>(structs_.size());
          AddStruct();
          for (const auto &associated_item :
              dynamic_cast<Struct *>(it.second.node)->associated_items_) {
            if (associated_item.second.node_type == type_function) {
              associated_item.second.node->IR_ID_ = static_cast<int>(functions_.size());
              AddFunction();
            }
              }
        } else if (it.second.node_type == type_function) {
          it.second.node->IR_ID_ = static_cast<int>(functions_.size());
          AddFunction();
        }
      }
      function_ptr->children_[i]->Accept(this);
    }
  }
  wrapping_functions_.pop_back();
  block_stack_.pop_back();
}
void IRVisitor::Visit(BlockExpression *block_expression_ptr) {
  if (block_expression_ptr->type_[1] == type_statements) {
    block_expression_ptr->children_[1]->Accept(this);
  }
}
void IRVisitor::Visit(Statements *statements_ptr) {
  for (int i = 0; i < statements_ptr->children_.size(); ++i) {
    if (statements_ptr->type_[i] == type_statement &&
        statements_ptr->children_[i]->type_[0] == type_item &&
        statements_ptr->children_[i]->children_[0]->type_[0] == type_struct) {
      statements_ptr->children_[i]->Accept(this);
    }
  }
  for (const auto &it : statements_ptr->children_) {
    it->Accept(this);
  }
}
void IRVisitor::Visit(Statement *statement_ptr) {
  statement_ptr->children_[0]->Accept(this);
}
void IRVisitor::Visit(LetStatement *let_statement_ptr) {
  auto &function = functions_[wrapping_functions_.back()];
  if (function.blocks_.empty()) {
    function.blocks_[0] = IRBlock();
    ++function.var_id_;
  }
  const int new_alloca_id = function.var_id_;
  let_statement_ptr->children_[1]->IR_ID_ = new_alloca_id;
  function.AddAlloca(new_alloca_id, let_statement_ptr->children_[1]->integrated_type_);
  ++function.var_id_;
  if (let_statement_ptr->children_[5]->integrated_type_->is_const) {
    function.blocks_[block_stack_.back()].AddValueStore(let_statement_ptr->children_[1]->integrated_type_,
        static_cast<int>(let_statement_ptr->children_[5]->value_.int_value), new_alloca_id);
  } else {
    let_statement_ptr->children_[5]->Accept(this);
    function.blocks_[block_stack_.back()].AddVariableStore(let_statement_ptr->children_[1]->integrated_type_,
        let_statement_ptr->children_[5]->IR_ID_, new_alloca_id);
  }
}
void IRVisitor::Visit(ExpressionStatement *expression_statement_ptr) {
  if (!expression_statement_ptr->children_[0]->integrated_type_->is_const) {
    expression_statement_ptr->children_[0]->Accept(this);
  }
  expression_statement_ptr->IR_ID_ = -1;
}
void IRVisitor::Visit(StringLiteral *string_literal_ptr) {
  Throw("String is not implemented.");
}
void IRVisitor::Visit(Expression *expression_ptr) {
  switch (expression_ptr->GetExprType()) {
    case block_expr: {
      break;
    }
    case infinite_loop_expr: {
      break;
    }
    case predicate_loop_expr: {
      break;
    }
    case if_expr: {
      break;
    }
    case literal_expr: {
      break;
    }
    case path_in_expr: {
      break;
    }
    case grouped_expr: {
      break;
    }
    case array_expr: {
      break;
    }
    case index_expr: {
      break;
    }
    case struct_expr: {
      break;
    }
    case call_expr: {
      break;
    }
    case method_call_expr: {
      break;
    }
    case field_expr: {
      break;
    }
    case continue_expr: {
      break;
    }
    case break_expr: {
      break;
    }
    case return_expr: {
      break;
    }
    case prefix_expr: {
      break;
    }
    case call_params: {
      break;
    }
    default: { // operator_expr
      auto &function = functions_[wrapping_functions_.back()];
      auto &target_block = function.blocks_[block_stack_.back()];
      switch (expression_ptr->GetExprInfix()) {
        case add: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const + var
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, add_,
                result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var + const
            expression_ptr->children_[0]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                add_, result_id, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var + var
            expression_ptr->children_[0]->Accept(this);
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                add_, result_id, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case minus: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const - var
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, sub_,
                result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var - const
            expression_ptr->children_[0]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                sub_, result_id, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var - var
            expression_ptr->children_[0]->Accept(this);
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                sub_, result_id, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case multiply: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const * var
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, mul_,
                result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var * const
            expression_ptr->children_[0]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                mul_, result_id, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var * var
            expression_ptr->children_[0]->Accept(this);
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                mul_, result_id, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case divide: {
          if (expression_ptr->integrated_type_->basic_type == u32_type ||
              expression_ptr->integrated_type_->basic_type == usize_type) { // unsigned division
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const / var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, udiv_,
                  result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var / const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                  udiv_, result_id, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var / var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                  udiv_, result_id, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed division
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const / var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, sdiv_,
                  result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var / const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                  sdiv_, result_id, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var / var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                  sdiv_, result_id, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          }
          break;
        }
        case mod: {
          if (expression_ptr->integrated_type_->basic_type == u32_type ||
              expression_ptr->integrated_type_->basic_type == usize_type) { // unsigned rem
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const % var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, urem_,
                  result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var % const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                  urem_, result_id, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var % var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                  urem_, result_id, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed rem
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const % var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, srem_,
                  result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var % const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                  srem_, result_id, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var % var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                  srem_, result_id, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          }
          break;
        }
        case bitwise_and: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const & var
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, and_,
                result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var & const
            expression_ptr->children_[0]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                and_, result_id, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var & var
            expression_ptr->children_[0]->Accept(this);
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                and_, result_id, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case bitwise_or: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const | var
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, or_,
                result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var | const
            expression_ptr->children_[0]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                or_, result_id, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var | var
            expression_ptr->children_[0]->Accept(this);
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                or_, result_id, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case bitwise_xor: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const ^ var
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, xor_,
                result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var ^ const
            expression_ptr->children_[0]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                xor_, result_id, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var ^ var
            expression_ptr->children_[0]->Accept(this);
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                xor_, result_id, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case left_shift: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const << var
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, shl_,
                result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var << const
            expression_ptr->children_[0]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                shl_, result_id, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var << var
            expression_ptr->children_[0]->Accept(this);
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                shl_, result_id, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case right_shift: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const >> var
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddConstVarBinaryOperation(expression_ptr->integrated_type_, ashr_,
                result_id, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var >> const
            expression_ptr->children_[0]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                ashr_, result_id, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var >> var
            expression_ptr->children_[0]->Accept(this);
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                ashr_, result_id, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case is_equal: {
          // Enum comparison is between consts, the result has been evaluated in value_type
          // There is no comparison between structs and arrays in testcases.
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const == var
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddConstVarIcmp(result_id, equal_, expression_ptr->children_[0]->integrated_type_,
                static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var == const
            expression_ptr->children_[0]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddVarConstIcmp(result_id, equal_, expression_ptr->children_[0]->integrated_type_,
                expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var == var
            expression_ptr->children_[0]->Accept(this);
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddTwoVarIcmp(result_id, equal_, expression_ptr->children_[0]->integrated_type_,
                expression_ptr->children_[0]->IR_ID_, expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case is_not_equal: {
          // Enum comparison is between consts, the result has been evaluated in value_type
          // There is no comparison between structs and arrays in testcases.
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const != var
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddConstVarIcmp(result_id, not_equal_, expression_ptr->children_[0]->integrated_type_,
                static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var != const
            expression_ptr->children_[0]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddVarConstIcmp(result_id, not_equal_, expression_ptr->children_[0]->integrated_type_,
                expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var != var
            expression_ptr->children_[0]->Accept(this);
            expression_ptr->children_[1]->Accept(this);
            const int result_id = function.var_id_;
            expression_ptr->IR_ID_ = result_id;
            ++function.var_id_;
            target_block.AddTwoVarIcmp(result_id, not_equal_, expression_ptr->children_[0]->integrated_type_,
                expression_ptr->children_[0]->IR_ID_, expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case is_bigger: {
          // Enum comparison is between consts, the result has been evaluated in value_type
          // There is no comparison between structs and arrays in testcases.
          if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
              expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) { // unsigned greater than
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const > var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarIcmp(result_id, unsigned_greater_than_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var > const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstIcmp(result_id, unsigned_greater_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var > var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarIcmp(result_id, unsigned_greater_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed greater than
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const > var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarIcmp(result_id, signed_greater_than_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var > const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstIcmp(result_id, signed_greater_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var > var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarIcmp(result_id, signed_greater_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          }
          break;
        }
        case is_smaller: {
          // Enum comparison is between consts, the result has been evaluated in value_type
          // There is no comparison between structs and arrays in testcases.
          if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
              expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) { // unsigned less than
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const < var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarIcmp(result_id, unsigned_less_than_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var < const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstIcmp(result_id, unsigned_less_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var < var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarIcmp(result_id, unsigned_less_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed less than
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const < var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarIcmp(result_id, signed_less_than_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var < const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstIcmp(result_id, signed_less_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var < var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarIcmp(result_id, signed_less_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          }
          break;
        }
        case is_not_smaller: {
          // Enum comparison is between consts, the result has been evaluated in value_type
          // There is no comparison between structs and arrays in testcases.
          if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
              expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) { // unsigned greater equal
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const >= var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarIcmp(result_id, unsigned_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var >= const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstIcmp(result_id, unsigned_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var >= var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarIcmp(result_id, unsigned_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed greater equal
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const >= var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarIcmp(result_id, signed_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var >= const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstIcmp(result_id, signed_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var >= var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarIcmp(result_id, signed_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          }
          break;
        }
        case is_not_bigger: {
          // Enum comparison is between consts, the result has been evaluated in value_type
          // There is no comparison between structs and arrays in testcases.
          if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
              expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) { // unsigned less equal
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const <= var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarIcmp(result_id, unsigned_less_equal_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var <= const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstIcmp(result_id, unsigned_less_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var <= var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarIcmp(result_id, unsigned_less_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed less equal
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const <= var
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddConstVarIcmp(result_id, signed_less_equal_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var <= const
              expression_ptr->children_[0]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddVarConstIcmp(result_id, signed_less_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var <= var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              const int result_id = function.var_id_;
              expression_ptr->IR_ID_ = result_id;
              ++function.var_id_;
              target_block.AddTwoVarIcmp(result_id, signed_less_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          }
          break;
        }
        case logic_or: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) {
            if (expression_ptr->children_[0]->value_.int_value == 1) {
              // true || _ = true
              expression_ptr->IR_ID_ = function.var_id_++;
              target_block.AddSelect(0b111, expression_ptr->IR_ID_, 1,
                  expression_ptr->children_[0]->integrated_type_, 1,
                  expression_ptr->children_[0]->integrated_type_, 1);
            } else {
              // false || _ = _
              expression_ptr->children_[1]->Accept(this);
              expression_ptr->IR_ID_ = function.var_id_++;
              target_block.AddSelect(0b100, expression_ptr->IR_ID_, 1,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_);
            }
          } else {
            expression_ptr->children_[0]->Accept(this);
            // a || b : if a == true, expression = true; if a == false, expression = b.
            const int if_true_block_label = function.var_id_++;
            function.blocks_[if_true_block_label] = IRBlock();
            const int if_false_block_label = function.var_id_++;
            function.blocks_[if_false_block_label] = IRBlock();
            const int merged_label = function.var_id_++;
            function.blocks_[merged_label] = IRBlock();
            target_block.AddConditionalBranch(expression_ptr->children_[0]->IR_ID_,
                if_true_block_label, if_false_block_label);
            // in if_true_block
            const int true_block_ans_id = function.var_id_++;
            function.blocks_[if_true_block_label].AddSelect(0b111, true_block_ans_id, 1,
                expression_ptr->children_[0]->integrated_type_, 1,
                expression_ptr->children_[0]->integrated_type_, 1);
            function.blocks_[if_true_block_label].AddUnconditionalBranch(merged_label);
            // in if_false_block
            block_stack_.back() = if_false_block_label;
            const int false_block_ans_id = function.var_id_++;
            if (expression_ptr->children_[1]->integrated_type_->is_const) {
              if (expression_ptr->children_[1]->value_.int_value == 0) {
                function.blocks_[if_false_block_label].AddSelect(0b111, false_block_ans_id, 1,
                    expression_ptr->children_[1]->integrated_type_, 0,
                    expression_ptr->children_[1]->integrated_type_, 0);
              } else {
                function.blocks_[if_false_block_label].AddSelect(0b111, false_block_ans_id, 1,
                    expression_ptr->children_[1]->integrated_type_, 1,
                    expression_ptr->children_[1]->integrated_type_, 1);
              }
            } else {
              expression_ptr->children_[1]->Accept(this);
              function.blocks_[if_false_block_label].AddSelect(0b011, false_block_ans_id,
                  expression_ptr->children_[1]->IR_ID_, expression_ptr->children_[1]->integrated_type_,
                  1, expression_ptr->children_[1]->integrated_type_, 0);
            }
            function.blocks_[if_false_block_label].AddUnconditionalBranch(merged_label);
            // merge
            block_stack_.back() = merged_label;
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[merged_label].AddSelect(0b000, expression_ptr->IR_ID_,
                expression_ptr->children_[0]->IR_ID_, expression_ptr->children_[1]->integrated_type_,
                true_block_ans_id, expression_ptr->children_[1]->integrated_type_,
                false_block_ans_id);
          }
          break;
        }
        case logic_and: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) {
            if (expression_ptr->children_[0]->value_.int_value == 1) {
              // true && _ = _
              expression_ptr->children_[1]->Accept(this);
              expression_ptr->IR_ID_ = function.var_id_++;
              target_block.AddSelect(0b100, expression_ptr->IR_ID_, 1,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_);
            } else {
              // false && _ = false
              expression_ptr->IR_ID_ = function.var_id_++;
              target_block.AddSelect(0b111, expression_ptr->IR_ID_, 1,
                  expression_ptr->children_[0]->integrated_type_, 0,
                  expression_ptr->children_[0]->integrated_type_, 0);
            }
          } else {
            expression_ptr->children_[0]->Accept(this);
            // a && b : if a == true, expression = b; if a == false, expression = false.
            const int if_true_block_label = function.var_id_++;
            function.blocks_[if_true_block_label] = IRBlock();
            const int if_false_block_label = function.var_id_++;
            function.blocks_[if_false_block_label] = IRBlock();
            const int merged_label = function.var_id_++;
            function.blocks_[merged_label] = IRBlock();
            target_block.AddConditionalBranch(expression_ptr->children_[0]->IR_ID_,
                if_true_block_label, if_false_block_label);
            // in if_true_block
            block_stack_.back() = if_true_block_label;
            const int true_block_ans_id = function.var_id_++;
            if (expression_ptr->children_[1]->integrated_type_->is_const) {
              if (expression_ptr->children_[1]->value_.int_value == 0) {
                function.blocks_[if_true_block_label].AddSelect(0b111, true_block_ans_id, 1,
                    expression_ptr->children_[1]->integrated_type_, 0,
                    expression_ptr->children_[1]->integrated_type_, 0);
              } else {
                function.blocks_[if_true_block_label].AddSelect(0b111, true_block_ans_id, 1,
                    expression_ptr->children_[1]->integrated_type_, 1,
                    expression_ptr->children_[1]->integrated_type_, 1);
              }
            } else {
              expression_ptr->children_[1]->Accept(this);
              function.blocks_[if_true_block_label].AddSelect(0b011, true_block_ans_id,
                  expression_ptr->children_[1]->IR_ID_, expression_ptr->children_[1]->integrated_type_,
                  1, expression_ptr->children_[1]->integrated_type_, 0);
            }
            function.blocks_[if_true_block_label].AddUnconditionalBranch(merged_label);
            // in if_false_block
            const int false_block_ans_id = function.var_id_++;
            function.blocks_[if_false_block_label].AddSelect(0b111, false_block_ans_id, 1,
                expression_ptr->children_[0]->integrated_type_, 0,
                expression_ptr->children_[0]->integrated_type_, 0);
            function.blocks_[if_false_block_label].AddUnconditionalBranch(merged_label);
            // merge
            block_stack_.back() = merged_label;
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[merged_label].AddSelect(0b000, expression_ptr->IR_ID_,
                expression_ptr->children_[0]->IR_ID_, expression_ptr->children_[1]->integrated_type_,
                true_block_ans_id, expression_ptr->children_[1]->integrated_type_,
                false_block_ans_id);
          }
          break;
        }
        case assign: {
          const int variable_id = expression_ptr->GetDefInfo().node->IR_ID_;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            target_block.AddValueStore(expression_ptr->children_[0]->integrated_type_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value), variable_id);
          } else {
            expression_ptr->children_[1]->Accept(this);
            target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
                expression_ptr->children_[1]->IR_ID_, variable_id);
          }
          break;
        }
        case add_assign: {
          const int pointer_id = dynamic_cast<Expression *>(expression_ptr->children_[0])
              ->GetDefInfo().node->IR_ID_;
          const int loaded_var_id = function.var_id_++;
          target_block.AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                add_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                add_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case minus_assign: {
          const int pointer_id = dynamic_cast<Expression *>(expression_ptr->children_[0])
              ->GetDefInfo().node->IR_ID_;
          const int loaded_var_id = function.var_id_++;
          target_block.AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                sub_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                sub_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case multiply_assign: {
          const int pointer_id = dynamic_cast<Expression *>(expression_ptr->children_[0])
              ->GetDefInfo().node->IR_ID_;
          const int loaded_var_id = function.var_id_++;
          target_block.AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                mul_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                mul_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case divide_assign: {
          const int pointer_id = dynamic_cast<Expression *>(expression_ptr->children_[0])
              ->GetDefInfo().node->IR_ID_;
          const int loaded_var_id = function.var_id_++;
          target_block.AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
                expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) {
              target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                  udiv_, temp_result_id, loaded_var_id,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else {
              target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                sdiv_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            }
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
                expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) {
              target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                udiv_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
            } else {
              target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                sdiv_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
            }
          }
          target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case mod_assign: {
          const int pointer_id = dynamic_cast<Expression *>(expression_ptr->children_[0])
              ->GetDefInfo().node->IR_ID_;
          const int loaded_var_id = function.var_id_++;
          target_block.AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
                expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) {
              target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                  urem_, temp_result_id, loaded_var_id,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else {
              target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                srem_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            }
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
                expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) {
              target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                urem_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
            } else {
              target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                srem_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
            }
          }
          target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case bitwise_and_assign: {
          const int pointer_id = dynamic_cast<Expression *>(expression_ptr->children_[0])
              ->GetDefInfo().node->IR_ID_;
          const int loaded_var_id = function.var_id_++;
          target_block.AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                and_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                and_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case bitwise_or_assign: {
          const int pointer_id = dynamic_cast<Expression *>(expression_ptr->children_[0])
              ->GetDefInfo().node->IR_ID_;
          const int loaded_var_id = function.var_id_++;
          target_block.AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                or_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                or_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case bitwise_xor_assign: {
          const int pointer_id = dynamic_cast<Expression *>(expression_ptr->children_[0])
              ->GetDefInfo().node->IR_ID_;
          const int loaded_var_id = function.var_id_++;
          target_block.AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                xor_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                xor_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case left_shift_assign: {
          const int pointer_id = dynamic_cast<Expression *>(expression_ptr->children_[0])
              ->GetDefInfo().node->IR_ID_;
          const int loaded_var_id = function.var_id_++;
          target_block.AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                shl_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                shl_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case right_shift_assign: {
          const int pointer_id = dynamic_cast<Expression *>(expression_ptr->children_[0])
              ->GetDefInfo().node->IR_ID_;
          const int loaded_var_id = function.var_id_++;
          target_block.AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            target_block.AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                ashr_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            target_block.AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                ashr_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case type_cast: {
          expression_ptr->IR_ID_ = function.var_id_++;
          target_block.AddSelect(0b100, expression_ptr->IR_ID_, 1,
              expression_ptr->integrated_type_, expression_ptr->children_[0]->IR_ID_,
              expression_ptr->integrated_type_, expression_ptr->children_[0]->IR_ID_);
          break;
        }
        default:;
      }
    }
  }
}

void IRVisitor::Output() {

}