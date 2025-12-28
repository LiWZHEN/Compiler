#include "IR_generator.h"
#include "item.h"
#include "expression.h"
#include "statements.h"
#include "fstream"

void IRThrow(const std::string &err_info) {
  std::cerr << "[IR Error] " << err_info << '\n';
  throw "";
}

void IRVisitor::AddFunction(const std::shared_ptr<IntegratedType> &return_type) {
  functions_.push_back(IRFunctionNode(return_type));
}
void IRVisitor::AddStruct() {
  structs_.push_back(IRStructNode());
}

void IRVisitor::RecursiveInitialize(const Node *expression_ptr, const int ptr_id) {
  auto &function = functions_[wrapping_functions_.back()];
  const auto &integrated_type = expression_ptr->integrated_type_;
  if (integrated_type->basic_type == array_type) {
    const auto basic_type = integrated_type->element_type->basic_type; // the type of the elements of the array
    if (basic_type == char_type || basic_type == str_type || basic_type == string_type) {
      IRThrow("Invalid type in IR.");
    }
    if (expression_ptr->children_.size() != 5 ||
        dynamic_cast<LeafNode *>(expression_ptr->children_[2])->GetContent().GetStr() != ";") { // [(Expression (, Expression)* ,?)?]
      for (int i = 0; i < integrated_type->size; ++i) {
        const int element_ptr_id = function.var_id_++;
        function.blocks_[block_stack_.back()].AddGetElementPtrByValue(element_ptr_id, integrated_type, ptr_id, i);
        // %element_ptr_id <- *expression_ptr->children_[2 * i + 1]
        if (basic_type == pointer_type) {
          expression_ptr->children_[2 * i + 1]->Accept(this);
          function.blocks_[block_stack_.back()].AddVariableStore(integrated_type->element_type,
              expression_ptr->children_[2 * i + 1]->IR_var_ID_, element_ptr_id);
        } else if (basic_type == array_type || basic_type == struct_type
            || !expression_ptr->children_[2 * i + 1]->integrated_type_->is_const) {
          expression_ptr->children_[2 * i + 1]->Accept(this);
          // todo : for array and struct, use memcpy
          if (expression_ptr->children_[2 * i + 1]->IR_ID_ == -1) {
            expression_ptr->children_[2 * i + 1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[2 * i + 1]->IR_ID_,
                expression_ptr->children_[2 * i + 1]->integrated_type_, expression_ptr->children_[2 * i + 1]->IR_var_ID_);
          }
          function.blocks_[block_stack_.back()].AddVariableStore(integrated_type->element_type,
              expression_ptr->children_[2 * i + 1]->IR_ID_, element_ptr_id);
        } else {
          function.blocks_[block_stack_.back()].AddValueStore(integrated_type->element_type,
              static_cast<int>(expression_ptr->children_[2 * i + 1]->value_.int_value), element_ptr_id);
        }
      }
    } else { // [Expression; Expression]
      if (basic_type == pointer_type) {
        expression_ptr->children_[1]->Accept(this);
        for (int i = 0; i < integrated_type->size; ++i) {
          const int element_ptr_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddGetElementPtrByValue(element_ptr_id, integrated_type, ptr_id, i);
          function.blocks_[block_stack_.back()].AddVariableStore(integrated_type->element_type,
              expression_ptr->children_[1]->IR_var_ID_, element_ptr_id);
        }
      } else if (basic_type == array_type || basic_type == struct_type
          || !expression_ptr->children_[1]->integrated_type_->is_const) {
        expression_ptr->children_[1]->Accept(this);
        // todo : for array and struct, use memcpy
        if (expression_ptr->children_[1]->IR_ID_ == -1) {
          expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
              expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
        }
        for (int i = 0; i < integrated_type->size; ++i) {
          const int element_ptr_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddGetElementPtrByValue(element_ptr_id, integrated_type, ptr_id, i);
          function.blocks_[block_stack_.back()].AddVariableStore(integrated_type->element_type,
              expression_ptr->children_[1]->IR_ID_, element_ptr_id);
        }
      } else if (expression_ptr->children_[1]->value_.int_value == 0) {
        function.blocks_[block_stack_.back()].AddBuiltinMemset(static_cast<int>(expression_ptr->children_[3]
            ->value_.int_value * 4), 0, ptr_id, true);
      } else {
        for (int i = 0; i < integrated_type->size; ++i) {
          const int element_ptr_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddGetElementPtrByValue(element_ptr_id, integrated_type, ptr_id, i);
          function.blocks_[block_stack_.back()].AddValueStore(integrated_type->element_type,
              static_cast<int>(expression_ptr->children_[1]->value_.int_value), element_ptr_id);
        }
      }
    }
  } else if (integrated_type->basic_type == struct_type) {
    if (expression_ptr->children_.size() == 3) {
      IRThrow("Empty struct expression!");
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
      function.blocks_[block_stack_.back()].AddGetElementPtrByValue(target_item_id, integrated_type, ptr_id, item_index);
      // %target_item_id <- *item_expr_ptr
      const auto basic_type = item_expr_ptr->integrated_type_->basic_type;
      if (basic_type == char_type || basic_type == str_type || basic_type == string_type) {
        IRThrow("Invalid type in IR.");
      }
      if (basic_type == pointer_type) {
        struct_expr_fields->children_[2 * i]->children_[2]->Accept(this);
        function.blocks_[block_stack_.back()].AddVariableStore(item_expr_ptr->integrated_type_,
            struct_expr_fields->children_[2 * i]->children_[2]->IR_var_ID_, target_item_id);
      } else if (basic_type == array_type || basic_type == struct_type
          || !item_expr_ptr->integrated_type_->is_const) {
        struct_expr_fields->children_[2 * i]->children_[2]->Accept(this);
        // todo: for array and struct, use memcpy
        if (struct_expr_fields->children_[2 * i]->children_[2]->IR_ID_ == -1) {
          struct_expr_fields->children_[2 * i]->children_[2]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(
              struct_expr_fields->children_[2 * i]->children_[2]->IR_ID_,
              struct_expr_fields->children_[2 * i]->children_[2]->integrated_type_,
              struct_expr_fields->children_[2 * i]->children_[2]->IR_var_ID_);
        }
        function.blocks_[block_stack_.back()].AddVariableStore(item_expr_ptr->integrated_type_,
            struct_expr_fields->children_[2 * i]->children_[2]->IR_ID_, target_item_id);
      } else {
        function.blocks_[block_stack_.back()].AddValueStore(item_expr_ptr->integrated_type_,
            static_cast<int>(item_expr_ptr->value_.int_value), target_item_id);
      }
    }
  } else {
    IRThrow("Recursive initializing shouldn't be used except when initalizing array or struct.");
  }
}

void IRVisitor::DeclareItems(const std::shared_ptr<ScopeNode> &new_scope) {
  for (const auto &it : new_scope->type_namespace) {
    if (it.second.node_type == type_struct) {
      it.second.node->IR_ID_ = static_cast<int>(structs_.size());
      AddStruct();
      for (const auto &associated_item :
          dynamic_cast<Struct *>(it.second.node)->associated_items_) {
        if (associated_item.second.node_type == type_function) {
          associated_item.second.node->IR_ID_ = static_cast<int>(functions_.size());
          AddFunction(associated_item.second.node->integrated_type_);
        }
      }
    }
  }
  for (const auto &it : new_scope->value_namespace) {
    if (it.second.node_type == type_function && it.second.node != nullptr) {
      it.second.node->IR_ID_ = static_cast<int>(functions_.size());
      AddFunction(it.second.node->integrated_type_);
      if (main_function_id_ == -1 &&
          dynamic_cast<LeafNode *>(it.second.node->children_[1])->GetContent().GetStr() == "main") {
        main_function_id_ = it.second.node->IR_ID_;
      }
    }
  }
}

int IRVisitor::GetBlockValue(Node *visited_statements_ptr, const std::shared_ptr<IntegratedType> &expected_type) {
  if (expected_type->basic_type == unit_type || visited_statements_ptr->integrated_type_->basic_type == never_type) {
    return -1;
  }
  const int variable_id = functions_[wrapping_functions_.back()].var_id_++;
  if (visited_statements_ptr->type_.back() == type_expression) {
    if (visited_statements_ptr->children_.back()->integrated_type_->is_const) {
      const int value = static_cast<int>(visited_statements_ptr->children_.back()->value_.int_value);
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddSelect(0b111,
          variable_id, 1, expected_type, value, expected_type, value);
    } else {
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddSelect(0b100,
          variable_id, 1, expected_type,
          visited_statements_ptr->children_.back()->IR_ID_,
          expected_type, visited_statements_ptr->children_.back()->IR_ID_);
    }
  } else { // expression statement
    functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddSelect(0b100,
        variable_id, 1, expected_type,
        visited_statements_ptr->children_.back()->children_[0]->IR_ID_,
        expected_type, visited_statements_ptr->children_.back()->children_[0]->IR_ID_);
  }
  return variable_id;
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
  // collect and declare the structs and functions
  DeclareItems(crate_ptr->scope_node_);
  for (const auto &it : crate_ptr->children_) {
    if (it->type_[0] == type_struct) {
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
  functions_[wrapping_functions_.back()].blocks_[0] = IRBlock();
  int block_expression_index = -1;
  for (int i = 0; i < function_ptr->children_.size(); ++i) {
    if (function_ptr->type_[i] == type_function_parameters) {
      auto &function_parameter_types = functions_[function_id].parameter_types_;
      functions_[wrapping_functions_.back()].var_id_ +=
          static_cast<int>((function_ptr->children_[i]->children_.size() + 1) / 2);
      for (int j = 0; j < function_ptr->children_[i]->children_.size(); j += 2) {
        function_ptr->children_[i]->children_[j]->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].AddAlloca(function_ptr->children_[i]->children_[j]->IR_var_ID_,
            function_ptr->children_[i]->children_[j]->integrated_type_);
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVariableStore(function_ptr->
            children_[i]->children_[j]->integrated_type_, j / 2,
            function_ptr->children_[i]->children_[j]->IR_var_ID_);
        function_ptr->children_[i]->children_[j]->IR_ID_ = static_cast<int>(function_parameter_types.size());
        function_parameter_types.push_back(function_ptr->children_[i]->children_[j]->integrated_type_);
      }
    } else if (function_ptr->type_[i] == type_block_expression) {
      // collect and declare the struct and function
      DeclareItems(function_ptr->children_[i]->scope_node_);
      function_ptr->children_[i]->Accept(this);
      block_expression_index = i;
    }
  }
  if (functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].instructions_.empty()) {
    if (function_ptr->children_[block_expression_index]->IR_ID_ == -1) {
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVoidReturn();
    } else {
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVariableReturn(
          function_ptr->integrated_type_, function_ptr->children_[block_expression_index]->IR_ID_);
    }
  } else {
    const auto type = functions_[wrapping_functions_.back()].blocks_[block_stack_.back()]
      .instructions_.back().instruction_type_;
    if (type != conditional_br_ && type != unconditional_br_ && type != value_ret_
        && type != variable_ret_ && type != void_ret_) {
      if (function_ptr->children_[block_expression_index]->IR_ID_ == -1) {
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVoidReturn();
      } else {
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVariableReturn(
            function_ptr->integrated_type_, function_ptr->children_[block_expression_index]->IR_ID_);
      }
    }
  }
  wrapping_functions_.pop_back();
  block_stack_.pop_back();
}
void IRVisitor::Visit(BlockExpression *block_expression_ptr) {
  if (block_expression_ptr->type_[1] == type_statements) {
    block_expression_ptr->children_[1]->Accept(this);
  }
  block_expression_ptr->IR_ID_ = block_expression_ptr->children_[1]->IR_ID_;
}
void IRVisitor::Visit(Statements *statements_ptr) {
  for (int i = 0; i < statements_ptr->children_.size(); ++i) {
    if (statements_ptr->type_[i] == type_statement &&
        statements_ptr->children_[i]->type_[0] == type_item &&
        statements_ptr->children_[i]->children_[0]->type_[0] == type_struct) {
      statements_ptr->children_[i]->Accept(this);
    }
  }
  for (int i = 0; i + 1 < statements_ptr->children_.size(); ++i) { // visit statement
    statements_ptr->children_[i]->Accept(this);
  }
  if (statements_ptr->type_.back() == type_expression) {
    const auto basic_type = statements_ptr->children_.back()->integrated_type_->basic_type;
    if (basic_type == array_type || basic_type == struct_type || basic_type == pointer_type
        || !statements_ptr->children_.back()->integrated_type_->is_const) {
      statements_ptr->children_.back()->Accept(this);
      // todo: for array and struct, replace loading the whole thing with loading a reference
      if (statements_ptr->children_.back()->IR_ID_ == -1 && statements_ptr->children_.back()->IR_var_ID_ != -1) {
        statements_ptr->children_.back()->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(statements_ptr->children_.back()->IR_ID_,
            statements_ptr->children_.back()->integrated_type_, statements_ptr->children_.back()->IR_var_ID_);
      }
      statements_ptr->IR_ID_ = statements_ptr->children_.back()->IR_ID_;
    } else {
      statements_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
      functions_[wrapping_functions_.back()].AddAlloca(statements_ptr->IR_var_ID_,
          statements_ptr->children_.back()->integrated_type_);
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddValueStore(
          statements_ptr->children_.back()->integrated_type_,
          static_cast<int>(statements_ptr->children_.back()->value_.int_value),
          statements_ptr->IR_var_ID_);
      statements_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(
          statements_ptr->IR_ID_, statements_ptr->children_.back()->integrated_type_,
          statements_ptr->IR_var_ID_);
    }
  } else {
    statements_ptr->children_.back()->Accept(this);
    statements_ptr->IR_ID_ = statements_ptr->children_.back()->IR_ID_;
  }
}
void IRVisitor::Visit(Statement *statement_ptr) {
  statement_ptr->children_[0]->Accept(this);
  if (statement_ptr->type_[0] == type_expression_statement) {
    // todo: for array and struct, replace loading the whole thing with loading a reference
    if (statement_ptr->children_[0]->IR_ID_ == -1 && statement_ptr->children_[0]->IR_var_ID_ != -1) {
      statement_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(statement_ptr->children_[0]->IR_ID_,
          statement_ptr->children_[0]->integrated_type_, statement_ptr->children_[0]->IR_var_ID_);
    }
    statement_ptr->IR_ID_ = statement_ptr->children_[0]->IR_ID_;
  }
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
      if (let_statement_ptr->children_[5]->IR_ID_ == -1) {
        let_statement_ptr->children_[5]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(let_statement_ptr->children_[5]->IR_ID_,
            let_statement_ptr->children_[5]->integrated_type_, let_statement_ptr->children_[5]->IR_var_ID_);
      }
      function.blocks_[block_stack_.back()].AddVariableStore(let_statement_ptr->children_[1]->integrated_type_,
          let_statement_ptr->children_[5]->IR_ID_, let_statement_ptr->children_[1]->IR_var_ID_);
    }
  } else if (let_statement_ptr->children_[1]->integrated_type_->basic_type == array_type ||
      let_statement_ptr->children_[1]->integrated_type_->basic_type == struct_type) {
    let_statement_ptr->children_[5]->Accept(this);
    // todo: use memcpy
    if (let_statement_ptr->children_[5]->IR_ID_ == -1) {
      let_statement_ptr->children_[5]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(let_statement_ptr->children_[5]->IR_ID_,
          let_statement_ptr->children_[5]->integrated_type_, let_statement_ptr->children_[5]->IR_var_ID_);
    }
    function.blocks_[block_stack_.back()].AddVariableStore(let_statement_ptr->children_[1]->integrated_type_,
        let_statement_ptr->children_[5]->IR_ID_, let_statement_ptr->children_[1]->IR_var_ID_);
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
      IRThrow("Invalid let statement of pointer type value.");
    }
  } else {
    IRThrow("There is a basic type that not implemented in LetStatement!");
  }
}
void IRVisitor::Visit(ExpressionStatement *expression_statement_ptr) {
  const auto expr_type = expression_statement_ptr->integrated_type_->basic_type;
  if (expr_type == array_type || expr_type == struct_type || expr_type == pointer_type
      || !expression_statement_ptr->children_[0]->integrated_type_->is_const) {
    expression_statement_ptr->children_[0]->Accept(this);
    if (expression_statement_ptr->children_.size() == 1) {
      if (expression_statement_ptr->children_[0]->IR_ID_ == -1 && expression_statement_ptr->children_[0]->IR_var_ID_ != -1) {
        expression_statement_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_statement_ptr->children_[0]->IR_ID_,
            expression_statement_ptr->children_[0]->integrated_type_, expression_statement_ptr->children_[0]->IR_var_ID_);
      }
      expression_statement_ptr->IR_ID_ = expression_statement_ptr->children_[0]->IR_ID_;
    }
  } else if (expression_statement_ptr->children_.size() == 1) {
    expression_statement_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
    functions_[wrapping_functions_.back()].AddAlloca(expression_statement_ptr->IR_var_ID_,
        expression_statement_ptr->integrated_type_);
    functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddValueStore(
        expression_statement_ptr->integrated_type_, static_cast<int>(expression_statement_ptr
        ->children_[0]->value_.int_value), expression_statement_ptr->IR_var_ID_);
    expression_statement_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
    functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(
        expression_statement_ptr->IR_ID_, expression_statement_ptr->integrated_type_,
        expression_statement_ptr->IR_var_ID_);
  }
}
void IRVisitor::Visit(StringLiteral *string_literal_ptr) {
  IRThrow("String is not implemented.");
}
void IRVisitor::Visit(Expression *expression_ptr) {
  switch (expression_ptr->GetExprType()) {
    case block_expr: {
      if (expression_ptr->children_.size() == 3) {
        DeclareItems(expression_ptr->children_[1]->scope_node_);
        expression_ptr->children_[1]->Accept(this);
        expression_ptr->IR_ID_ = GetBlockValue(expression_ptr->children_[1], expression_ptr->integrated_type_);
      }
      break;
    }
    case infinite_loop_expr: {
      const int loop_begin = functions_[wrapping_functions_.back()].var_id_++;
      functions_[wrapping_functions_.back()].blocks_[loop_begin] = IRBlock();
      const int loop_end = functions_[wrapping_functions_.back()].var_id_++;
      functions_[wrapping_functions_.back()].blocks_[loop_end] = IRBlock();
      if (expression_ptr->integrated_type_->basic_type != unit_type &&
          expression_ptr->integrated_type_->basic_type != never_type) {
        expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
      }
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
          AddUnconditionalBranch(loop_begin);
      // complete loop_begin
      block_stack_.back() = loop_begin;
      wrapping_loops_.push_back({loop_begin, loop_end, expression_ptr->IR_ID_,
          wrapping_functions_.back()});
      if (expression_ptr->children_.size() == 3) {
        IRThrow("Empty indefinite loop!");
      }
      DeclareItems(expression_ptr->children_[2]->scope_node_);
      expression_ptr->children_[2]->Accept(this);
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
          AddUnconditionalBranch(loop_begin);
      wrapping_loops_.pop_back();
      block_stack_.back() = loop_end;
      break;
    }
    case predicate_loop_expr: {
      if (expression_ptr->children_[2]->integrated_type_->is_const) {
        if (expression_ptr->children_[2]->value_.int_value == 1) { // infinite loop
          const int loop_begin = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[loop_begin] = IRBlock();
          const int loop_end = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[loop_end] = IRBlock();
          if (expression_ptr->integrated_type_->basic_type != unit_type &&
              expression_ptr->integrated_type_->basic_type != never_type) {
            expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          }
          wrapping_loops_.push_back({loop_begin, loop_end, expression_ptr->IR_ID_,
              wrapping_functions_.back()});
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
              AddUnconditionalBranch(loop_begin);
          // loop_begin
          block_stack_.back() = loop_begin;
          if (expression_ptr->children_.size() == 7) {
            DeclareItems(expression_ptr->children_[5]->scope_node_);
            expression_ptr->children_[5]->Accept(this);
          } else {
            IRThrow("Empty indefinite loop!");
          }
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
              AddUnconditionalBranch(loop_begin);
          // loop_end
          block_stack_.back() = loop_end;
          wrapping_loops_.pop_back();
        } else { // skip
          break;
        }
      } else {
        const int condition_check = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[condition_check] = IRBlock();
        const int loop_begin = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[loop_begin] = IRBlock();
        const int loop_end = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[loop_end] = IRBlock();
        wrapping_loops_.push_back({condition_check, loop_end});
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
            AddUnconditionalBranch(condition_check);
        // complete condition_check
        block_stack_.back() = condition_check;
        expression_ptr->children_[2]->Accept(this);
        if (expression_ptr->children_[2]->IR_ID_ == -1) {
          expression_ptr->children_[2]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[2]->IR_ID_,
              expression_ptr->children_[2]->integrated_type_, expression_ptr->children_[2]->IR_var_ID_);
        }
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
            AddConditionalBranch(expression_ptr->children_[2]->IR_ID_, loop_begin,
                loop_end);
        // complete loop_begin
        block_stack_.back() = loop_begin;
        if (expression_ptr->children_.size() == 7) {
          DeclareItems(expression_ptr->children_[5]->scope_node_);
          expression_ptr->children_[5]->Accept(this);
        }
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
            AddUnconditionalBranch(condition_check);
        block_stack_.back() = loop_end;
        wrapping_loops_.pop_back();
      }
      break;
    }
    case if_expr: {
      if (expression_ptr->children_.size() == 6) { // empty block, skip
        break;
      }
      if (expression_ptr->children_.size() == 7) { // only "if" branch
        if (expression_ptr->children_[2]->integrated_type_->is_const) {
          if (expression_ptr->children_[2]->value_.int_value == 0) { // always false, skip if_expr
            break;
          }
          // always true
          DeclareItems(expression_ptr->children_[5]->scope_node_);
          expression_ptr->children_[5]->Accept(this);
        } else { // whether enter "if" block depends on the condition expression
          expression_ptr->children_[2]->Accept(this);
          if (expression_ptr->children_[2]->IR_ID_ == -1) {
            expression_ptr->children_[2]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[2]->IR_ID_,
                expression_ptr->children_[2]->integrated_type_, expression_ptr->children_[2]->IR_var_ID_);
          }
          const int if_block_id = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[if_block_id] = IRBlock();
          const int exit_if_block_id = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[exit_if_block_id] = IRBlock();
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddConditionalBranch(
              expression_ptr->children_[2]->IR_ID_, if_block_id, exit_if_block_id);
          // complete if_label_block
          block_stack_.back() = if_block_id;
          DeclareItems(expression_ptr->children_[5]->scope_node_);
          expression_ptr->children_[5]->Accept(this);
          if (functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].instructions_.empty()) {
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddUnconditionalBranch(exit_if_block_id);
            block_stack_.back() = exit_if_block_id;
          } else {
            const auto last_instruction_type = functions_[wrapping_functions_.back()].
              blocks_[block_stack_.back()].instructions_.back().instruction_type_;
            if (last_instruction_type == value_ret_ || last_instruction_type == variable_ret_ ||
                last_instruction_type == void_ret_ || last_instruction_type == conditional_br_ ||
                last_instruction_type == unconditional_br_) { // the if block has ended
              block_stack_.back() = exit_if_block_id;
            } else {
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddUnconditionalBranch(exit_if_block_id);
              block_stack_.back() = exit_if_block_id;
            }
          }
        }
      } else { // has "else" branch
        if (expression_ptr->children_[2]->integrated_type_->is_const) {
          if (expression_ptr->children_[2]->value_.int_value == 1) { // always true
            if (expression_ptr->type_[5] == type_punctuation) { // empty "if" block
              break;
            }
            DeclareItems(expression_ptr->children_[5]->scope_node_);
            expression_ptr->children_[5]->Accept(this);
            expression_ptr->IR_ID_ = GetBlockValue(expression_ptr->children_[5], expression_ptr->integrated_type_);
          } else { // always false
            expression_ptr->children_.back()->Accept(this);
            if (expression_ptr->children_.back()->IR_ID_ == -1) {
              expression_ptr->children_.back()->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_.back()->IR_ID_,
                  expression_ptr->children_.back()->integrated_type_, expression_ptr->children_.back()->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddSelect(0b100,
                expression_ptr->IR_ID_, 1, expression_ptr->integrated_type_,
                expression_ptr->children_.back()->IR_ID_, expression_ptr->integrated_type_,
                expression_ptr->children_.back()->IR_ID_);
          }
        } else { // entering which block depends on condition expression
          expression_ptr->children_[2]->Accept(this);
          if (expression_ptr->children_[2]->IR_ID_ == -1) {
            expression_ptr->children_[2]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[2]->IR_ID_,
                expression_ptr->children_[2]->integrated_type_, expression_ptr->children_[2]->IR_var_ID_);
          }
          const int if_block_id = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[if_block_id] = IRBlock();
          const int else_block_id = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[if_block_id] = IRBlock();
          const int exit_if_block_id = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[exit_if_block_id] = IRBlock();
          // jump conditionally
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddConditionalBranch(
              expression_ptr->children_[2]->IR_ID_, if_block_id, else_block_id);
          // complete if_label_block
          block_stack_.back() = if_block_id;
          DeclareItems(expression_ptr->children_[5]->scope_node_);
          expression_ptr->children_[5]->Accept(this);
          const int if_block_value = GetBlockValue(expression_ptr->children_[5], expression_ptr->integrated_type_);
          bool if_returns = true;
          if (functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].instructions_.empty()) {
            if_returns = false;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddUnconditionalBranch(exit_if_block_id);
          } else {
            const auto last_instruction_type_in_if = functions_[wrapping_functions_.back()].
              blocks_[block_stack_.back()].instructions_.back().instruction_type_;
            if (last_instruction_type_in_if != value_ret_ && last_instruction_type_in_if != variable_ret_ &&
                last_instruction_type_in_if != void_ret_ && last_instruction_type_in_if != conditional_br_ &&
                last_instruction_type_in_if != unconditional_br_) {
              if_returns = false;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddUnconditionalBranch(exit_if_block_id);
            }
          }
          // complete else_label_block
          block_stack_.back() = else_block_id;
          expression_ptr->children_.back()->Accept(this);
          bool else_returns = true;
          if (functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].instructions_.empty()) {
            else_returns = false;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddUnconditionalBranch(exit_if_block_id);
          } else {
            const auto last_instruction_type_in_else = functions_[wrapping_functions_.back()].
              blocks_[block_stack_.back()].instructions_.back().instruction_type_;
            if (last_instruction_type_in_else != value_ret_ && last_instruction_type_in_else != variable_ret_ &&
                last_instruction_type_in_else != void_ret_ && last_instruction_type_in_else != conditional_br_ &&
                last_instruction_type_in_else != unconditional_br_) {
              else_returns = false;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddUnconditionalBranch(exit_if_block_id);
            }
          }
          // back together
          if (if_returns && else_returns) {
            // remove block with index exit_if_block_id
            functions_[wrapping_functions_.back()].blocks_.erase(exit_if_block_id);
          } else {
            block_stack_.back() = exit_if_block_id;
            if (if_block_value != -1 && expression_ptr->children_.back()->IR_ID_ != -1) {
              expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddSelect(0b000,
                  expression_ptr->IR_ID_, expression_ptr->children_[2]->IR_ID_,
                  expression_ptr->integrated_type_, if_block_value,
                  expression_ptr->integrated_type_, expression_ptr->children_.back()->IR_ID_);
            } else if (if_block_value != -1) {
              expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddSelect(0b100,
                  expression_ptr->IR_ID_, 1, expression_ptr->integrated_type_, if_block_value,
                  expression_ptr->integrated_type_, if_block_value);
            } else if (expression_ptr->children_.back()->IR_ID_ != -1) {
              expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddSelect(0b100,
                  expression_ptr->IR_ID_, 0, expression_ptr->integrated_type_, expression_ptr->children_.back()->IR_ID_,
                  expression_ptr->integrated_type_, expression_ptr->children_.back()->IR_ID_);
            }
            // If two blocks all returned, there shouldn't be select instruction.
          }
        }
      }
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
          IRThrow("Invalid literal type in IR.");
          break;
        }
        default:;
      }
      break;
    }
    case path_in_expr: {
      expression_ptr->IR_var_ID_ = expression_ptr->GetDefInfo().node->IR_var_ID_;
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
      break;
    }
    case index_expr: {
      expression_ptr->children_[0]->Accept(this);
      if (expression_ptr->children_[0]->integrated_type_->basic_type == array_type) {
        if (expression_ptr->children_[0]->IR_var_ID_ == -1) {
          IRThrow("Cannot get the element of a value.");
        }
        expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        if (expression_ptr->children_[1]->integrated_type_->is_const) {
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddGetElementPtrByValue(
              expression_ptr->IR_var_ID_, expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[0]->IR_var_ID_, static_cast<int>(expression_ptr->children_[1]->value_.int_value));
        } else {
          expression_ptr->children_[1]->Accept(this);
          const int loaded_value = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(loaded_value,
              expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddGetElementPtrByVariable(
              expression_ptr->IR_var_ID_, expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[0]->IR_var_ID_, loaded_value);
        }
      } else { // expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type
        if (expression_ptr->children_[0]->IR_var_ID_ == -1) {
          IRThrow("Cannot dereference a value.");
        }
        const int dereferenced_id = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddPtrLoad(dereferenced_id,
            expression_ptr->children_[0]->IR_var_ID_);
        expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        if (expression_ptr->children_[1]->integrated_type_->is_const) {
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddGetElementPtrByValue(
              expression_ptr->IR_var_ID_, expression_ptr->children_[0]->integrated_type_->element_type,
              dereferenced_id, static_cast<int>(expression_ptr->children_[1]->value_.int_value));
        } else {
          expression_ptr->children_[1]->Accept(this);
          const int loaded_value = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(loaded_value,
              expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddGetElementPtrByVariable(
              expression_ptr->IR_var_ID_, expression_ptr->children_[0]->integrated_type_->element_type,
              dereferenced_id, loaded_value);
        }
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
            argument_list[0].is_variable_ = false;
            argument_list[0].value_ = static_cast<int>(expression_ptr->children_[1]->children_[0]->value_.int_value);
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddBuiltinCall(-1,
                nullptr, 0, argument_list);
          } else {
            expression_ptr->children_[1]->children_[0]->Accept(this);
            if (expression_ptr->children_[1]->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[1]->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->children_[0]->integrated_type_, expression_ptr->children_[1]->children_[0]->IR_var_ID_);
            }
            argument_list[0].type_ = expression_ptr->children_[1]->children_[0]->integrated_type_;
            argument_list[0].is_variable_ = true;
            argument_list[0].value_ = expression_ptr->children_[1]->children_[0]->IR_ID_;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddBuiltinCall(-1,
                nullptr, 0, argument_list);
          }
        } else if (function_name == "printlnInt") {
          std::vector<FunctionCallArgument> argument_list(1);
          if (expression_ptr->children_[1]->children_[0]->integrated_type_->is_const) {
            argument_list[0].type_ = expression_ptr->children_[1]->children_[0]->integrated_type_;
            argument_list[0].is_variable_ = false;
            argument_list[0].value_ = static_cast<int>(expression_ptr->children_[1]->children_[0]->value_.int_value);
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddBuiltinCall(-1,
                nullptr, 1, argument_list);
          } else {
            expression_ptr->children_[1]->children_[0]->Accept(this);
            if (expression_ptr->children_[1]->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[1]->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->children_[0]->integrated_type_, expression_ptr->children_[1]->children_[0]->IR_var_ID_);
            }
            argument_list[0].type_ = expression_ptr->children_[1]->children_[0]->integrated_type_;
            argument_list[0].is_variable_ = true;
            argument_list[0].value_ = expression_ptr->children_[1]->children_[0]->IR_ID_;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddBuiltinCall(-1,
                nullptr, 1, argument_list);
          }
        } else if (function_name == "getInt") {
          expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddBuiltinCall(
              expression_ptr->IR_ID_, expression_ptr->integrated_type_, 2);
        } else if (function_name == "exit") {
          /*if (expression_ptr->children_[1]->children_[0]->integrated_type_->is_const) {
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddValueReturn(
                expression_ptr->children_[1]->children_[0]->integrated_type_,
                static_cast<int>(expression_ptr->children_[1]->children_[0]->value_.int_value));
          } else {
            expression_ptr->children_[1]->children_[0]->Accept(this);
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVariableReturn(
                expression_ptr->children_[1]->children_[0]->integrated_type_,
                expression_ptr->children_[1]->children_[0]->IR_ID_);
          }*/
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVoidReturn();
        } else {
          IRThrow("Invalid builtin (including print, println and getString).");
        }
      } else { // not builtin
        std::vector<FunctionCallArgument> argument_list;
        if (expression_ptr->children_.size() == 2) { // has call_params
          for (int i = 0; i < expression_ptr->children_[1]->children_.size(); i += 2) {
            const auto basic_type = expression_ptr->children_[1]->children_[i]->integrated_type_->basic_type;
            if (!expression_ptr->children_[1]->children_[i]->integrated_type_->is_const ||
                basic_type == array_type || basic_type == struct_type || basic_type == pointer_type) {
              expression_ptr->children_[1]->children_[i]->Accept(this);
              if (expression_ptr->children_[1]->children_[i]->IR_ID_ == -1) {
                expression_ptr->children_[1]->children_[i]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->children_[i]->IR_ID_,
                    expression_ptr->children_[1]->children_[i]->integrated_type_, expression_ptr->children_[1]->children_[i]->IR_var_ID_);
              }
              argument_list.push_back(FunctionCallArgument(expression_ptr->children_[1]
                  ->children_[i]->integrated_type_, true,
                  expression_ptr->children_[1]->children_[i]->IR_ID_));
            } else {
              argument_list.push_back(FunctionCallArgument(expression_ptr->children_[1]
                  ->children_[i]->integrated_type_, false,
                  static_cast<int>(expression_ptr->children_[1]->children_[i]->value_.int_value)));
            }
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
        IRThrow("There is no builtin method in IR testcases.");
      } else { // not builtin
        std::vector<FunctionCallArgument> argument_list;
        const auto function_def_node = expression_ptr->GetDefInfo().node;
        if (expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type) {
          switch (function_def_node->children_[3]->children_[0]->children_[0]->children_.size()) {
            case 1: { // self
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              const int loaded_reference_self_id = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(
                  loaded_reference_self_id, expression_ptr->children_[0]->integrated_type_->element_type,
                  expression_ptr->children_[0]->IR_ID_);
              argument_list.push_back(FunctionCallArgument(expression_ptr->children_[0]->integrated_type_,
                  true, loaded_reference_self_id));
              break;
            }
            case 2:
            case 3: { // &self or &mut self
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              argument_list.push_back(FunctionCallArgument(expression_ptr->children_[0]->integrated_type_,
                  true, expression_ptr->children_[0]->IR_ID_));
              break;
            }
            default:;
          }
        } else {
          switch (function_def_node->children_[3]->children_[0]->children_[0]->children_.size()) {
            case 1: { // self
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              argument_list.push_back(FunctionCallArgument(expression_ptr->children_[0]->integrated_type_,
                  true, expression_ptr->children_[0]->IR_ID_));
              break;
            }
            case 2:
            case 3: { // &self or &mut self
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_var_ID_ == -1) {
                IRThrow("The 'self' is not a left value and cannot be borrowed.");
              }
              std::shared_ptr<IntegratedType> target_type;
              if (functions_[function_def_node->IR_ID_].parameter_types_.empty()) {
                for (int i = 0; i < function_def_node->children_.size(); ++i) {
                  if (function_def_node->type_[i] == type_function_parameters) {
                    target_type = function_def_node->children_[i]->children_[0]->integrated_type_;
                    break;
                  }
                }
              } else {
                target_type = functions_[function_def_node->IR_ID_].parameter_types_[0];
              }
              argument_list.push_back(FunctionCallArgument(target_type, true,
                  expression_ptr->children_[0]->IR_var_ID_));
              break;
            }
            default:;
          }
        }
        if (expression_ptr->children_.size() == 3) {
          for (int i = 0; i < expression_ptr->children_[2]->children_.size(); i += 2) {
            const auto basic_type = expression_ptr->children_[2]->children_[i]->integrated_type_->basic_type;
            if (!expression_ptr->children_[2]->children_[i]->integrated_type_->is_const ||
                basic_type == array_type || basic_type == struct_type || basic_type == pointer_type) {
              expression_ptr->children_[2]->children_[i]->Accept(this);
              // todo: for array and struct, pass reference
              if (expression_ptr->children_[2]->children_[i]->IR_ID_ == -1) {
                expression_ptr->children_[2]->children_[i]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[2]->children_[i]->IR_ID_,
                    expression_ptr->children_[2]->children_[i]->integrated_type_, expression_ptr->children_[2]->children_[i]->IR_var_ID_);
              }
              argument_list.push_back(FunctionCallArgument(expression_ptr->children_[2]
                  ->children_[i]->integrated_type_, true,
                  expression_ptr->children_[2]->children_[i]->IR_ID_));
            } else {
              argument_list.push_back(FunctionCallArgument(expression_ptr->children_[2]
                  ->children_[i]->integrated_type_, false,
                  static_cast<int>(expression_ptr->children_[2]->children_[i]->value_.int_value)));
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
            expression_ptr->IR_var_ID_, expression_ptr->children_[0]->integrated_type_,
            expression_ptr->children_[0]->IR_var_ID_, item_index);
      } else { // expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type
        const int dereferenced_id = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddPtrLoad(dereferenced_id,
            expression_ptr->children_[0]->IR_var_ID_);
        const auto struct_ptr = dynamic_cast<Struct *>(expression_ptr->children_[0]
            ->integrated_type_->element_type->struct_node);
        const std::string identifier_name = dynamic_cast<LeafNode *>(expression_ptr->children_[1]->
            children_[0]->children_[0])->GetContent().GetStr();
        expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        const int item_index = struct_ptr->field_item_index_[identifier_name];
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddGetElementPtrByValue(
            expression_ptr->IR_var_ID_, expression_ptr->children_[0]->integrated_type_->element_type,
            dereferenced_id, item_index);
      }
      break;
    }
    case continue_expr: {
      functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
          AddUnconditionalBranch(wrapping_loops_.back().begin);
      break;
    }
    case break_expr: {
      if (expression_ptr->children_.size() == 1) {
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
            AddUnconditionalBranch(wrapping_loops_.back().end);
      } else {
        const auto basic_type = expression_ptr->children_[1]->integrated_type_->basic_type;
        if (expression_ptr->children_[1]->integrated_type_->is_const &&
            (expression_ptr->children_[1]->integrated_type_->is_int || basic_type == bool_type
            || basic_type == enumeration_type)) {
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
              AddSelect(0b111, wrapping_loops_.back().var_id, 1,
              expression_ptr->children_[1]->integrated_type_,
              static_cast<int>(expression_ptr->children_[1]->value_.int_value),
              expression_ptr->children_[1]->integrated_type_,
              static_cast<int>(expression_ptr->children_[1]->value_.int_value));
        } else {
          expression_ptr->children_[1]->Accept(this);
          if (expression_ptr->children_[1]->IR_ID_ == -1) {
            expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
          }
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
              AddSelect(0b100, wrapping_loops_.back().var_id, 1,
              expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_,
              expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_);
        }
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].
            AddUnconditionalBranch(wrapping_loops_.back().end);
      }
      break;
    }
    case return_expr: {
      if (expression_ptr->children_.size() == 1) {
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVoidReturn();
      } else {
        const auto basic_type = expression_ptr->children_[1]->integrated_type_->basic_type;
        if ((expression_ptr->children_[1]->integrated_type_->is_int || basic_type == bool_type ||
            basic_type == enumeration_type) && expression_ptr->children_[1]->integrated_type_->is_const) {
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddValueReturn(
              expression_ptr->children_[1]->integrated_type_,
              static_cast<int>(expression_ptr->children_[1]->value_.int_value));
        } else {
          expression_ptr->children_[1]->Accept(this);
          if (expression_ptr->children_[1]->IR_ID_ == -1) {
            expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
          }
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVariableReturn(
              expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_);
        }
      }
      break;
    }
    case prefix_expr: {
      std::string prefix = dynamic_cast<LeafNode *>(expression_ptr->children_[0])
          ->GetContent().GetStr();
      if (prefix == "-") {
        expression_ptr->children_[1]->Accept(this);
        if (expression_ptr->children_[1]->IR_ID_ == -1) {
          expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
              expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
        }
        expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddConstVarBinaryOperation(
            expression_ptr->integrated_type_, sub_, expression_ptr->IR_ID_, 0,
            expression_ptr->children_[1]->IR_ID_);
      } else if (prefix == "*") {
        expression_ptr->children_[1]->Accept(this);
        expression_ptr->IR_var_ID_ = expression_ptr->children_[1]->IR_ID_;
      } else if (prefix == "!") {
        expression_ptr->children_[1]->Accept(this);
        if (expression_ptr->children_[1]->IR_ID_ == -1) {
          expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
              expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
        }
        expression_ptr->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
        if (expression_ptr->integrated_type_->basic_type == bool_type) {
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddConstVarBinaryOperation(
              expression_ptr->integrated_type_, sub_, expression_ptr->IR_ID_,
              1, expression_ptr->children_[1]->IR_ID_);
        } else {
          functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVarConstBinaryOperation(
              expression_ptr->integrated_type_, xor_, expression_ptr->IR_ID_,
              expression_ptr->children_[1]->IR_ID_, -1);
        }
      } else if (prefix == "&") {
        auto content_expr_ptr = expression_ptr->children_.back();
        const auto basic_type = content_expr_ptr->integrated_type_->basic_type;
        if (content_expr_ptr->integrated_type_->is_int || basic_type == bool_type ||
            basic_type == enumeration_type || basic_type == struct_type || basic_type == array_type) {
          content_expr_ptr->Accept(this);
          if (content_expr_ptr->IR_var_ID_ == -1) {
            const int new_left_value = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].AddAlloca(new_left_value, content_expr_ptr->integrated_type_);
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVariableStore(
                content_expr_ptr->integrated_type_, content_expr_ptr->IR_ID_, new_left_value);
            expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].AddAlloca(expression_ptr->IR_var_ID_, expression_ptr->integrated_type_);
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddPtrStore(new_left_value,
                expression_ptr->IR_var_ID_);
          } else {
            // the struct/array value has an allocated ptr %content_expr->IR_var_ID_
            expression_ptr->IR_var_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].AddAlloca(expression_ptr->IR_var_ID_, expression_ptr->integrated_type_);
            // store the ptr %content_expr->IR_var_ID_ into %expression_ptr->IR_var_ID_
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddPtrStore(content_expr_ptr->IR_var_ID_,
                expression_ptr->IR_var_ID_);
          }
        } else {
          IRThrow("Invalid type for borrow expression in IR.");
        }
      } else if (prefix == "&&") {
        IRThrow("There is no && in IR testcases.");
      }
      break;
    }
    case call_params: {
      break;
    }
    default: { // operator_expr
      auto &function = functions_[wrapping_functions_.back()];
      switch (expression_ptr->GetExprInfix()) {
        case add: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const + var
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                add_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var + const
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                add_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var + var
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                add_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case minus: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const - var
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                sub_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var - const
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                sub_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var - var
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                sub_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case multiply: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const * var
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                mul_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var * const
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                mul_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var * var
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                mul_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case divide: {
          if (expression_ptr->integrated_type_->basic_type == u32_type ||
              expression_ptr->integrated_type_->basic_type == usize_type) { // unsigned division
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const / var
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                  udiv_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var / const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                  udiv_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var / var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                  udiv_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed division
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const / var
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                  sdiv_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var / const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                  sdiv_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var / var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                  sdiv_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
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
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                  urem_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var % const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                  urem_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var % var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                  urem_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed rem
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const % var
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                  srem_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var % const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                  srem_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var % var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                  srem_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          }
          break;
        }
        case bitwise_and: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const & var
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                and_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var & const
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                and_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var & var
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                and_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case bitwise_or: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const | var
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                or_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var | const
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                or_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var | var
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                or_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case bitwise_xor: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const ^ var
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                xor_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var ^ const
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                xor_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var ^ var
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                xor_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case left_shift: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const << var
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                shl_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var << const
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                shl_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var << var
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                shl_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case right_shift: {
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const >> var
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddConstVarBinaryOperation(expression_ptr->integrated_type_,
                ashr_, expression_ptr->IR_ID_, static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var >> const
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->integrated_type_,
                ashr_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var >> var
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->integrated_type_,
                ashr_, expression_ptr->IR_ID_, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case is_equal: {
          // Enum comparison is between consts, the result has been evaluated in value_type
          // There is no comparison between structs and arrays in testcases.
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const == var
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddConstVarIcmp(expression_ptr->IR_ID_, equal_,
                expression_ptr->children_[0]->integrated_type_,
                static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var == const
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddVarConstIcmp(expression_ptr->IR_ID_, equal_,
                expression_ptr->children_[0]->integrated_type_,
                expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var == var
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddTwoVarIcmp(expression_ptr->IR_ID_, equal_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
          }
          break;
        }
        case is_not_equal: {
          // Enum comparison is between consts, the result has been evaluated in value_type
          // There is no comparison between structs and arrays in testcases.
          if (expression_ptr->children_[0]->integrated_type_->is_const) { // const != var
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddConstVarIcmp(expression_ptr->IR_ID_, not_equal_,
                expression_ptr->children_[0]->integrated_type_,
                static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                expression_ptr->children_[1]->IR_ID_);
          } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var != const
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddVarConstIcmp(expression_ptr->IR_ID_, not_equal_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else { // var != var
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddTwoVarIcmp(expression_ptr->IR_ID_, not_equal_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[1]->IR_ID_);
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
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarIcmp(expression_ptr->IR_ID_, unsigned_greater_than_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var > const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstIcmp(expression_ptr->IR_ID_, unsigned_greater_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var > var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarIcmp(expression_ptr->IR_ID_, unsigned_greater_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed greater than
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const > var
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarIcmp(expression_ptr->IR_ID_, signed_greater_than_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var > const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstIcmp(expression_ptr->IR_ID_, signed_greater_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var > var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarIcmp(expression_ptr->IR_ID_, signed_greater_than_,
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
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarIcmp(expression_ptr->IR_ID_, unsigned_less_than_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var < const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstIcmp(expression_ptr->IR_ID_, unsigned_less_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var < var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarIcmp(expression_ptr->IR_ID_, unsigned_less_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed less than
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const < var
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarIcmp(expression_ptr->IR_ID_, signed_less_than_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var < const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstIcmp(expression_ptr->IR_ID_, signed_less_than_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var < var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarIcmp(expression_ptr->IR_ID_, signed_less_than_,
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
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarIcmp(expression_ptr->IR_ID_, unsigned_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var >= const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstIcmp(expression_ptr->IR_ID_, unsigned_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var >= var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarIcmp(expression_ptr->IR_ID_, unsigned_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed greater equal
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const >= var
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarIcmp(expression_ptr->IR_ID_, signed_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var >= const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstIcmp(expression_ptr->IR_ID_, signed_greater_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var >= var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarIcmp(expression_ptr->IR_ID_, signed_greater_equal_,
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
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarIcmp(expression_ptr->IR_ID_, unsigned_less_equal_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var <= const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstIcmp(expression_ptr->IR_ID_, unsigned_less_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var <= var
              expression_ptr->children_[0]->Accept(this);
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarIcmp(expression_ptr->IR_ID_, unsigned_less_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[1]->IR_ID_);
            }
          } else { // signed less equal
            if (expression_ptr->children_[0]->integrated_type_->is_const) { // const <= var
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddConstVarIcmp(expression_ptr->IR_ID_, signed_less_equal_,
                  expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[0]->value_.int_value),
                  expression_ptr->children_[1]->IR_ID_);
            } else if (expression_ptr->children_[1]->integrated_type_->is_const) { // var <= const
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddVarConstIcmp(expression_ptr->IR_ID_, signed_less_equal_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_ID_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else { // var <= var
              expression_ptr->children_[0]->Accept(this);
              if (expression_ptr->children_[0]->IR_ID_ == -1) {
                expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                    expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
              }
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddTwoVarIcmp(expression_ptr->IR_ID_, signed_less_equal_,
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
              function.blocks_[block_stack_.back()].AddSelect(0b111, expression_ptr->IR_ID_, 1,
                  expression_ptr->children_[0]->integrated_type_, 1,
                  expression_ptr->children_[0]->integrated_type_, 1);
            } else {
              // false || _ = _
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddSelect(0b100, expression_ptr->IR_ID_, 1,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_);
            }
          } else {
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            // a || b : if a == true, expression = true; if a == false, expression = b.
            const int if_true_block_label = function.var_id_++;
            function.blocks_[if_true_block_label] = IRBlock();
            const int if_false_block_label = function.var_id_++;
            function.blocks_[if_false_block_label] = IRBlock();
            const int merged_label = function.var_id_++;
            function.blocks_[merged_label] = IRBlock();
            function.blocks_[block_stack_.back()].AddConditionalBranch(expression_ptr->children_[0]->IR_ID_,
                if_true_block_label, if_false_block_label);
            // in if_true_block
            block_stack_.back() = if_true_block_label;
            const int true_block_ans_id = function.var_id_++;
            function.blocks_[block_stack_.back()].AddSelect(0b111, true_block_ans_id, 1,
                expression_ptr->children_[0]->integrated_type_, 1,
                expression_ptr->children_[0]->integrated_type_, 1);
            function.blocks_[block_stack_.back()].AddUnconditionalBranch(merged_label);
            // in if_false_block
            block_stack_.back() = if_false_block_label;
            const int false_block_ans_id = function.var_id_++;
            if (expression_ptr->children_[1]->integrated_type_->is_const) {
              if (expression_ptr->children_[1]->value_.int_value == 0) {
                function.blocks_[block_stack_.back()].AddSelect(0b111, false_block_ans_id, 1,
                    expression_ptr->children_[1]->integrated_type_, 0,
                    expression_ptr->children_[1]->integrated_type_, 0);
              } else {
                function.blocks_[block_stack_.back()].AddSelect(0b111, false_block_ans_id, 1,
                    expression_ptr->children_[1]->integrated_type_, 1,
                    expression_ptr->children_[1]->integrated_type_, 1);
              }
            } else {
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              function.blocks_[block_stack_.back()].AddSelect(0b011, false_block_ans_id,
                  expression_ptr->children_[1]->IR_ID_, expression_ptr->children_[1]->integrated_type_,
                  1, expression_ptr->children_[1]->integrated_type_, 0);
            }
            function.blocks_[block_stack_.back()].AddUnconditionalBranch(merged_label);
            // merge
            block_stack_.back() = merged_label;
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddSelect(0b000, expression_ptr->IR_ID_,
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
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddSelect(0b100, expression_ptr->IR_ID_, 1,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_);
            } else {
              // false && _ = false
              expression_ptr->IR_ID_ = function.var_id_++;
              function.blocks_[block_stack_.back()].AddSelect(0b111, expression_ptr->IR_ID_, 1,
                  expression_ptr->children_[0]->integrated_type_, 0,
                  expression_ptr->children_[0]->integrated_type_, 0);
            }
          } else {
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            // a && b : if a == true, expression = b; if a == false, expression = false.
            const int if_true_block_label = function.var_id_++;
            function.blocks_[if_true_block_label] = IRBlock();
            const int if_false_block_label = function.var_id_++;
            function.blocks_[if_false_block_label] = IRBlock();
            const int merged_label = function.var_id_++;
            function.blocks_[merged_label] = IRBlock();
            function.blocks_[block_stack_.back()].AddConditionalBranch(expression_ptr->children_[0]->IR_ID_,
                if_true_block_label, if_false_block_label);
            // in if_true_block
            block_stack_.back() = if_true_block_label;
            const int true_block_ans_id = function.var_id_++;
            if (expression_ptr->children_[1]->integrated_type_->is_const) {
              if (expression_ptr->children_[1]->value_.int_value == 0) {
                function.blocks_[block_stack_.back()].AddSelect(0b111, true_block_ans_id, 1,
                    expression_ptr->children_[1]->integrated_type_, 0,
                    expression_ptr->children_[1]->integrated_type_, 0);
              } else {
                function.blocks_[block_stack_.back()].AddSelect(0b111, true_block_ans_id, 1,
                    expression_ptr->children_[1]->integrated_type_, 1,
                    expression_ptr->children_[1]->integrated_type_, 1);
              }
            } else {
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              function.blocks_[block_stack_.back()].AddSelect(0b011, true_block_ans_id,
                  expression_ptr->children_[1]->IR_ID_, expression_ptr->children_[1]->integrated_type_,
                  1, expression_ptr->children_[1]->integrated_type_, 0);
            }
            function.blocks_[block_stack_.back()].AddUnconditionalBranch(merged_label);
            // in if_false_block
            block_stack_.back() = if_false_block_label;
            const int false_block_ans_id = function.var_id_++;
            function.blocks_[block_stack_.back()].AddSelect(0b111, false_block_ans_id, 1,
                expression_ptr->children_[0]->integrated_type_, 0,
                expression_ptr->children_[0]->integrated_type_, 0);
            function.blocks_[block_stack_.back()].AddUnconditionalBranch(merged_label);
            // merge
            block_stack_.back() = merged_label;
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddSelect(0b000, expression_ptr->IR_ID_,
                expression_ptr->children_[0]->IR_ID_, expression_ptr->children_[1]->integrated_type_,
                true_block_ans_id, expression_ptr->children_[1]->integrated_type_,
                false_block_ans_id);
          }
          break;
        }
        case assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int variable_id = expression_ptr->children_[0]->IR_var_ID_;
          const auto basic_type = expression_ptr->children_[1]->integrated_type_->basic_type;
          if (expression_ptr->children_[1]->integrated_type_->is_int ||
              basic_type == bool_type || basic_type == enumeration_type) {
            if (expression_ptr->children_[1]->integrated_type_->is_const) {
              function.blocks_[block_stack_.back()].AddValueStore(expression_ptr->children_[0]->integrated_type_,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value), variable_id);
            } else {
              expression_ptr->children_[1]->Accept(this);
              if (expression_ptr->children_[1]->IR_ID_ == -1) {
                expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
                functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                    expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
              }
              function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
                  expression_ptr->children_[1]->IR_ID_, variable_id);
            }
          } else if (basic_type == array_type || basic_type == struct_type || basic_type == pointer_type) {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddVariableStore(
                expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_ID_,
                variable_id);
          }
          break;
        }
        case add_assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int pointer_id = expression_ptr->children_[0]->IR_var_ID_;
          const int loaded_var_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                add_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                add_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case minus_assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int pointer_id = expression_ptr->children_[0]->IR_var_ID_;
          const int loaded_var_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                sub_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                sub_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case multiply_assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int pointer_id = expression_ptr->children_[0]->IR_var_ID_;
          const int loaded_var_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                mul_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                mul_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case divide_assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int pointer_id = expression_ptr->children_[0]->IR_var_ID_;
          const int loaded_var_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
                expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) {
              function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                  udiv_, temp_result_id, loaded_var_id,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else {
              function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                sdiv_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            }
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
                expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) {
              function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                udiv_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
            } else {
              function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                sdiv_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
            }
          }
          function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case mod_assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int pointer_id = expression_ptr->children_[0]->IR_var_ID_;
          const int loaded_var_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
                expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) {
              function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                  urem_, temp_result_id, loaded_var_id,
                  static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            } else {
              function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                srem_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
            }
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            if (expression_ptr->children_[0]->integrated_type_->basic_type == u32_type ||
                expression_ptr->children_[0]->integrated_type_->basic_type == usize_type) {
              function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                urem_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
            } else {
              function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                srem_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
            }
          }
          function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case bitwise_and_assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int pointer_id = expression_ptr->children_[0]->IR_var_ID_;
          const int loaded_var_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                and_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                and_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case bitwise_or_assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int pointer_id = expression_ptr->children_[0]->IR_var_ID_;
          const int loaded_var_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                or_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                or_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case bitwise_xor_assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int pointer_id = expression_ptr->children_[0]->IR_var_ID_;
          const int loaded_var_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                xor_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                xor_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case left_shift_assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int pointer_id = expression_ptr->children_[0]->IR_var_ID_;
          const int loaded_var_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                shl_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                shl_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case right_shift_assign: {
          expression_ptr->children_[0]->Accept(this);
          if (expression_ptr->children_[0]->IR_ID_ == -1) {
            expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
            functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
          }
          const int pointer_id = expression_ptr->children_[0]->IR_var_ID_;
          const int loaded_var_id = function.var_id_++;
          function.blocks_[block_stack_.back()].AddLoad(loaded_var_id, expression_ptr->children_[0]->integrated_type_,
              pointer_id);
          const int temp_result_id = function.var_id_++;
          if (expression_ptr->children_[1]->integrated_type_->is_const) {
            function.blocks_[block_stack_.back()].AddVarConstBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                ashr_, temp_result_id, loaded_var_id,
                static_cast<int>(expression_ptr->children_[1]->value_.int_value));
          } else {
            expression_ptr->children_[1]->Accept(this);
            if (expression_ptr->children_[1]->IR_ID_ == -1) {
              expression_ptr->children_[1]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[1]->IR_ID_,
                  expression_ptr->children_[1]->integrated_type_, expression_ptr->children_[1]->IR_var_ID_);
            }
            function.blocks_[block_stack_.back()].AddTwoVarBinaryOperation(expression_ptr->children_[0]->integrated_type_,
                ashr_, temp_result_id, loaded_var_id,
                expression_ptr->children_[1]->IR_ID_);
          }
          function.blocks_[block_stack_.back()].AddVariableStore(expression_ptr->children_[0]->integrated_type_,
              temp_result_id, pointer_id);
          break;
        }
        case type_cast: {
          // expression_ptr->children[0]->integrated_type->is_const is always false, otherwise the whole expression is const.
          if (expression_ptr->children_[0]->integrated_type_->is_int) {
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = expression_ptr->children_[0]->IR_ID_;
          } else { // expression_ptr->children_[0]->integrated_type_->basic_type == bool_type
            expression_ptr->children_[0]->Accept(this);
            if (expression_ptr->children_[0]->IR_ID_ == -1) {
              expression_ptr->children_[0]->IR_ID_ = functions_[wrapping_functions_.back()].var_id_++;
              functions_[wrapping_functions_.back()].blocks_[block_stack_.back()].AddLoad(expression_ptr->children_[0]->IR_ID_,
                  expression_ptr->children_[0]->integrated_type_, expression_ptr->children_[0]->IR_var_ID_);
            }
            expression_ptr->IR_ID_ = function.var_id_++;
            function.blocks_[block_stack_.back()].AddSelect(0b011, expression_ptr->IR_ID_,
                expression_ptr->children_[0]->IR_ID_, expression_ptr->integrated_type_,
                1, expression_ptr->integrated_type_, 0);
          }
          break;
        }
        default:;
      }
    }
  }
}

void IRVisitor::OutputType(std::ofstream &file, const std::shared_ptr<IntegratedType> &integrated_type) {
  switch (integrated_type->basic_type) {
    case bool_type: {
      file << "i1";
      break;
    }
    case i32_type:
    case u32_type:
    case isize_type:
    case usize_type:
    case enumeration_type: {
      file << "i32";
      break;
    }
    case array_type: {
      file << '[' << integrated_type->size << " x ";
      OutputType(file, integrated_type->element_type);
      file << ']';
      break;
    }
    case struct_type: {
      file << "%struct." << integrated_type->struct_node->IR_ID_;
      break;
    }
    case pointer_type: {
      file << "ptr";
      break;
    }
    default:;
  }
}

void IRVisitor::Print(std::ofstream &file, const IRInstruction &instruction) {
  file << '\t';
  switch (instruction.instruction_type_) {
    case two_var_binary_operation_: {
      file << "%var." << instruction.result_id_ << " = ";
      switch (instruction.operator_) {
        case add_: {
          file << "add";
          break;
        }
        case sub_: {
          file << "sub";
          break;
        }
        case mul_: {
          file << "mul";
          break;
        }
        case udiv_: {
          file << "udiv";
          break;
        }
        case sdiv_: {
          file << "sdiv";
          break;
        }
        case urem_: {
          file << "urem";
          break;
        }
        case srem_: {
          file << "srem";
          break;
        }
        case shl_: {
          file << "shl";
          break;
        }
        case ashr_: {
          file << "ashr";
          break;
        }
        case and_: {
          file << "and";
          break;
        }
        case or_: {
          file << "or";
          break;
        }
        case xor_: {
          file << "xor";
          break;
        }
        default:;
      }
      file << " ";
      OutputType(file, instruction.result_type_);
      file << " %var." << instruction.operand_1_id_ << ", %var." << instruction.operand_2_id_;
      break;
    }
    case var_const_binary_operation_: {
      file << "%var." << instruction.result_id_ << " = ";
      switch (instruction.operator_) {
        case add_: {
          file << "add";
          break;
        }
        case sub_: {
          file << "sub";
          break;
        }
        case mul_: {
          file << "mul";
          break;
        }
        case udiv_: {
          file << "udiv";
          break;
        }
        case sdiv_: {
          file << "sdiv";
          break;
        }
        case urem_: {
          file << "urem";
          break;
        }
        case srem_: {
          file << "srem";
          break;
        }
        case shl_: {
          file << "shl";
          break;
        }
        case ashr_: {
          file << "ashr";
          break;
        }
        case and_: {
          file << "and";
          break;
        }
        case or_: {
          file << "or";
          break;
        }
        case xor_: {
          file << "xor";
          break;
        }
        default:;
      }
      file << " ";
      OutputType(file, instruction.result_type_);
      file << " %var." << instruction.operand_1_id_ << ", " << instruction.operand_2_id_;
      break;
    }
    case const_var_binary_operation_: {
      file << "%var." << instruction.result_id_ << " = ";
      switch (instruction.operator_) {
        case add_: {
          file << "add";
          break;
        }
        case sub_: {
          file << "sub";
          break;
        }
        case mul_: {
          file << "mul";
          break;
        }
        case udiv_: {
          file << "udiv";
          break;
        }
        case sdiv_: {
          file << "sdiv";
          break;
        }
        case urem_: {
          file << "urem";
          break;
        }
        case srem_: {
          file << "srem";
          break;
        }
        case shl_: {
          file << "shl";
          break;
        }
        case ashr_: {
          file << "ashr";
          break;
        }
        case and_: {
          file << "and";
          break;
        }
        case or_: {
          file << "or";
          break;
        }
        case xor_: {
          file << "xor";
          break;
        }
        default:;
      }
      file << " ";
      OutputType(file, instruction.result_type_);
      file << " " << instruction.operand_1_id_ << ", %var." << instruction.operand_2_id_;
      break;
    }
    case conditional_br_: {
      file << "br i1 %var." << instruction.condition_id_ << ", label %label_" <<
          instruction.if_true_ << ", label %label_" << instruction.if_false_;
      break;
    }
    case unconditional_br_: {
      file << "br label %label_" << instruction.destination_;
      break;
    }
    case value_ret_: {
      file << "ret ";
      OutputType(file, instruction.result_type_);
      file << " " << instruction.result_id_;
      break;
    }
    case variable_ret_: {
      file << "ret ";
      OutputType(file, instruction.result_type_);
      file << " %var." << instruction.result_id_;
      break;
    }
    case void_ret_: {
      file << "ret void";
      break;
    }
    case alloca_: {
      file << "%var." << instruction.result_id_ << " = alloca ";
      OutputType(file, instruction.result_type_);
      break;
    }
    case load_: {
      file << "%var." << instruction.result_id_ << " = load ";
      OutputType(file, instruction.result_type_);
      file << ", ptr %var." << instruction.pointer_;
      break;
    }
    case ptr_load_: {
      file << "%var." << instruction.result_id_ <<
          " = load ptr, ptr %var." << instruction.pointer_;
      break;
    }
    case variable_store_: {
      file << "store ";
      OutputType(file, instruction.result_type_);
      file << " %var." << instruction.result_id_ << ", ptr %var." << instruction.pointer_;
      break;
    }
    case value_store_: {
      file << "store ";
      OutputType(file, instruction.result_type_);
      file << " " << instruction.result_id_ << ", ptr %var." << instruction.pointer_;
      break;
    }
    case ptr_store_: {
      file << "store ptr %var." << instruction.result_id_
          << ", ptr %var." << instruction.pointer_;
      break;
    }
    case get_element_ptr_by_value_: {
      file << "%var." << instruction.result_id_ << " = getelementptr ";
      OutputType(file, instruction.result_type_);
      file << ", ptr %var." << instruction.pointer_ << ", i32 0, i32 " << instruction.index_;
      break;
    }
    case get_element_ptr_by_variable_: {
      file << "%var." << instruction.result_id_ << " = getelementptr ";
      OutputType(file, instruction.result_type_);
      file << ", ptr %var." << instruction.pointer_ << ", i32 0, i32 %var." << instruction.index_;
      break;
    }
    case two_var_icmp_: {
      file << "%var." << instruction.result_id_ << " = icmp ";
      switch (instruction.icmp_condition_) {
        case equal_: {
          file << "eq";
          break;
        }
        case not_equal_: {
          file << "ne";
          break;
        }
        case unsigned_greater_than_: {
          file << "ugt";
          break;
        }
        case unsigned_greater_equal_: {
          file << "uge";
          break;
        }
        case unsigned_less_than_: {
          file << "ult";
          break;
        }
        case unsigned_less_equal_: {
          file << "ule";
          break;
        }
        case signed_greater_than_: {
          file << "sgt";
          break;
        }
        case signed_greater_equal_: {
          file << "sge";
          break;
        }
        case signed_less_than_: {
          file << "slt";
          break;
        }
        case signed_less_equal_: {
          file << "sle";
          break;
        }
        default:;
      }
      file << " ";
      OutputType(file, instruction.result_type_);
      file << " %var." << instruction.operand_1_id_ << ", %var." <<
          instruction.operand_2_id_;
      break;
    }
    case var_const_icmp_: {
      file << "%var." << instruction.result_id_ << " = icmp ";
      switch (instruction.icmp_condition_) {
        case equal_: {
          file << "eq";
          break;
        }
        case not_equal_: {
          file << "ne";
          break;
        }
        case unsigned_greater_than_: {
          file << "ugt";
          break;
        }
        case unsigned_greater_equal_: {
          file << "uge";
          break;
        }
        case unsigned_less_than_: {
          file << "ult";
          break;
        }
        case unsigned_less_equal_: {
          file << "ule";
          break;
        }
        case signed_greater_than_: {
          file << "sgt";
          break;
        }
        case signed_greater_equal_: {
          file << "sge";
          break;
        }
        case signed_less_than_: {
          file << "slt";
          break;
        }
        case signed_less_equal_: {
          file << "sle";
          break;
        }
        default:;
      }
      file << " ";
      OutputType(file, instruction.result_type_);
      file << " %var." << instruction.operand_1_id_ << ", " <<
          instruction.operand_2_id_;
      break;
    }
    case const_var_icmp_: {
      file << "%var." << instruction.result_id_ << " = icmp ";
      switch (instruction.icmp_condition_) {
        case equal_: {
          file << "eq";
          break;
        }
        case not_equal_: {
          file << "ne";
          break;
        }
        case unsigned_greater_than_: {
          file << "ugt";
          break;
        }
        case unsigned_greater_equal_: {
          file << "uge";
          break;
        }
        case unsigned_less_than_: {
          file << "ult";
          break;
        }
        case unsigned_less_equal_: {
          file << "ule";
          break;
        }
        case signed_greater_than_: {
          file << "sgt";
          break;
        }
        case signed_greater_equal_: {
          file << "sge";
          break;
        }
        case signed_less_than_: {
          file << "slt";
          break;
        }
        case signed_less_equal_: {
          file << "sle";
          break;
        }
        default:;
      }
      file << " ";
      OutputType(file, instruction.result_type_);
      file << " " << instruction.operand_1_id_ << ", %var." <<
          instruction.operand_2_id_;
      break;
    }
    case non_void_call_: {
      file << "%var." << instruction.result_id_ << " = call ";
      OutputType(file, instruction.result_type_);
      file << " @fn." << instruction.function_name_ << "(";
      for (int i = 0; i < instruction.function_call_arguments_.size(); ++i) {
        OutputType(file, instruction.function_call_arguments_[i].type_);
        file << " ";
        if (instruction.function_call_arguments_[i].is_variable_) {
          file << "%var." << instruction.function_call_arguments_[i].value_;
        } else {
          file << instruction.function_call_arguments_[i].value_;
        }
        if (i < instruction.function_call_arguments_.size() - 1) {
          file << ", ";
        }
      }
      file << ")";
      break;
    }
    case void_call_: {
      file << "call void @fn." << instruction.function_name_ << "(";
      for (int i = 0; i < instruction.function_call_arguments_.size(); ++i) {
        OutputType(file, instruction.function_call_arguments_[i].type_);
        file << " ";
        if (instruction.function_call_arguments_[i].is_variable_) {
          file << "%var." << instruction.function_call_arguments_[i].value_;
        } else {
          file << instruction.function_call_arguments_[i].value_;
        }
        if (i < instruction.function_call_arguments_.size() - 1) {
          file << ", ";
        }
      }
      file << ")";
      break;
    }
    case builtin_call_: {
      switch (instruction.function_name_) {
        case 0: { // printInt
          file << "call void @printInt(";
          for (int i = 0; i < instruction.function_call_arguments_.size(); ++i) {
            OutputType(file, instruction.function_call_arguments_[i].type_);
            file << " ";
            if (instruction.function_call_arguments_[i].is_variable_) {
              file << "%var." << instruction.function_call_arguments_[i].value_;
            } else {
              file << instruction.function_call_arguments_[i].value_;
            }
            if (i < instruction.function_call_arguments_.size() - 1) {
              file << ", ";
            }
          }
          file << ")";
          break;
        }
        case 1: { // printlnInt
          file << "call void @printlnInt(";
          for (int i = 0; i < instruction.function_call_arguments_.size(); ++i) {
            OutputType(file, instruction.function_call_arguments_[i].type_);
            file << " ";
            if (instruction.function_call_arguments_[i].is_variable_) {
              file << "%var." << instruction.function_call_arguments_[i].value_;
            } else {
              file << instruction.function_call_arguments_[i].value_;
            }
            if (i < instruction.function_call_arguments_.size() - 1) {
              file << ", ";
            }
          }
          file << ")";
          break;
        }
        case 2: { // getInt
          file << "%var." << instruction.result_id_ << " = call ";
          OutputType(file, instruction.result_type_);
          file << " @getInt(";
          for (int i = 0; i < instruction.function_call_arguments_.size(); ++i) {
            OutputType(file, instruction.function_call_arguments_[i].type_);
            file << " ";
            if (instruction.function_call_arguments_[i].is_variable_) {
              file << "%var." << instruction.function_call_arguments_[i].value_;
            } else {
              file << instruction.function_call_arguments_[i].value_;
            }
            if (i < instruction.function_call_arguments_.size() - 1) {
              file << ", ";
            }
          }
          file << ")";
          break;
        }
        default:;
      }
      break;
    }
    case phi_: {
      IRThrow("No phi function.");
      break;
    }
    case value_select_ii_: {
      file << "%var." << instruction.result_id_ << " = select i1 ";
      if (instruction.condition_id_ == 0) {
        file << "false, ";
        OutputType(file, instruction.another_type_);
        file << " %var." << instruction.operand_2_id_ << ", ";
        OutputType(file, instruction.another_type_);
        file << " %var." << instruction.operand_2_id_;
      } else {
        file << "true, ";
        OutputType(file, instruction.result_type_);
        file << " %var." << instruction.operand_1_id_ << ", ";
        OutputType(file, instruction.result_type_);
        file << " %var." << instruction.operand_1_id_;
      }
      break;
    }
    case value_select_iv_: {
      file << "%var." << instruction.result_id_ << " = select i1 ";
      if (instruction.condition_id_ == 0) {
        file << "false, ";
        OutputType(file, instruction.another_type_);
        file << " " << instruction.operand_2_id_ << ", ";
        OutputType(file, instruction.another_type_);
        file << " " << instruction.operand_2_id_;
      } else {
        file << "true, ";
        OutputType(file, instruction.result_type_);
        file << " %var." << instruction.operand_1_id_ << ", ";
        OutputType(file, instruction.result_type_);
        file << " %var." << instruction.operand_1_id_;
      }
      break;
    }
    case value_select_vi_: {
      file << "%var." << instruction.result_id_ << " = select i1 ";
      if (instruction.condition_id_ == 0) {
        file << "false, ";
        OutputType(file, instruction.another_type_);
        file << " %var." << instruction.operand_2_id_ << ", ";
        OutputType(file, instruction.another_type_);
        file << " %var." << instruction.operand_2_id_;
      } else {
        file << "true, ";
        OutputType(file, instruction.result_type_);
        file << " " << instruction.operand_1_id_ << ", ";
        OutputType(file, instruction.result_type_);
        file << " " << instruction.operand_1_id_;
      }
      break;
    }
    case value_select_vv_: {
      file << "%var." << instruction.result_id_ << " = select i1 ";
      if (instruction.condition_id_ == 0) {
        file << "false, ";
        OutputType(file, instruction.another_type_);
        file << " " << instruction.operand_2_id_ << ", ";
        OutputType(file, instruction.another_type_);
        file << " " << instruction.operand_2_id_;
      } else {
        file << "true, ";
        OutputType(file, instruction.result_type_);
        file << " " << instruction.operand_1_id_ << ", ";
        OutputType(file, instruction.result_type_);
        file << " " << instruction.operand_1_id_;
      }
      break;
    }
    case variable_select_ii_: {
      file << "%var." << instruction.result_id_ << " = select i1 ";
      file << "%var." << instruction.condition_id_;
      file << ", ";
      OutputType(file, instruction.result_type_);
      file << " %var." << instruction.operand_1_id_ << ", ";
      OutputType(file, instruction.another_type_);
      file << " %var." << instruction.operand_2_id_;
      break;
    }
    case variable_select_iv_: {
      file << "%var." << instruction.result_id_ << " = select i1 ";
      file << "%var." << instruction.condition_id_;
      file << ", ";
      OutputType(file, instruction.result_type_);
      file << " %var." << instruction.operand_1_id_ << ", ";
      OutputType(file, instruction.another_type_);
      file << " " << instruction.operand_2_id_;
      break;
    }
    case variable_select_vi_: {
      file << "%var." << instruction.result_id_ << " = select i1 ";
      file << "%var." << instruction.condition_id_;
      file << ", ";
      OutputType(file, instruction.result_type_);
      file << " " << instruction.operand_1_id_ << ", ";
      OutputType(file, instruction.another_type_);
      file << " %var." << instruction.operand_2_id_;
      break;
    }
    case variable_select_vv_: {
      file << "%var." << instruction.result_id_ << " = select i1 ";
      file << "%var." << instruction.condition_id_;
      file << ", ";
      OutputType(file, instruction.result_type_);
      file << " " << instruction.operand_1_id_ << ", ";
      OutputType(file, instruction.another_type_);
      file << " " << instruction.operand_2_id_;
      break;
    }
    case builtin_memset_: {
      file << "call void @builtin_memset(ptr %var." << instruction.pointer_ << ", i32 ";
      if (instruction.condition_id_ == 0) {
        file << "%var." << instruction.operand_1_id_;
      } else {
        file << instruction.operand_1_id_;
      }
      file << ", i32 " << instruction.result_id_ << ")";
      break;
    }
    case builtin_memcpy_: {
      file << "call void @builtin_memcpy(ptr %var." << instruction.destination_
          << ", ptr %var." << instruction.pointer_ << ", i32 " << instruction.result_id_ << ")";
      break;
    }
    default:;
  }
  file << '\n';
}

void IRVisitor::Output(std::ofstream &file) {
  // output the builtin function declarations
  std::ifstream builtin_functions("../RCompiler-Testcases/IR-1/builtin/builtin.ll");
  std::string line_in_file;
  if (builtin_functions.is_open()) {
    while (std::getline(builtin_functions, line_in_file)) {
      file << line_in_file << '\n';
    }
    builtin_functions.close();
  } else {
    std::cerr << "Cannot open builtin functions file!\n";
  }
  file << "\n";
  // output struct definitions
  for (int i = 0; i < structs_.size(); ++i) {
    file << "%struct." << i << " = type { ";
    for (int j = 0; j < structs_[i].element_type_.size(); ++j) {
      OutputType(file, structs_[i].element_type_[j]);
      if (j < structs_[i].element_type_.size() - 1) {
        file << ',';
      }
      file << ' ';
    }
    file << "}\n";
  }
  file << '\n';
  // output function definitions with blocks in it
  for (int i = 0; i < functions_.size(); ++i) {
    file << "define ";
    if (functions_[i].return_type_->basic_type == unit_type) {
      file << "void";
    } else {
      OutputType(file, functions_[i].return_type_);
    }
    if (i == main_function_id_) {
      file << " @main(";
    } else {
      file << " @fn." << i << '(';
    }
    for (int j = 0; j < functions_[i].parameter_types_.size(); ++j) {
      OutputType(file, functions_[i].parameter_types_[j]);
      file << " %var." << j;
      if (j < functions_[i].parameter_types_.size() - 1) {
        file << ", ";
      }
    }
    file << ") {\n";
    // print alloca
    file << "alloca:\n";
    for (const auto &alloca_instruction : functions_[i].alloca_instructions_) {
      Print(file, alloca_instruction);
    }
    file << "\tbr label %label_" << functions_[i].blocks_.begin()->first << '\n';
    // print blocks
    for (const auto &[block_label, block] : functions_[i].blocks_) {
      file << "label_" << block_label << ":\n";
      for (const auto &instruction : block.instructions_) {
        Print(file, instruction);
      }
    }
    file << "}\n\n";
  }
}