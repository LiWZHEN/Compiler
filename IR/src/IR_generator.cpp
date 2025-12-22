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

void IRVisitor::RecursiveInitialize(const Expression *expression_ptr, const int ptr_id) {
  auto &function = functions_[wrapping_functions_.back()];
  auto &target_block = function.blocks_[block_stack_.back()];
  const auto &integrated_type = expression_ptr->integrated_type_;
  if (integrated_type->basic_type == array_type) {
    for (int i = 0; i < integrated_type->size; ++i) {
      const int element_ptr_id = function.var_id_++;
      target_block.AddGetElementPtrByValue(element_ptr_id, integrated_type->element_type,
          ptr_id, i);
      const auto element_expr_ptr = dynamic_cast<Expression *>(expression_ptr->children_[2 * i + 1]);
      const auto basic_type = integrated_type->element_type->basic_type;
      // %element_ptr_id <- *element_expr_ptr
      if (basic_type == char_type || basic_type == str_type || basic_type == string_type) {
        Throw("Invalid type in IR.");
      }
      if (basic_type == array_type || basic_type == struct_type) {
        RecursiveInitialize(element_expr_ptr, element_ptr_id);
      } else if (basic_type == pointer_type || !element_expr_ptr->integrated_type_->is_const) {
        expression_ptr->children_[2 * i + 1]->Accept(this);
        target_block.AddVariableStore(integrated_type->element_type,
            expression_ptr->children_[2 * i + 1]->IR_ID_, element_ptr_id);
      } else {
        target_block.AddValueStore(integrated_type->element_type,
            static_cast<int>(element_expr_ptr->value_.array_values[i].int_value),
            element_ptr_id);
      }
    }
  } else if (integrated_type->basic_type == struct_type) {
    if (expression_ptr->children_.size() == 3) {
      Throw("Empty struct expression!");
    }
    const auto struct_expr_fields = expression_ptr->children_[2];
    const auto struct_def_ptr = dynamic_cast<Struct *>(integrated_type->struct_node);
    for (int i = 0; i < struct_def_ptr->field_item_index_.size(); ++i) {
      // i-th struct_expr_field: struct_expr_fields->children_[2 * i]
      const std::string &item_name = dynamic_cast<LeafNode *>(struct_expr_fields->children_[2 * i]
          ->children_[0])->GetContent().GetStr();
      const int item_index = struct_def_ptr->field_item_index_[item_name];
      const int target_item_id = function.var_id_++;
      const auto item_expr_ptr = dynamic_cast<Expression *>(struct_expr_fields->children_[2 * i]->children_[2]);
      target_block.AddGetElementPtrByValue(target_item_id, item_expr_ptr->integrated_type_, ptr_id, item_index);
      // %target_item_id <- *item_expr_ptr
      if (integrated_type->element_type->basic_type == char_type ||
          integrated_type->element_type->basic_type == str_type ||
          integrated_type->element_type->basic_type == string_type) {
        Throw("Invalid type in IR.");
      }
      if (item_expr_ptr->integrated_type_->basic_type == array_type ||
          item_expr_ptr->integrated_type_->basic_type == struct_type) {
        RecursiveInitialize(item_expr_ptr, target_item_id);
      } else if (integrated_type->element_type->basic_type == pointer_type ||
          !item_expr_ptr->integrated_type_->is_const) {
        struct_expr_fields->children_[2 * i]->children_[2]->Accept(this);
        target_block.AddVariableStore(item_expr_ptr->integrated_type_,
            struct_expr_fields->children_[2 * i]->children_[2]->IR_ID_, target_item_id);
      } else {
        target_block.AddValueStore(item_expr_ptr->integrated_type_,
            static_cast<int>(item_expr_ptr->value_.array_values[i].int_value),
            target_item_id);
      }
    }
  } else {
    Throw("Recursive initializing shouldn't be used except when initalizing array or struct.");
  }
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
  let_statement_ptr->children_[1]->IR_var_ID_ = function.var_id_++;
  function.AddAlloca(let_statement_ptr->children_[1]->IR_var_ID_, let_statement_ptr->children_[1]->integrated_type_);
  if (let_statement_ptr->children_[1]->integrated_type_->is_int ||
      let_statement_ptr->children_[1]->integrated_type_->basic_type == bool_type ||
      let_statement_ptr->children_[1]->integrated_type_->basic_type == enumeration_type) {
    if (let_statement_ptr->children_[5]->integrated_type_->is_const) {
      function.blocks_[block_stack_.back()].AddValueStore(let_statement_ptr->children_[1]->integrated_type_,
          static_cast<int>(let_statement_ptr->children_[5]->value_.int_value), let_statement_ptr->children_[1]->IR_var_ID_);
    } else {
      let_statement_ptr->children_[5]->Accept(this);
      function.blocks_[block_stack_.back()].AddVariableStore(let_statement_ptr->children_[1]->integrated_type_,
          let_statement_ptr->children_[5]->IR_ID_, let_statement_ptr->children_[1]->IR_var_ID_);
    }
  } else if (let_statement_ptr->children_[1]->integrated_type_->basic_type == array_type ||
      let_statement_ptr->children_[1]->integrated_type_->basic_type == struct_type) {
    RecursiveInitialize(dynamic_cast<Expression *>(let_statement_ptr->children_[5]),
        let_statement_ptr->children_[1]->IR_var_ID_);
  } else if (let_statement_ptr->children_[1]->integrated_type_->basic_type == pointer_type) {
    let_statement_ptr->children_[5]->Accept(this);
    if (let_statement_ptr->children_[5]->IR_ID_ != -1) {
      function.blocks_[block_stack_.back()].AddVariableStore(let_statement_ptr->children_[1]->integrated_type_,
        let_statement_ptr->children_[5]->IR_ID_, let_statement_ptr->children_[1]->IR_var_ID_);
    } else if (let_statement_ptr->children_[5]->IR_var_ID_ != -1) {
      const int loaded_var_id = function.var_id_++;
      function.blocks_[block_stack_.back()].AddLoad(loaded_var_id,
          let_statement_ptr->children_[5]->integrated_type_, let_statement_ptr->children_[5]->IR_var_ID_);
      function.blocks_[block_stack_.back()].AddVariableStore(let_statement_ptr->children_[1]->integrated_type_,
          loaded_var_id, let_statement_ptr->children_[1]->IR_var_ID_);
    } else {
      Throw("Invalid let statement of pointer type value.");
    }
  } else {
    Throw("There is a basic type that not implemented in LetStatement!");
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
      if (expression_ptr->children_.size() == 3) {
        auto inner_scope = expression_ptr->children_[1]->scope_node_;
        for (const auto &it : inner_scope->type_namespace) {
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
        expression_ptr->children_[1]->Accept(this);
        if (expression_ptr->integrated_type_->basic_type != unit_type) { // block expression has a value
          expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          if (expression_ptr->children_[1]->type_.back() == type_expression) {
            if (expression_ptr->children_[1]->children_.back()->integrated_type_->is_const) {
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddSelect(0b111,
                  expression_ptr->IR_ID_, 1, expression_ptr->integrated_type_,
                  static_cast<int>(expression_ptr->children_[1]->children_.back()->value_.int_value),
                  expression_ptr->integrated_type_, 1);
            } else {
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddSelect(0b100,
                  expression_ptr->IR_ID_, 1, expression_ptr->integrated_type_,
                  expression_ptr->children_[1]->children_.back()->IR_ID_,
                  expression_ptr->integrated_type_, -1);
            }
          } else { // expression statement
            expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddSelect(0b100,
                expression_ptr->IR_ID_, 1, expression_ptr->integrated_type_,
                expression_ptr->children_[1]->children_.back()->children_[0]->IR_ID_,
                expression_ptr->integrated_type_, -1);
          }
        }
      }
      break;
    }
    case infinite_loop_expr: {
      // todo
      break;
    }
    case predicate_loop_expr: {
      // todo
      break;
    }
    case if_expr: {
      // todo
      break;
    }
    case literal_expr: {
      switch (expression_ptr->type_[0]) {
        case type_keyword:
        case type_integer_literal: {
          expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].AddAlloca(expression_ptr->IR_var_ID_, expression_ptr->integrated_type_);
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddValueStore(expression_ptr->integrated_type_,
              static_cast<int>(expression_ptr->value_.int_value), expression_ptr->IR_var_ID_);
          break;
        }
        case type_char_literal:
        case type_string_literal:
        case type_raw_string_literal:
        case type_c_string_literal:
        case type_raw_c_string_literal: {
          Throw("Invalid literal type in IR.");
          break;
        }
        default:;
      }
      break;
    }
    case path_in_expr: {
      expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
      expression_ptr->IR_var_ID_ = expression_ptr->GetDefInfo().node->IR_ID_;
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->IR_ID_,
          expression_ptr->integrated_type_, expression_ptr->IR_var_ID_);
      break;
    }
    case grouped_expr: {
      expression_ptr->children_[1]->Accept(this);
      expression_ptr->IR_ID_ = expression_ptr->children_[1]->IR_ID_;
      expression_ptr->IR_var_ID_ = expression_ptr->children_[1]->IR_var_ID_;
      break;
    }
    case array_expr:
    case struct_expr: {
      expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
      functions_[wrapping_functions_.back()].AddAlloca(expression_ptr->IR_var_ID_, expression_ptr->integrated_type_);
      RecursiveInitialize(expression_ptr, expression_ptr->IR_var_ID_);
      expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->IR_ID_,
          expression_ptr->integrated_type_, expression_ptr->IR_var_ID_);
      break;
    }
    case index_expr: {
      expression_ptr->children_[0]->Accept(this);
      if (expression_ptr->children_[0]->IR_var_ID_ == -1) {
        Throw("Cannot get the element of a value.");
      }
      expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
      if (expression_ptr->children_[1]->integrated_type_->is_const) {
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddGetElementPtrByValue(
            expression_ptr->IR_var_ID_, expression_ptr->integrated_type_,
            expression_ptr->children_[0]->IR_var_ID_, static_cast<int>(expression_ptr->children_[1]->value_.int_value));
      } else {
        expression_ptr->children_[1]->Accept(this);
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddGetElementPtrByVariable(
            expression_ptr->IR_var_ID_, expression_ptr->integrated_type_,
            expression_ptr->children_[0]->IR_var_ID_, expression_ptr->children_[1]->IR_ID_);
      }
      break;
    }
    case call_expr: {
      if (expression_ptr->GetDefInfo().node == nullptr) { // builtin
        std::string function_name = dynamic_cast<LeafNode *>(expression_ptr->children_[0]->children_[0])
            ->GetContent().GetStr();
        if (function_name == "printInt") {
          std::vector<FunctionCallArgument> argument_list(1);
          if (expression_ptr->children_[1]->children_[0]->integrated_type_->is_const) {
            argument_list[0].type_ = expression_ptr->children_[1]->children_[0]->integrated_type_;
            argument_list[0].is_variable = false;
            argument_list[0].value_ = static_cast<int>(expression_ptr->children_[1]->children_[0]->value_.int_value);
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddBuiltinCall(-1,
                nullptr, 0, argument_list);
          } else {
            expression_ptr->children_[1]->children_[0]->Accept(this);
            argument_list[0].type_ = expression_ptr->children_[1]->children_[0]->integrated_type_;
            argument_list[0].is_variable = true;
            argument_list[0].value_ = expression_ptr->children_[1]->children_[0]->IR_ID_;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddBuiltinCall(-1,
                nullptr, 0, argument_list);
          }
        } else if (function_name == "printlnInt") {
          std::vector<FunctionCallArgument> argument_list(1);
          if (expression_ptr->children_[1]->children_[0]->integrated_type_->is_const) {
            argument_list[0].type_ = expression_ptr->children_[1]->children_[0]->integrated_type_;
            argument_list[0].is_variable = false;
            argument_list[0].value_ = static_cast<int>(expression_ptr->children_[1]->children_[0]->value_.int_value);
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddBuiltinCall(-1,
                nullptr, 1, argument_list);
          } else {
            expression_ptr->children_[1]->children_[0]->Accept(this);
            argument_list[0].type_ = expression_ptr->children_[1]->children_[0]->integrated_type_;
            argument_list[0].is_variable = true;
            argument_list[0].value_ = expression_ptr->children_[1]->children_[0]->IR_ID_;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddBuiltinCall(-1,
                nullptr, 1, argument_list);
          }
        } else if (function_name == "getInt") {
          expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddBuiltinCall(
              expression_ptr->IR_ID_, expression_ptr->integrated_type_, 2);
        } else if (function_name == "exit") {
          if (expression_ptr->children_[1]->children_[0]->integrated_type_->is_const) {
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddValueReturn(
                expression_ptr->children_[1]->children_[0]->integrated_type_,
                static_cast<int>(expression_ptr->children_[1]->children_[0]->value_.int_value));
          } else {
            expression_ptr->children_[1]->children_[0]->Accept(this);
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVariableReturn(
                expression_ptr->children_[1]->children_[0]->integrated_type_,
                expression_ptr->children_[1]->children_[0]->IR_ID_);
          }
        } else {
          Throw("Invalid builtin (including print, println and getString).");
        }
      } else { // not builtin
        std::vector<FunctionCallArgument> argument_list;
        for (int i = 0; i < expression_ptr->children_[1]->children_.size(); i += 2) {
          if (expression_ptr->children_[1]->children_[i]->integrated_type_->is_const) {
            argument_list.push_back(FunctionCallArgument(expression_ptr->children_[1]
                ->children_[i]->integrated_type_, false,
                static_cast<int>(expression_ptr->children_[1]->children_[i]->value_.int_value)));
          } else {
            expression_ptr->children_[1]->children_[i]->Accept(this);
            argument_list.push_back(FunctionCallArgument(expression_ptr->children_[1]
                ->children_[i]->integrated_type_, true,
                expression_ptr->children_[1]->children_[i]->IR_ID_));
          }
        }
        if (expression_ptr->integrated_type_->basic_type == unit_type) { // void call
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVoidCall(
              expression_ptr->GetDefInfo().node->IR_ID_, argument_list);
        } else { // non-void call
          expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddNonVoidCall(
              expression_ptr->IR_ID_, expression_ptr->integrated_type_,
              expression_ptr->GetDefInfo().node->IR_ID_, argument_list);
        }
      }
      break;
    }
    case method_call_expr: {
      if (expression_ptr->GetDefInfo().node == nullptr) { // builtin

      } else { // not builtin
        std::vector<FunctionCallArgument> argument_list;
        const auto function_def_node = expression_ptr->GetDefInfo().node;
        switch (function_def_node->children_[3]->children_[0]->children_[0]->children_.size()) {
          case 1: { // self
            expression_ptr->children_[0]->Accept(this);
            argument_list.push_back(FunctionCallArgument(expression_ptr->children_[0]->integrated_type_,
                true, expression_ptr->children_[0]->IR_ID_));
            break;
          }
          case 2:
          case 3: { // &self or &mut self
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_var_ID_ == -1) {
              Throw("The 'self' is not a left value and cannot be borrowed.");
            }
            const int borrowed_self_id = functions_[wrapping_functions_.back()].var_id_++;
            auto target_type = functions_[function_def_node->IR_ID_].parameter_types_[0];
            functions_[wrapping_functions_.back()].AddAlloca(borrowed_self_id,
                target_type);
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddPtrStore(
                expression_ptr->children_[0]->IR_var_ID_, borrowed_self_id);
            const int loaded_borrowed_self_id = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(
                loaded_borrowed_self_id, target_type, borrowed_self_id);
            argument_list.push_back(FunctionCallArgument(target_type, true, loaded_borrowed_self_id));
            break;
          }
          default:;
        }
        if (expression_ptr->children_.size() == 3) {
          for (int i = 0; i < expression_ptr->children_[2]->children_.size(); i += 2) {
            if (expression_ptr->children_[2]->children_[i]->integrated_type_->is_const) {
              argument_list.push_back(FunctionCallArgument(expression_ptr->children_[2]
                  ->children_[i]->integrated_type_, false,
                  static_cast<int>(expression_ptr->children_[2]->children_[i]->value_.int_value)));
            } else {
              expression_ptr->children_[2]->children_[i]->Accept(this);
              argument_list.push_back(FunctionCallArgument(expression_ptr->children_[2]
                  ->children_[i]->integrated_type_, true,
                  expression_ptr->children_[2]->children_[i]->IR_ID_));
            }
          }
        }
        if (expression_ptr->integrated_type_->basic_type == unit_type) { // void call
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVoidCall(
              function_def_node->IR_ID_, argument_list);
        } else { // non-void call
          expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddNonVoidCall(
              expression_ptr->IR_ID_, expression_ptr->integrated_type_,
              function_def_node->IR_ID_, argument_list);
        }
      }
      break;
    }
    case field_expr: {
      expression_ptr->children_[0]->Accept(this);
      if (expression_ptr->children_[0]->integrated_type_->basic_type == struct_type) {
        const auto struct_ptr = dynamic_cast<Struct *>(expression_ptr->children_[0]->integrated_type_->struct_node);
        const std::string identifier_name = dynamic_cast<LeafNode *>(expression_ptr->children_[1]->
            children_[0]->children_[0])->GetContent().GetStr();
        expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        const int item_index = struct_ptr->field_item_index_[identifier_name];
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddGetElementPtrByValue(
            expression_ptr->IR_var_ID_, expression_ptr->integrated_type_,
            expression_ptr->children_[0]->IR_var_ID_, item_index);
        expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->IR_ID_,
            expression_ptr->integrated_type_, expression_ptr->IR_var_ID_);
      } else { // expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type
        expression_ptr->children_[0]->Accept(this);
        const int dereferenced_id = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddPtrLoad(dereferenced_id,
            expression_ptr->children_[0]->IR_var_ID_);
        const auto struct_ptr = dynamic_cast<Struct *>(expression_ptr->children_[0]->integrated_type_->element_type->struct_node);
        const std::string identifier_name = dynamic_cast<LeafNode *>(expression_ptr->children_[1]->
            children_[0]->children_[0])->GetContent().GetStr();
        expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        const int item_index = struct_ptr->field_item_index_[identifier_name];
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddGetElementPtrByValue(
            expression_ptr->IR_var_ID_, expression_ptr->integrated_type_,
            dereferenced_id, item_index);
        expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->IR_ID_,
            expression_ptr->integrated_type_, expression_ptr->IR_var_ID_);
      }
      break;
    }
    case continue_expr: {
      // todo
      break;
    }
    case break_expr: {
      // todo
      break;
    }
    case return_expr: {
      // todo
      break;
    }
    case prefix_expr: {
      std::string prefix = dynamic_cast<LeafNode *>(expression_ptr->children_[0])
          ->GetContent().GetStr();
      if (prefix == "-") {
        expression_ptr->children_[0]->Accept(this);
        expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddConstVarBinaryOperation(
            expression_ptr->integrated_type_, sub_, expression_ptr->IR_ID_, 0,
            expression_ptr->children_[0]->IR_ID_);
      } else if (prefix == "*") {
        expression_ptr->children_[0]->Accept(this);
        expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddPtrLoad(
            expression_ptr->IR_var_ID_, expression_ptr->children_[0]->IR_ID_);
        expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->IR_ID_,
            expression_ptr->integrated_type_, expression_ptr->IR_var_ID_);
      } else if (prefix == "!") {
        expression_ptr->children_[0]->Accept(this);
        expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        if (expression_ptr->integrated_type_->basic_type == bool_type) {
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddConstVarBinaryOperation(
              expression_ptr->integrated_type_, sub_, expression_ptr->IR_ID_,
              1, expression_ptr->children_[0]->IR_ID_);
        }
      } else if (prefix == "&") {
        auto content_expr_ptr = expression_ptr->children_[0];
        if (content_expr_ptr->integrated_type_->is_int ||
            content_expr_ptr->integrated_type_->basic_type == bool_type ||
            content_expr_ptr->integrated_type_->basic_type == enumeration_type ||
            content_expr_ptr->integrated_type_->basic_type == struct_type ||
            content_expr_ptr->integrated_type_->basic_type == array_type) {
          content_expr_ptr->Accept(this);
          if (content_expr_ptr->IR_var_ID_ == -1) {
            Throw("The struct/array in borrow expression is not a left value!");
          }
          // the struct/array value has an allocated ptr %content_expr->IR_var_ID_
          expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].AddAlloca(expression_ptr->IR_var_ID_, expression_ptr->integrated_type_);
          // store the ptr %content_expr->IR_var_ID_ into %expression_ptr->IR_var_ID_
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddPtrStore(content_expr_ptr->IR_var_ID_,
              expression_ptr->IR_var_ID_);
          // load the value from ptr %expression_ptr->IR_var_ID_ to variable %expression_ptr->IR_ID_
          expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->IR_ID_,
              expression_ptr->integrated_type_, expression_ptr->IR_var_ID_);
        } else {
          Throw("Invalid type for borrow expression in IR.");
        }
      } else if (prefix == "&&") {
        Throw("There is no && in IR testcases.");
      }
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
          const auto basic_type = expression_ptr->children_[1]->integrated_type_->basic_type;
          if (expression_ptr->children_[1]->integrated_type_->is_int ||
              basic_type == bool_type || basic_type == enumeration_type) {
            if (expression_ptr->children_[1]->integrated_type_->is_const) {
              target_block.AddValueStore(expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value), variable_id);
            } else {
              expression_ptr->children_[1]->Accept(this);
              target_block.AddVariableStore(expression_ptr->children_[0]->integrated_type_,
                  expression_ptr->children_[1]->IR_ID_, variable_id);
            }
          } else if (basic_type == array_type || basic_type == struct_type) {
            RecursiveInitialize(dynamic_cast<Expression *>(expression_ptr->children_[1]), variable_id);
          } else if (expression_ptr->children_[1]->integrated_type_->basic_type == pointer_type) {
            expression_ptr->children_[1]->Accept(this);
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVariableStore(
                expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_,
                variable_id);
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