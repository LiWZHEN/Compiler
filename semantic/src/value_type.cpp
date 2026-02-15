#include "value_type.h"
#include "item.h"
#include "functions.h"
#include "function_parameters.h"
#include "expression.h"
#include "type.h"
#include "statements.h"
#include "structs.h"
#include <unordered_set>

void Throw(const std::string &info) {
  std::cerr << "Error in value_type check: " << info << '\n';
  throw "";
}

void TryToMatch(const std::shared_ptr<IntegratedType> &target_type, std::shared_ptr<IntegratedType> &expr_type,
    const bool need_const) {
  if (target_type->basic_type == unknown_type || expr_type->basic_type == unknown_type) {
    Throw("Unknown type is incomparable.");
  }
  if (target_type->basic_type == never_type || expr_type->basic_type == never_type) {
    return;
  }
  if (target_type->basic_type != expr_type->basic_type) {
    if (!target_type->is_int || !expr_type->is_int) {
      Throw("Type mismatch.");
    }
    // different but both int
    if (expr_type->possible_types.contains(i32_type) && !target_type->possible_types.contains(i32_type)) {
      expr_type->RemovePossibility(i32_type);
    }
    if (expr_type->possible_types.contains(u32_type) && !target_type->possible_types.contains(u32_type)) {
      expr_type->RemovePossibility(u32_type);
    }
    if (expr_type->possible_types.contains(isize_type) && !target_type->possible_types.contains(isize_type)) {
      expr_type->RemovePossibility(isize_type);
    }
    if (expr_type->possible_types.contains(usize_type) && !target_type->possible_types.contains(usize_type)) {
      expr_type->RemovePossibility(usize_type);
    }
    // be able to fit
  } else {// basic type matched
    if (target_type->basic_type == array_type) {
      if (target_type->size != expr_type->size) {
        Throw("Type mismatch.");
      }
      TryToMatch(target_type->element_type, expr_type->element_type, need_const);
    } else if (target_type->basic_type == struct_type || target_type->basic_type == enumeration_type) {
      if (target_type->struct_node != expr_type->struct_node) {
        Throw("Type mismatch.");
      }
    } else if (target_type->basic_type == pointer_type) {
      TryToMatch(target_type->element_type, expr_type->element_type, need_const);
    }
  }
  if (need_const && !expr_type->is_const) {
    Throw("Type mismatch.");
  }
}

void CheckOverflow(const long long value, const std::shared_ptr<IntegratedType> &integrated) {
  if (value > 2147483647 || value < -2147483648) {
    integrated->RemovePossibility(i32_type);
  }
  if (value > 4294967295) {
    integrated->RemovePossibility(u32_type);
  }
  if (value < 0) {
    integrated->RemovePossibility(u32_type);
    integrated->RemovePossibility(usize_type);
  }
}

bool Value::operator==(const Value &other) const {
  if (int_value != other.int_value) {
    return false;
  }
  if (array_values.size() != other.array_values.size()) {
    return false;
  }
  for (int i = 0; i < array_values.size(); ++i) {
    if (array_values[i] != other.array_values[i]) {
      return false;
    }
  }
  if (str_value != other.str_value) {
    return false;
  }
  if (struct_values == other.struct_values) {
    return false;
  }
  return pointer_value->value_ == other.pointer_value->value_;
}

void ValueTypeVisitor::Visit(Implementation *implementation_ptr) {}
void ValueTypeVisitor::Visit(Keyword *keyword_ptr) {}
void ValueTypeVisitor::Visit(Identifier *identifier_ptr){}
void ValueTypeVisitor::Visit(Punctuation *punctuation_ptr){}
void ValueTypeVisitor::Visit(ShorthandSelf *shorthand_self_ptr){}
void ValueTypeVisitor::Visit(Pattern *pattern_ptr){}
void ValueTypeVisitor::Visit(ReferencePattern *reference_pattern_ptr){}
void ValueTypeVisitor::Visit(IdentifierPattern *identifier_pattern_ptr){}
void ValueTypeVisitor::Visit(PathInExpression *path_in_expression_ptr){}
void ValueTypeVisitor::Visit(Enumeration *enumeration_ptr) {}
void ValueTypeVisitor::Visit(Trait *trait_ptr) {}
void ValueTypeVisitor::Visit(CharLiteral *char_literal_ptr){}
void ValueTypeVisitor::Visit(StringLiteral *string_literal_ptr){}
void ValueTypeVisitor::Visit(RawStringLiteral *raw_string_literal_ptr){}
void ValueTypeVisitor::Visit(CStringLiteral *c_string_literal_ptr){}
void ValueTypeVisitor::Visit(RawCStringLiteral *raw_c_string_literal_ptr){}
void ValueTypeVisitor::Visit(IntegerLiteral *integer_literal_ptr){}
void ValueTypeVisitor::Visit(EnumVariants *enum_variants_ptr){}
void ValueTypeVisitor::Visit(StructFields *struct_fields_ptr){}
void ValueTypeVisitor::Visit(AssociatedItem *associated_item_ptr){}
void ValueTypeVisitor::Visit(PathExprSegment *path_expr_segment_ptr) {}
void ValueTypeVisitor::Visit(Crate *crate_ptr) {
  for (const auto it : crate_ptr->children_) {
    it->Accept(this);
  }
  if (crate_ptr->scope_node_->FindInValue("main").node == nullptr) {
    Throw("There is no outermost main function.");
  }
}
void ValueTypeVisitor::Visit(Item *item_ptr) {
  item_ptr->children_[0]->Accept(this);
}
void ValueTypeVisitor::Visit(Function *function_ptr) {
  for (int i = 0 ; i < function_ptr->children_.size(); ++i) {
    if (function_ptr->type_[i] != type_function_return_type) {
      continue;
    }
    is_reading_type_ = true;
    type_owner_ = function_ptr;
    function_ptr->integrated_type_ = std::make_shared<IntegratedType>();
    function_ptr->children_[i]->Accept(this);
    function_ptr->integrated_type_->type_completed = true;
    is_reading_type_ = false;
    type_owner_ = nullptr;
  }
  if (function_ptr->integrated_type_ == nullptr) {
    function_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
        false, false, false, true, 0);
  } else if (function_ptr->integrated_type_->basic_type == unknown_type) {
    function_ptr->integrated_type_->basic_type = unit_type;
  }
  int block_expr_ind = -1;
  for (int i = 0; i < function_ptr->children_.size(); ++i) {
    if (function_ptr->type_[i] == type_function_return_type) {
      continue;
    }
    if (function_ptr->type_[i] == type_block_expression) {
      block_expr_ind = i;
      wrapping_function_.push_back({function_ptr, type_function});
      function_ptr->children_[i]->Accept(this);
      wrapping_function_.pop_back();
      continue;
    }
    function_ptr->children_[i]->Accept(this);
  }
  if (block_expr_ind != -1) {
    TryToMatch(function_ptr->integrated_type_,
        function_ptr->children_[block_expr_ind]->integrated_type_, false);
  }
  std::string function_name = dynamic_cast<LeafNode *>(function_ptr->children_[1])
      ->GetContent().GetStr();
  if (function_name == "main" && wrapping_function_.empty()) {
    if (block_expr_ind == -1 || function_ptr->children_[block_expr_ind]->children_.size() == 2) {
      Throw("Unexpected block expression missing in outermost main function.");
    }
    auto statements_ptr = function_ptr->children_[block_expr_ind]->children_[1];
    if (statements_ptr->type_[statements_ptr->children_.size() - 1] == type_statement) {
      auto statement_ptr = statements_ptr->children_[statements_ptr->children_.size() - 1];
      if (statement_ptr->type_[0] != type_expression_statement) {
        Throw("Expect exit function at the end of outermost main function.");
      }
      auto expr_ptr = dynamic_cast<Expression *>(statement_ptr->children_[0]->children_[0]);
      if (expr_ptr->GetExprType() != call_expr) {
        Throw("Expect exit function at the end of outermost main function.");
      }
      if (dynamic_cast<LeafNode *>(expr_ptr->children_[0]->children_[0])->GetContent().GetStr() != "exit") {
        Throw("Expect exit function at the end of outermost main function.");
      }
    } else {
      auto expr_ptr = dynamic_cast<Expression *>(statements_ptr->children_[statements_ptr->children_.size() - 1]);
      if (expr_ptr->GetExprType() != call_expr) {
        Throw("Expect exit function at the end of outermost main function.");
      }
      if (dynamic_cast<LeafNode *>(expr_ptr->children_[0]->children_[0])->GetContent().GetStr() != "exit") {
        Throw("Expect exit function at the end of outermost main function.");
      }
    }
  }
}
void ValueTypeVisitor::Visit(Struct *struct_ptr) {
  this->wrapping_structs_.push_back({struct_ptr, type_struct});
  for (const auto &it : struct_ptr->field_items_) {
    it.second.node->Accept(this);
  }
  for (const auto &it : struct_ptr->associated_items_) {
    it.second.node->Accept(this);
  }
  this->wrapping_structs_.pop_back();
}
void ValueTypeVisitor::Visit(FunctionParameters *function_parameters_ptr) {
  for (const auto it : function_parameters_ptr->children_) {
    it->Accept(this);
  }
}
void ValueTypeVisitor::Visit(FunctionReturnType *function_return_type_ptr) {
  function_return_type_ptr->children_[1]->Accept(this);
}
void ValueTypeVisitor::Visit(ConstantItem *constant_item_ptr) {
  constant_item_ptr->integrated_type_ = std::make_shared<IntegratedType>();
  is_reading_type_ = true;
  type_owner_ = constant_item_ptr->children_[1];
  constant_item_ptr->children_[3]->Accept(this); // visit Type
  constant_item_ptr->integrated_type_->type_completed = true;
  is_reading_type_ = false;
  type_owner_ = nullptr;
  constant_item_ptr->children_[1]->integrated_type_->is_const = true;
  if (constant_item_ptr->children_.size() < 6) {
    Throw("Missing Expression.");
  }
  constant_item_ptr->children_[5]->Accept(this); // visit Expression
  TryToMatch(constant_item_ptr->children_[1]->integrated_type_,
      constant_item_ptr->children_[5]->integrated_type_, true);
  constant_item_ptr->children_[1]->value_ = constant_item_ptr->children_[5]->value_;
}
void ValueTypeVisitor::Visit(BlockExpression *block_expression_ptr) {
  if (block_expression_ptr->children_.size() == 2) {
    block_expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type, true,
        false, false, true, 0);
    return;
  }
  block_expression_ptr->children_[1]->Accept(this); // visit Statements
  block_expression_ptr->integrated_type_ = block_expression_ptr->children_[1]->integrated_type_;
}
void ValueTypeVisitor::Visit(SelfParam *self_param_ptr) {
  if (this->wrapping_structs_.empty()) {
    Throw("'self' parameter is only allowed in associated functions.");
  }
  self_param_ptr->integrated_type_ = std::make_shared<IntegratedType>();
  auto target_struct = wrapping_structs_.back();
  switch (self_param_ptr->children_[0]->children_.size()) {
    case 1: { // self
      switch (target_struct.node_type) {
        case type_struct: {
          self_param_ptr->integrated_type_->struct_node = target_struct.node;
          self_param_ptr->integrated_type_->basic_type = struct_type;
          break;
        }
        case type_enumeration: {
          self_param_ptr->integrated_type_->struct_node = target_struct.node;
          self_param_ptr->integrated_type_->basic_type = enumeration_type;
          break;
        }
        default: {
          Throw("Invalid 'self'.");
        }
      }
      break;
    }
    case 2: { // &self
      self_param_ptr->integrated_type_->basic_type = pointer_type;
      switch (target_struct.node_type) {
        case type_struct: {
          self_param_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(struct_type,
              false, false, false, true, 0);
          self_param_ptr->integrated_type_->element_type->struct_node = target_struct.node;
          break;
        }
        case type_enumeration: {
          self_param_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(enumeration_type,
              false, false, false, true, 0);
          self_param_ptr->integrated_type_->element_type->struct_node = target_struct.node;
          break;
        }
        default: {
          Throw("Invalid 'self'.");
        }
      }
      break;
    }
    case 3: { // &mut self
      self_param_ptr->integrated_type_->basic_type = pointer_type;
      switch (target_struct.node_type) {
        case type_struct: {
          self_param_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(struct_type,
              false, true, false, true, 0);
          self_param_ptr->integrated_type_->element_type->struct_node = target_struct.node;
          break;
        }
        case type_enumeration: {
          self_param_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(enumeration_type,
              false, true, false, true, 0);
          self_param_ptr->integrated_type_->element_type->struct_node = target_struct.node;
          break;
        }
        default: {
          Throw("Invalid 'self'.");
        }
      }
      break;
    }
    default:;
  }
  self_param_ptr->integrated_type_->type_completed = true;
}
void ValueTypeVisitor::Visit(FunctionParam *function_param_ptr) {
  function_param_ptr->integrated_type_ = std::make_shared<IntegratedType>();
  auto identifier_pattern_ptr = function_param_ptr->children_[0]->children_[0];
  if (identifier_pattern_ptr->children_.size() != 1) {
    // mut IDENTIFIER
    function_param_ptr->integrated_type_->is_mutable = true;
  }
  is_reading_type_ = true;
  type_owner_ = function_param_ptr;
  function_param_ptr->children_[2]->Accept(this);
  function_param_ptr->integrated_type_->type_completed = true;
  is_reading_type_ = false;
  type_owner_ = nullptr;
}
void ValueTypeVisitor::Visit(Type *type_ptr) {
  if (is_reading_type_ == false) {
    Throw("Type without owner.");
  }
  if (type_owner_->integrated_type_ == nullptr) {
    type_owner_->integrated_type_ = std::make_shared<IntegratedType>();
  }
  if (type_owner_->integrated_type_->type_completed) {
    return;
  }
  type_ptr->children_[0]->Accept(this);
}
void ValueTypeVisitor::Visit(UnitType *unit_type_ptr) {
  auto current_type_ptr = &type_owner_->integrated_type_;
  while ((*current_type_ptr)->basic_type != unknown_type) {
    switch ((*current_type_ptr)->basic_type) {
      case bool_type:
      case i32_type:
      case u32_type:
      case isize_type:
      case usize_type:
      case char_type:
      case str_type:
      case string_type:
      case unit_type:
      case struct_type:
      case enumeration_type: {
        Throw("Invalid type.");
      }
      case array_type:
      case pointer_type: {
        current_type_ptr = &(*current_type_ptr)->element_type;
        if (*current_type_ptr == nullptr) {
          Throw("Invalid type.");
        }
        break;
      }
      default:;
    }
  }
  (*current_type_ptr)->basic_type = unit_type;
  (*current_type_ptr)->type_completed = true;
}
void ValueTypeVisitor::Visit(ReferenceType *reference_type_ptr) {
  auto current_type_ptr = &type_owner_->integrated_type_;
  while ((*current_type_ptr)->basic_type != unknown_type) {
    switch ((*current_type_ptr)->basic_type) {
      case bool_type:
      case i32_type:
      case u32_type:
      case isize_type:
      case usize_type:
      case char_type:
      case str_type:
      case string_type:
      case unit_type:
      case struct_type:
      case enumeration_type: {
        Throw("Invalid type.");
      }
      case array_type:
      case pointer_type: {
        current_type_ptr = &(*current_type_ptr)->element_type;
        if (*current_type_ptr == nullptr) {
          Throw("Invalid type.");
        }
        break;
      }
      default:;
    }
  }
  (*current_type_ptr)->basic_type = pointer_type;
  (*current_type_ptr)->element_type = std::make_shared<IntegratedType>();
  if (reference_type_ptr->children_.size() == 3) {
    (*current_type_ptr)->element_type->is_mutable = true;
    reference_type_ptr->children_[2]->Accept(this);
  } else {
    reference_type_ptr->children_[1]->Accept(this);
  }
  (*current_type_ptr)->type_completed = true;
}
void ValueTypeVisitor::Visit(ArrayType *array_type_ptr) {
  if (is_reading_type_ == false) {
    Throw("Type without owner.");
  }
  auto current_type_ptr = &type_owner_->integrated_type_;
  while ((*current_type_ptr)->basic_type != unknown_type) {
    switch ((*current_type_ptr)->basic_type) {
      case bool_type:
      case i32_type:
      case u32_type:
      case isize_type:
      case usize_type:
      case char_type:
      case str_type:
      case string_type:
      case unit_type:
      case struct_type:
      case enumeration_type: {
        Throw("Invalid type.");
      }
      case array_type:
      case pointer_type: {
        current_type_ptr = &(*current_type_ptr)->element_type;
        if (*current_type_ptr == nullptr) {
          Throw("Invalid type.");
        }
        break;
      }
      default:;
    }
  }
  (*current_type_ptr)->basic_type = array_type;
  (*current_type_ptr)->element_type = std::make_shared<IntegratedType>();
  array_type_ptr->children_[1]->Accept(this);
  array_type_ptr->children_[3]->Accept(this);
  const auto target_size_ptr = std::make_shared<IntegratedType>(usize_type, true,
      false, true, true, 0);
  target_size_ptr->RemovePossibility(u32_type);
  target_size_ptr->RemovePossibility(i32_type);
  target_size_ptr->RemovePossibility(isize_type);
  TryToMatch(target_size_ptr, array_type_ptr->children_[3]->integrated_type_, true);
  (*current_type_ptr)->size = array_type_ptr->children_[3]->value_.int_value;
  (*current_type_ptr)->type_completed = true;
}
void ValueTypeVisitor::Visit(TypePath *type_path_ptr) {
  if (is_reading_type_ == false) {
    Throw("Type without owner.");
  }
  auto current_type_ptr = &type_owner_->integrated_type_;
  while ((*current_type_ptr)->basic_type != unknown_type) {
    switch ((*current_type_ptr)->basic_type) {
      case bool_type:
      case i32_type:
      case u32_type:
      case isize_type:
      case usize_type:
      case char_type:
      case str_type:
      case unit_type:
      case struct_type:
      case enumeration_type: {
        Throw("Invalid type.");
      }
      case array_type:
      case pointer_type: {
        current_type_ptr = &(*current_type_ptr)->element_type;
        if (*current_type_ptr == nullptr) {
          Throw("Invalid type.");
        }
        break;
      }
      default:;
    }
  }
  std::string type_name = type_path_ptr->GetContent().GetStr();
  if (type_name == "i32") {
    (*current_type_ptr)->basic_type = i32_type;
    (*current_type_ptr)->is_int = true;
    (*current_type_ptr)->RemovePossibility(u32_type);
    (*current_type_ptr)->RemovePossibility(isize_type);
    (*current_type_ptr)->RemovePossibility(usize_type);
  } else if (type_name == "u32") {
    (*current_type_ptr)->basic_type = u32_type;
    (*current_type_ptr)->is_int = true;
    (*current_type_ptr)->RemovePossibility(i32_type);
    (*current_type_ptr)->RemovePossibility(isize_type);
    (*current_type_ptr)->RemovePossibility(usize_type);
  } else if (type_name == "isize") {
    (*current_type_ptr)->basic_type = isize_type;
    (*current_type_ptr)->is_int = true;
    (*current_type_ptr)->RemovePossibility(i32_type);
    (*current_type_ptr)->RemovePossibility(u32_type);
    (*current_type_ptr)->RemovePossibility(usize_type);
  } else if (type_name == "usize") {
    (*current_type_ptr)->basic_type = usize_type;
    (*current_type_ptr)->is_int = true;
    (*current_type_ptr)->RemovePossibility(i32_type);
    (*current_type_ptr)->RemovePossibility(u32_type);
    (*current_type_ptr)->RemovePossibility(isize_type);
  } else if (type_name == "bool") {
    (*current_type_ptr)->basic_type = bool_type;
  } else if (type_name == "char") {
    (*current_type_ptr)->basic_type = char_type;
  } else if (type_name == "str") {
    (*current_type_ptr)->basic_type = str_type;
  } else if (type_name == "String") {
    (*current_type_ptr)->basic_type = string_type;
  } else if (type_name == "Self") {
    if (wrapping_structs_.empty()) {
      Throw("'Self' is undefined outside a struct.");
    }
    (*current_type_ptr)->basic_type = struct_type;
    (*current_type_ptr)->struct_node = wrapping_structs_.back().node;
  } else {
    auto type_node = type_path_ptr->scope_node_->FindInType(type_name);
    if (type_node.node == nullptr) {
      Throw("Undefined type name.");
    }
    if (type_node.node_type == type_struct) {
      (*current_type_ptr)->basic_type = struct_type;
    } else if (type_node.node_type == type_enumeration) {
      (*current_type_ptr)->basic_type = enumeration_type;
    } else {
      Throw("Invalid type.");
    }
    (*current_type_ptr)->struct_node = type_node.node;
  }
  (*current_type_ptr)->type_completed = true;
}
void ValueTypeVisitor::Visit(Statements *statements_ptr) {
  for (const auto it : statements_ptr->children_) {
    it->Accept(this);
  }
  for (const auto it : statements_ptr->children_) {
    if (it->integrated_type_->basic_type == never_type) {
      statements_ptr->integrated_type_ = it->integrated_type_;
      return;
    }
  }
  if (statements_ptr->type_.back() == type_expression) {
    statements_ptr->integrated_type_ = std::make_shared<IntegratedType>();
    *statements_ptr->integrated_type_ = *statements_ptr->children_.back()->integrated_type_;
    statements_ptr->integrated_type_->is_const = false;
  } else {
    auto statement_ptr = statements_ptr->children_.back();
    if (statement_ptr->type_[0] == type_expression_statement) {
      if (statement_ptr->children_[0]->children_.size() == 1) {
        statements_ptr->integrated_type_ = statement_ptr->children_[0]->children_[0]->integrated_type_;
      } else {
        statements_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
            false, false, false, true, 0);
      }
    } else {
      statements_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
          false, false, false, true, 0);
    }
  }
}
void ValueTypeVisitor::Visit(Statement *statement_ptr) {
  statement_ptr->children_[0]->Accept(this);
  if (statement_ptr->type_[0] == type_expression_statement) {
    statement_ptr->integrated_type_ = statement_ptr->children_[0]->integrated_type_;
  } else {
    statement_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
        false, false, false, true, 0);
  }
}
void ValueTypeVisitor::Visit(LetStatement *let_statement_ptr) {
  let_statement_ptr->integrated_type_ = std::make_shared<IntegratedType>();
  is_reading_type_ = true;
  type_owner_ = let_statement_ptr->children_[1];
  let_statement_ptr->children_[3]->Accept(this); // visit type
  let_statement_ptr->children_[1]->integrated_type_->type_completed = true;
  is_reading_type_ = false;
  type_owner_ = nullptr;
  let_statement_ptr->children_[5]->Accept(this); // visit expression
  TryToMatch(let_statement_ptr->children_[1]->integrated_type_,
      let_statement_ptr->children_[5]->integrated_type_, false);
  auto identifier_pattern_ptr = let_statement_ptr->children_[1]->children_[0];
  std::string identifier_pattern_name;
  if (identifier_pattern_ptr->children_.size() > 1) {
    let_statement_ptr->children_[1]->integrated_type_->is_mutable = true;
    identifier_pattern_name = dynamic_cast<LeafNode *>(identifier_pattern_ptr->children_[1])
        ->GetContent().GetStr();
  } else {
    identifier_pattern_name = dynamic_cast<LeafNode *>(identifier_pattern_ptr->children_[0])
        ->GetContent().GetStr();
  }
  let_statement_ptr->scope_node_->value_namespace[identifier_pattern_name] =
      {let_statement_ptr->children_[1], type_pattern};
}
void ValueTypeVisitor::Visit(ExpressionStatement *expression_statement_ptr) {
  expression_statement_ptr->children_[0]->Accept(this);
  if (expression_statement_ptr->children_.size() > 1) {
    if (expression_statement_ptr->children_[0]->integrated_type_->basic_type == never_type) {
      expression_statement_ptr->integrated_type_ = expression_statement_ptr->children_[0]->integrated_type_;
    } else {
      expression_statement_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
          false, false, false, true, 0);
    }
  } else {
    expression_statement_ptr->integrated_type_ = expression_statement_ptr->children_[0]->integrated_type_;
  }
}
void ValueTypeVisitor::Visit(StructExprFields *struct_expr_fields_ptr) {
  for (const auto it : struct_expr_fields_ptr->children_) {
    it->Accept(this);
  }
}
void ValueTypeVisitor::Visit(StructExprField *struct_expr_field_ptr) {
  struct_expr_field_ptr->children_[2]->Accept(this);
  struct_expr_field_ptr->integrated_type_ = struct_expr_field_ptr->children_[2]->integrated_type_;
}
void ValueTypeVisitor::Visit(StructField *struct_field_ptr) {
  is_reading_type_ = true;
  type_owner_ = struct_field_ptr;
  struct_field_ptr->children_[2]->Accept(this);
  struct_field_ptr->integrated_type_->type_completed = true;
  is_reading_type_ = false;
  type_owner_ = nullptr;
}
void ValueTypeVisitor::Visit(Expression *expression_ptr) {
  switch (expression_ptr->GetExprType()) {
    case block_expr: {
      if (expression_ptr->children_.size() == 2) {
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
            false, false, false, true, 0);
      } else {
        expression_ptr->children_[1]->Accept(this);
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
        *expression_ptr->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
        expression_ptr->integrated_type_->is_const = false;
      }
      break;
    }
    case infinite_loop_expr: {
      if (expression_ptr->children_.size() == 4) {
        wrapping_loop_.push_back({expression_ptr, type_expression});
        expression_ptr->children_[2]->Accept(this);
        wrapping_loop_.pop_back();
        auto target_type = std::make_shared<IntegratedType>(unit_type,
          false, false, false, true, 0);
        TryToMatch(target_type, expression_ptr->children_[2]->integrated_type_, false);
      }
      if (expression_ptr->integrated_type_ == nullptr) {
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(never_type,
            false, false, false, true, 0);
      } else if (expression_ptr->integrated_type_->basic_type == unknown_type) {
        expression_ptr->integrated_type_->basic_type = never_type;
      }
      break;
    }
    case predicate_loop_expr: {
      expression_ptr->children_[2]->Accept(this);
      if (expression_ptr->children_[2]->integrated_type_->basic_type != bool_type) {
        Throw("The expression in condition should be bool type.");
      }
      if (expression_ptr->children_.size() == 7) {
        wrapping_loop_.push_back({expression_ptr, type_expression});
        expression_ptr->children_[5]->Accept(this);
        wrapping_loop_.pop_back();
        auto target_type = std::make_shared<IntegratedType>(unit_type,
          false, false, false, true, 0);
        TryToMatch(target_type, expression_ptr->children_[5]->integrated_type_, false);
      }
      if (expression_ptr->integrated_type_ == nullptr) {
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
            false, false, false, true, 0);
      } else if (expression_ptr->integrated_type_->basic_type == unknown_type) {
        expression_ptr->integrated_type_->basic_type = unit_type;
        expression_ptr->integrated_type_->type_completed = true;
      }
      break;
    }
    case if_expr: {
      expression_ptr->children_[2]->Accept(this);
      if (expression_ptr->children_[2]->integrated_type_->basic_type != bool_type) {
        Throw("The expression in condition should be bool type.");
      }
      int if_statements_ind = -1, else_block_ind = -1;
      for (int i = 5; i < expression_ptr->children_.size(); ++i) {
        if (expression_ptr->type_[i] == type_statements) {
          if_statements_ind = i;
          expression_ptr->children_[i]->Accept(this);
        } else if (expression_ptr->type_[i] == type_expression) {
          else_block_ind = i;
          expression_ptr->children_[i]->Accept(this);
        }
      }
      if (if_statements_ind != -1 && else_block_ind != -1) {
        TryToMatch(expression_ptr->children_[if_statements_ind]->integrated_type_,
            expression_ptr->children_[else_block_ind]->integrated_type_, false);
        if (expression_ptr->children_[else_block_ind]->integrated_type_->basic_type != never_type) {
          expression_ptr->integrated_type_ = expression_ptr->children_[else_block_ind]->integrated_type_;
        } else {
          expression_ptr->integrated_type_ = expression_ptr->children_[if_statements_ind]->integrated_type_;
        }
      } else { // either no 'if' or no 'else', expression has unit type
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
            false, false, false, true, 0);
        if (if_statements_ind != -1) {
          TryToMatch(expression_ptr->children_[if_statements_ind]->integrated_type_,
              expression_ptr->integrated_type_, false);
        } else if (else_block_ind != -1) {
          TryToMatch(expression_ptr->children_[else_block_ind]->integrated_type_,
              expression_ptr->integrated_type_, false);
        }
      }
      break;
    }
    case literal_expr: {
      switch (expression_ptr->type_[0]) {
        case type_keyword: {
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(bool_type,
              true, false, false, true, 0);
          if (dynamic_cast<LeafNode *>(expression_ptr->children_[0])->GetContent().GetStr() == "true") {
            expression_ptr->value_.int_value = 1;
          } else {
            expression_ptr->value_.int_value = 0;
          }
          break;
        }
        case type_char_literal: {
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(char_type,
              true, false, false, true, 0);
          expression_ptr->value_.str_value = dynamic_cast<LeafNode *>(expression_ptr->children_[0])
              ->GetContent().GetCharContent();
          break;
        }
        case type_string_literal:
        case type_raw_string_literal:
        case type_c_string_literal:
        case type_raw_c_string_literal: {
          expression_ptr->children_[0]->integrated_type_ = std::make_shared<IntegratedType>(str_type,
              true, false, false, true, 0);
          expression_ptr->children_[0]->value_.str_value = dynamic_cast<LeafNode *>(expression_ptr->children_[0])
              ->GetContent().GetStringContent();
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(pointer_type,
              true, false, false, true, 0);
          expression_ptr->integrated_type_->element_type = expression_ptr->children_[0]->integrated_type_;
          expression_ptr->value_.pointer_value = expression_ptr->children_[0];
          break;
        }
        case type_integer_literal: {
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(i32_type,
          true, false, true, false, 0);
          std::string int_type = dynamic_cast<LeafNode *>(expression_ptr->children_[0])->GetContent().GetIntType();
          if (int_type == "i32") {
            expression_ptr->integrated_type_->RemovePossibility(u32_type);
            expression_ptr->integrated_type_->RemovePossibility(isize_type);
            expression_ptr->integrated_type_->RemovePossibility(usize_type);
          } else if (int_type == "u32") {
            expression_ptr->integrated_type_->RemovePossibility(i32_type);
            expression_ptr->integrated_type_->RemovePossibility(isize_type);
            expression_ptr->integrated_type_->RemovePossibility(usize_type);
          } else if (int_type == "isize") {
            expression_ptr->integrated_type_->RemovePossibility(i32_type);
            expression_ptr->integrated_type_->RemovePossibility(u32_type);
            expression_ptr->integrated_type_->RemovePossibility(usize_type);
          } else if (int_type == "usize") {
            expression_ptr->integrated_type_->RemovePossibility(i32_type);
            expression_ptr->integrated_type_->RemovePossibility(u32_type);
            expression_ptr->integrated_type_->RemovePossibility(isize_type);
          }
          const long long literal_value = dynamic_cast<LeafNode *>(expression_ptr->children_[0])->GetContent().GetInt();
          CheckOverflow(literal_value, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = literal_value;
          expression_ptr->integrated_type_->type_completed = true;
          break;
        }
        default:;
      }
      break;
    }
    case path_in_expr: {
      auto path_in_expression_ptr = expression_ptr->children_[0];
      if (path_in_expression_ptr->children_.size() == 1) { // PathExprSegment
        const std::string path_name = dynamic_cast<LeafNode *>(path_in_expression_ptr->children_[0])
            ->GetContent().GetStr();
        const auto path_node_info = expression_ptr->scope_node_->FindInValue(path_name);
        expression_ptr->info_in_namespace_ = path_node_info;
        if (path_node_info.node == nullptr) {
          Throw("Cannot find the path name '" + path_name + "' in value namespace.");
        }
        if (path_node_info.node->integrated_type_->basic_type == unknown_type) {
          path_node_info.node->Accept(this);
        }
        expression_ptr->integrated_type_ = path_node_info.node->integrated_type_;
        if (expression_ptr->integrated_type_->is_const) {
          expression_ptr->value_ = path_node_info.node->value_;
        }
      } else { // PathExprSegment :: PathExprSegment
        const std::string first_path_name = dynamic_cast<LeafNode *>(path_in_expression_ptr->children_[0])
            ->GetContent().GetStr();
        auto first_path_node_info = expression_ptr->scope_node_->FindInType(first_path_name);
        if (first_path_name == "Self") {
          if (wrapping_structs_.empty()) {
            Throw("'Self' is undefined outside a struct.");
          }
          first_path_node_info = wrapping_structs_.back();
        } else if (first_path_node_info.node == nullptr) {
          Throw("Cannot find the path name in type namespace.");
        }
        // visit struct/enum node if hasn't
        switch (first_path_node_info.node_type) {
          case type_struct: {
            auto struct_ptr = dynamic_cast<Struct *>(first_path_node_info.node);
            for (const auto &item : struct_ptr->associated_items_) {
              if (item.second.node->integrated_type_ == nullptr ||
                  item.second.node->integrated_type_->basic_type == unknown_type) {
                // target struct has not been visited yet
                wrapping_structs_.push_back(first_path_node_info);
                for (const auto &it : struct_ptr->associated_items_) {
                  if (it.second.node_type == type_function) {
                    auto function_ptr = it.second.node;
                    for (int i = 0; i < function_ptr->children_.size(); ++i) {
                      if (function_ptr->type_[i] == type_function_return_type) {
                        is_reading_type_ = true;
                        type_owner_ = function_ptr;
                        function_ptr->children_[i]->Accept(this);
                        function_ptr->integrated_type_->type_completed = true;
                        is_reading_type_ = false;
                        type_owner_ = nullptr;
                      } else if (function_ptr->type_[i] == type_function_parameters) {
                        function_ptr->children_[i]->Accept(this);
                      }
                    }
                    if (function_ptr->integrated_type_ == nullptr) {
                      function_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
                          false, false, false, true, 0);
                    } else if (function_ptr->integrated_type_->basic_type == unknown_type) {
                      function_ptr->integrated_type_->basic_type = unit_type;
                      function_ptr->integrated_type_->type_completed = true;
                    }
                  } else { // it.second.node_type == type_constant_item
                    auto const_item_ptr = it.second.node;
                    is_reading_type_ = true;
                    type_owner_ = const_item_ptr;
                    const_item_ptr->children_[3]->Accept(this);
                    const_item_ptr->integrated_type_->type_completed = true;
                    is_reading_type_ = false;
                    type_owner_ = nullptr;
                  }
                }
                wrapping_structs_.pop_back();
                break;
              }
            }
            break;
          }
          case type_enumeration: {
            break;
          }
          default: {
            Throw("Unexpected path node type.");
          }
        }
        // now the first path node is visited
        const std::string second_path_name = dynamic_cast<LeafNode *>(path_in_expression_ptr->children_[2])
            ->GetContent().GetStr();
        if (first_path_node_info.node_type == type_struct) {
          auto struct_ptr = dynamic_cast<Struct *>(first_path_node_info.node);
          if (!struct_ptr->associated_items_.contains(second_path_name)) {
            Throw("There is no target item in the associated-item-field.");
          }
          const auto associated_item_node_info = struct_ptr->associated_items_[second_path_name];
          // What we get is a constant item.
          expression_ptr->integrated_type_ = associated_item_node_info.node->integrated_type_;
          expression_ptr->value_ = associated_item_node_info.node->value_;
        } else { // type_enumeration
          auto enum_ptr = dynamic_cast<Enumeration *>(first_path_node_info.node);
          if (!enum_ptr->enum_variants_.contains(second_path_name)) {
            Throw("There is no target enum variable in the enumeration field.");
          }
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(enumeration_type,
              true, false, false, true, 0);
          expression_ptr->integrated_type_->struct_node = enum_ptr;
          expression_ptr->value_.int_value = enum_ptr->enum_variants_[second_path_name];
        }
      }
      break;
    }
    case grouped_expr: {
      expression_ptr->children_[1]->Accept(this);
      expression_ptr->integrated_type_ = expression_ptr->children_[1]->integrated_type_;
      expression_ptr->value_ = expression_ptr->children_[1]->value_;
      break;
    }
    case array_expr: {
      if (expression_ptr->children_.size() == 2) {
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(array_type,
            true, false, false, true, 0);
        expression_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(never_type,
            true, false, false, true, 0);
        break;
      }
      if (expression_ptr->children_.size() == 5 &&
          dynamic_cast<LeafNode *>(expression_ptr->children_[2])->GetContent().GetStr() == ";") {
        // [Expression ; Expression]
        expression_ptr->children_[1]->Accept(this);
        expression_ptr->children_[3]->Accept(this);
        if (!expression_ptr->children_[3]->integrated_type_->is_const) {
          Throw("The length operand must be a constant expression.");
        }
        auto target_type = std::make_shared<IntegratedType>(usize_type, false,
            false, true, true, 0);
        target_type->RemovePossibility(i32_type);
        target_type->RemovePossibility(isize_type);
        target_type->RemovePossibility(u32_type);
        TryToMatch(target_type, expression_ptr->children_[3]->integrated_type_,
          true);
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(array_type,
            expression_ptr->children_[1]->integrated_type_->is_const, false,
            false, true, expression_ptr->children_[3]->value_.int_value);
        expression_ptr->integrated_type_->element_type = expression_ptr->children_[1]->integrated_type_;
        for (int i = 0; i < expression_ptr->children_[3]->value_.int_value; ++i) {
          expression_ptr->value_.array_values.push_back(expression_ptr->children_[1]->value_);
        }
      } else { // [Expression ( , Expression )* ,?]
        for (int i = 1; i < expression_ptr->children_.size() - 1; i += 2) {
          expression_ptr->children_[i]->Accept(this);
        }
        const int elem_num = static_cast<int>(expression_ptr->children_.size() - 1) / 2;
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(array_type,
            true, false, false, true, elem_num);
        expression_ptr->integrated_type_->element_type = expression_ptr->children_[1]->integrated_type_;
        for (int i = 1; i < expression_ptr->children_.size() - 1; i += 2) {
          TryToMatch(expression_ptr->children_[i]->integrated_type_,
              expression_ptr->integrated_type_->element_type, false);
          expression_ptr->value_.array_values.push_back(expression_ptr->children_[i]->value_);
          if (!expression_ptr->children_[i]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
        }
      }
      break;
    }
    case index_expr: {
      expression_ptr->children_[0]->Accept(this);
      expression_ptr->children_[1]->Accept(this);
      auto target_type = std::make_shared<IntegratedType>(usize_type, false,
          false, true, true, 0);
      target_type->RemovePossibility(i32_type);
      target_type->RemovePossibility(isize_type);
      target_type->RemovePossibility(u32_type);
      TryToMatch(target_type, expression_ptr->children_[1]->integrated_type_,
          false);
      if (expression_ptr->children_[0]->integrated_type_->basic_type == array_type) {
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
        *expression_ptr->integrated_type_ = *expression_ptr->children_[0]->integrated_type_->element_type;
        expression_ptr->integrated_type_->is_const = false;
        expression_ptr->integrated_type_->is_mutable = expression_ptr->children_[0]->integrated_type_->is_mutable;
        if (expression_ptr->children_[1]->integrated_type_->is_const) {
          if (expression_ptr->children_[1]->value_.int_value >=
              expression_ptr->children_[0]->integrated_type_->size) {
            Throw("Index out of bound.");
          }
          if (expression_ptr->children_[0]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
            expression_ptr->value_ = expression_ptr->children_[0]->value_.
                array_values[expression_ptr->children_[1]->value_.int_value];
          }
        }
      } else if (expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type
          && expression_ptr->children_[0]->integrated_type_->element_type->basic_type == array_type) {
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
        *expression_ptr->integrated_type_ = *expression_ptr->children_[0]->integrated_type_->element_type->element_type;
        expression_ptr->integrated_type_->is_const = false;
        expression_ptr->integrated_type_->is_mutable = expression_ptr->children_[0]->integrated_type_->element_type->is_mutable;
        if (expression_ptr->children_[1]->integrated_type_->is_const) {
          if (expression_ptr->children_[1]->value_.int_value >=
              expression_ptr->children_[0]->integrated_type_->element_type->size) {
            Throw("Index out of bound.");
          }
          if (expression_ptr->children_[0]->integrated_type_->element_type->is_const) {
            expression_ptr->integrated_type_->is_const = true;
            expression_ptr->value_ = expression_ptr->children_[0]->value_.pointer_value
                ->value_.array_values[expression_ptr->children_[1]->value_.int_value];
          }
        }
      } else {
        Throw("Cannot apply index operation to non-array expression.");
      }
      break;
    }
    case struct_expr: {
      const std::string struct_name = dynamic_cast<LeafNode *>(expression_ptr->children_[0]->children_[0])
          ->GetContent().GetStr();
      auto struct_info = expression_ptr->scope_node_->FindInType(struct_name);
      if (struct_name == "Self") {
        if (wrapping_structs_.empty()) {
          Throw("'Self' is undefined outside a struct.");
        }
        struct_info = wrapping_structs_.back();
      } else if (struct_info.node == nullptr) {
        Throw("Cannot find target struct.");
      }
      const auto struct_ptr = dynamic_cast<Struct *>(struct_info.node);
      // if target struct has not been visited, visit its field items
      wrapping_structs_.push_back(struct_info);
      for (const auto &item : struct_ptr->field_items_) {
        if (item.second.node->integrated_type_ == nullptr
            || item.second.node->integrated_type_->basic_type == unknown_type) {
          for (const auto &it : struct_ptr->field_items_) {
            it.second.node->Accept(this);
          }
          break;
        }
      }
      wrapping_structs_.pop_back();
      // now the field items in target struct are all visited
      expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(struct_type,
          true, false, false, true, 0);
      expression_ptr->integrated_type_->struct_node = struct_info.node;
      if (expression_ptr->children_.size() == 3) {
        break;
      }
      // expression_ptr->children_.size() == 4
      const auto struct_expr_fields_ptr = expression_ptr->children_[2];
      if ((struct_expr_fields_ptr->children_.size() + 1) / 2 != struct_ptr->field_items_.size()) {
        Throw("Type mismatch.");
      }
      std::unordered_set<std::string> initialized_field_items;
      for (int i = 0; i < struct_expr_fields_ptr->children_.size(); i += 2) {
        // struct_expr_fields_ptr->children[i] => StructExprField
        std::string field_item_name = dynamic_cast<LeafNode *>(struct_expr_fields_ptr->children_[i]->children_[0])
            ->GetContent().GetStr();
        if (initialized_field_items.contains(field_item_name)) {
          Throw("Duplicate field items.");
        }
        struct_expr_fields_ptr->children_[i]->children_[2]->Accept(this);
        if (!struct_ptr->field_items_.contains(field_item_name)) {
          Throw("Cannot find target item in struct field.");
        }
        auto standard_info = struct_ptr->field_items_[field_item_name];
        TryToMatch(standard_info.node->integrated_type_,
            struct_expr_fields_ptr->children_[i]->children_[2]->integrated_type_, false);
        if (!struct_expr_fields_ptr->children_[i]->children_[2]->integrated_type_->is_const) {
          expression_ptr->integrated_type_->is_const = false;
        } else {
          expression_ptr->value_.struct_values[field_item_name]
              = struct_expr_fields_ptr->children_[i]->children_[2]->value_;
        }
        initialized_field_items.insert(field_item_name);
      }
      break;
    }
    case call_expr: {
      if (expression_ptr->children_[0]->children_.size() == 1) {
        std::string function_name = dynamic_cast<LeafNode *>(expression_ptr->children_[0]->children_[0])
            ->GetContent().GetStr();
        if (function_name == "print") {
          // call parameter type: &str
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          if (expression_ptr->children_.size() == 1 || (expression_ptr->children_[1]->children_.size() + 1) / 2 != 1) {
            Throw("print(s : &str) should be called with one parameter.");
          }
          expression_ptr->children_[1]->children_[0]->Accept(this);
          if (expression_ptr->children_[1]->children_[0]->integrated_type_->basic_type != pointer_type
              || expression_ptr->children_[1]->children_[0]->integrated_type_->element_type->basic_type != str_type) {
            Throw("Type mismatch.");
          }
        } else if (function_name == "println") {
          // call parameter type: &str
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          if (expression_ptr->children_.size() == 1 || (expression_ptr->children_[1]->children_.size() + 1) / 2 != 1) {
            Throw("println(s : &str) should be called with one parameter.");
          }
          expression_ptr->children_[1]->children_[0]->Accept(this);
          if (expression_ptr->children_[1]->children_[0]->integrated_type_->basic_type != pointer_type
              || expression_ptr->children_[1]->children_[0]->integrated_type_->element_type->basic_type != str_type) {
            Throw("Type mismatch.");
          }
        } else if (function_name == "printInt") {
          // call parameter type: i32
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          if (expression_ptr->children_.size() == 1 || (expression_ptr->children_[1]->children_.size() + 1) / 2 != 1) {
            Throw("printInt(n : i32) should be called with one parameter.");
          }
          expression_ptr->children_[1]->children_[0]->Accept(this);
          auto target_type = std::make_shared<IntegratedType>(i32_type,
              false, false, true, true, 0);
          target_type->RemovePossibility(isize_type);
          target_type->RemovePossibility(u32_type);
          target_type->RemovePossibility(usize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->children_[0]->integrated_type_, false);
        } else if (function_name == "printlnInt") {
          // call parameter type: i32
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          if (expression_ptr->children_.size() == 1 || (expression_ptr->children_[1]->children_.size() + 1) / 2 != 1) {
            Throw("printlnInt(n : i32) should be called with one parameter.");
          }
          expression_ptr->children_[1]->children_[0]->Accept(this);
          auto target_type = std::make_shared<IntegratedType>(i32_type,
              false, false, true, true, 0);
          target_type->RemovePossibility(isize_type);
          target_type->RemovePossibility(u32_type);
          target_type->RemovePossibility(usize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->children_[0]->integrated_type_, false);
        } else if (function_name == "getString") {
          // no call parameter
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(string_type,
              false, false, false, true, 0);
          if (expression_ptr->children_.size() != 1) {
            Throw("getString() should be called with no parameter");
          }
        } else if (function_name == "getInt") {
          // no call parameter
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(i32_type,
              false, false, true, true, 0);
          expression_ptr->integrated_type_->RemovePossibility(isize_type);
          expression_ptr->integrated_type_->RemovePossibility(u32_type);
          expression_ptr->integrated_type_->RemovePossibility(usize_type);
          if (expression_ptr->children_.size() != 1) {
            Throw("getString() should be called with no parameter");
          }
        } else if (function_name == "exit") {
          // call parameter type: i32
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          if (expression_ptr->children_.size() == 1 || (expression_ptr->children_[1]->children_.size() + 1) / 2 != 1) {
            Throw("exit(code : i32) should be called with one parameter.");
          }
          expression_ptr->children_[1]->children_[0]->Accept(this);
          auto target_type = std::make_shared<IntegratedType>(i32_type,
              false, false, true, true, 0);
          target_type->RemovePossibility(isize_type);
          target_type->RemovePossibility(u32_type);
          target_type->RemovePossibility(usize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->children_[0]->integrated_type_, false);
          if (wrapping_function_.size() != 1 || !wrapping_structs_.empty()) { // not outermost
            Throw("exit() should be used in the end of the outermost main function");
          }
          auto exiting_function_info = wrapping_function_.back();
          if (dynamic_cast<LeafNode *>(exiting_function_info.node->children_[1])->GetContent().GetStr() != "main") {
            Throw("exit() should only be used in main function.");
          }
          Node *main_func_block_ptr = nullptr;
          for (int i = 0; i < exiting_function_info.node->children_.size(); ++i) {
            if (exiting_function_info.node->type_[i] != type_block_expression) {
              continue;
            }
            main_func_block_ptr = exiting_function_info.node->children_[i];
            break;
          }
          if (main_func_block_ptr == nullptr || main_func_block_ptr->children_.size() == 2) {
            Throw("Unexpected main function block ptr missing.");
          }
          Node *statements_ptr = main_func_block_ptr->children_[1];
          if (statements_ptr->type_[statements_ptr->children_.size() - 1] == type_statement) {
            Node *last_statement_ptr = statements_ptr->children_[statements_ptr->children_.size() - 1];
            if (last_statement_ptr->type_[0] != type_expression_statement) {
              Throw("exit() is not the last expression in Statements.");
            }
            if (last_statement_ptr->children_[0]->children_[0] != expression_ptr) {
              Throw("exit() is not the last expression in Statements.");
            }
          } else { // type_expression_without_block
            if (statements_ptr->children_[statements_ptr->children_.size() - 1] != expression_ptr) {
              Throw("exit() is not the last expression in Statements.");
            }
          }
        } else { // not builtin function
          const auto function_info = expression_ptr->scope_node_->FindInValue(function_name);
          if (function_info.node == nullptr) {
            Throw("Cannot find target function in the scope.");
          }
          expression_ptr->info_in_namespace_ = function_info;
          // if the target function has not been visited, visit its type and parameter type
          if (function_info.node->integrated_type_ == nullptr ||
              function_info.node->integrated_type_->basic_type == unknown_type) {
            for (int i = 0; i < function_info.node->children_.size(); ++i) {
              if (function_info.node->type_[i] == type_function_return_type) {
                is_reading_type_ = true;
                type_owner_ = function_info.node;
                function_info.node->children_[i]->Accept(this);
                function_info.node->integrated_type_->type_completed = true;
                is_reading_type_ = false;
                type_owner_ = nullptr;
              } else if (function_info.node->type_[i] == type_function_parameters) {
                function_info.node->children_[i]->Accept(this);
              }
            }
            if (function_info.node->integrated_type_ == nullptr) {
              function_info.node->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
                  false, false, false, true, 0);
            } else if (function_info.node->integrated_type_->basic_type == unknown_type) {
              function_info.node->integrated_type_->basic_type = unit_type;
              function_info.node->integrated_type_->type_completed = true;
            }
              }
          // now the function's type is ready
          int call_expr_param_num = 0;
          if (expression_ptr->children_.size() == 2) {
            call_expr_param_num = static_cast<int>(expression_ptr->children_[1]->children_.size() + 1) / 2;
          }
          int declared_param_num = 0;
          int function_parameters_node_ind = -1;
          for (int i = 0; i < function_info.node->children_.size(); ++i) {
            if (function_info.node->type_[i] != type_function_parameters) {
              continue;
            }
            function_parameters_node_ind = i;
            declared_param_num = static_cast<int>(function_info.node->children_[i]->children_.size() + 1) / 2;
            break;
          }
          if (call_expr_param_num != declared_param_num) {
            Throw("Parameter number doesn't match.");
          }
          expression_ptr->integrated_type_ = function_info.node->integrated_type_;
          if (call_expr_param_num == 0) {
            break;
          }
          for (int i = 0; i < expression_ptr->children_[1]->children_.size(); i += 2) {
            expression_ptr->children_[1]->children_[i]->Accept(this);
            TryToMatch(function_info.node->children_[function_parameters_node_ind]->children_[i]->integrated_type_,
                expression_ptr->children_[1]->children_[i]->integrated_type_, false);
          }
        }
      } else { // expression_ptr->children_[0]->children_.size == 3
        std::string struct_name = dynamic_cast<LeafNode *>(expression_ptr->children_[0]->children_[0])
            ->GetContent().GetStr();
        auto struct_info = expression_ptr->scope_node_->FindInType(struct_name);
        if (struct_name == "Self") {
          if (wrapping_structs_.empty()) {
            Throw("'Self' is undefined outside a struct.");
          }
          struct_info = wrapping_structs_.back();
        } else if (struct_info.node == nullptr) {
          Throw("Cannot find target struct in the scope.");
        }
        if (struct_info.node_type != type_struct) {
          Throw("Cannot find target struct.");
        }
        std::string function_name = dynamic_cast<LeafNode *>(expression_ptr->children_[0]->children_[2])
            ->GetContent().GetStr();
        auto struct_ptr = dynamic_cast<Struct *>(struct_info.node);
        if (!struct_ptr->associated_items_.contains(function_name)) {
          Throw("There is no target function in the associated-item field of the struct.");
        }
        const auto function_info = struct_ptr->associated_items_[function_name];
        expression_ptr->info_in_namespace_ = function_info;
        if (function_info.node_type != type_function) {
          Throw("Cannot call a non-function.");
        }
        // make sure the function type is ready
        if (function_info.node->integrated_type_ == nullptr ||
            function_info.node->integrated_type_->basic_type == unknown_type) {
          wrapping_structs_.push_back({struct_ptr, type_struct});
          for (const auto &it : struct_ptr->associated_items_) {
            if (it.second.node_type == type_function) {
              auto function_ptr = it.second.node;
              for (int i = 0; i < function_ptr->children_.size(); ++i) {
                if (function_ptr->type_[i] == type_function_return_type) {
                  is_reading_type_ = true;
                  type_owner_ = function_ptr;
                  function_ptr->children_[i]->Accept(this);
                  function_ptr->integrated_type_->type_completed = true;
                  is_reading_type_ = false;
                  type_owner_ = nullptr;
                } else if (function_ptr->type_[i] == type_function_parameters) {
                  function_ptr->children_[i]->Accept(this);
                }
              }
              if (function_ptr->integrated_type_ == nullptr) {
                function_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
                    false, false, false, true, 0);
              } else if (function_ptr->integrated_type_->basic_type == unknown_type) {
                function_ptr->integrated_type_->basic_type = unit_type;
                function_ptr->integrated_type_->type_completed = true;
              }
            } else { // it.second.node_type == type_constant_item
              auto const_item_ptr = it.second.node;
              is_reading_type_ = true;
              type_owner_ = const_item_ptr;
              const_item_ptr->children_[3]->Accept(this);
              const_item_ptr->integrated_type_->type_completed = true;
              is_reading_type_ = false;
              type_owner_ = nullptr;
            }
          }
          wrapping_structs_.pop_back();
        }
        // now the function type is ready
        int call_expr_param_num = 0;
        if (expression_ptr->children_.size() == 2) {
          call_expr_param_num = static_cast<int>(expression_ptr->children_[1]->children_.size() + 1) / 2;
        }
        int declared_param_num = 0;
        int function_parameters_node_ind = -1;
        for (int i = 0; i < function_info.node->children_.size(); ++i) {
          if (function_info.node->type_[i] != type_function_parameters) {
            continue;
          }
          function_parameters_node_ind = i;
          declared_param_num = static_cast<int>(function_info.node->children_[i]->children_.size() + 1) / 2;
          break;
        }
        if (call_expr_param_num != declared_param_num) {
          Throw("Parameter number doesn't match.");
        }
        expression_ptr->integrated_type_ = function_info.node->integrated_type_;
        if (call_expr_param_num == 0) {
          break;
        }
        if (function_info.node->children_[function_parameters_node_ind]->type_[0] == type_self_param) {
          Throw("function with self parameter should be called with method call expression.");
        }
        for (int i = 0; i < expression_ptr->children_[1]->children_.size(); i += 2) {
          expression_ptr->children_[1]->children_[i]->Accept(this);
          TryToMatch(function_info.node->children_[function_parameters_node_ind]->children_[i]->integrated_type_,
              expression_ptr->children_[1]->children_[i]->integrated_type_, false);
        }
      }
      break;
    }
    case method_call_expr: {
      if (expression_ptr->children_[1]->children_[0]->children_.size() != 1) {
        Throw("Expect an identifier following '.'.");
      }
      std::string function_name = dynamic_cast<LeafNode *>(expression_ptr->children_[1]->children_[0]->children_[0])
          ->GetContent().GetStr();
      expression_ptr->children_[0]->Accept(this);
      if (function_name == "to_string" && expression_ptr->children_[0]->integrated_type_->is_int) {
        // to_string() of type String : u32, usize
        expression_ptr->children_[0]->integrated_type_->RemovePossibility(i32_type);
        expression_ptr->children_[0]->integrated_type_->RemovePossibility(isize_type);
        if (expression_ptr->children_[0]->integrated_type_->is_const) {
          CheckOverflow(expression_ptr->children_[0]->value_.int_value, expression_ptr->children_[0]->integrated_type_);
        }
        if (expression_ptr->children_.size() != 2) {
          Throw("to_string method should be called with no parameter.");
        }
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(string_type,
            false, false, false, true, 0);
      } else if (function_name == "as_str" &&
          expression_ptr->children_[0]->integrated_type_->basic_type == string_type) {
        // as_str() of type &str : String
        if (expression_ptr->children_.size() != 2) {
          Throw("as_str method should be called with no parameter.");
        }
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(pointer_type,
            false, false, false, true, 0);
        expression_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(str_type,
            false, false, false, true, 0);
      } else if (function_name == "len" &&
          (expression_ptr->children_[0]->integrated_type_->basic_type == string_type
          || expression_ptr->children_[0]->integrated_type_->basic_type == array_type
          || (expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type
          && expression_ptr->children_[0]->integrated_type_->element_type->basic_type == str_type))) {
        // len() of usize : [T; N], String, &str
        if (expression_ptr->children_.size() != 2) {
          Throw("len method should be called with no parameter.");
        }
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(usize_type,
            false, false, true, true, 0);
        expression_ptr->integrated_type_->RemovePossibility(i32_type);
        expression_ptr->integrated_type_->RemovePossibility(isize_type);
        expression_ptr->integrated_type_->RemovePossibility(u32_type);
      } else {
        Struct *struct_ptr = nullptr;
        if (expression_ptr->children_[0]->integrated_type_->basic_type == struct_type) {
          struct_ptr = dynamic_cast<Struct *>(expression_ptr->children_[0]->integrated_type_->struct_node);
        } else if (expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type) {
          auto pointer = expression_ptr->children_[0]->integrated_type_->element_type;
          if (pointer->basic_type != struct_type) {
            Throw("Cannot apply method call operation to non-struct type.");
          }
          struct_ptr = dynamic_cast<Struct *>(pointer->struct_node);
        } else {
          Throw("Invalid type.");
        }
        if (!struct_ptr->associated_items_.contains(function_name)) {
          Throw("Cannot find target function in the associated-item-field of the struct.");
        }
        // make sure the struct type is ready
        for (const auto &item : struct_ptr->associated_items_) {
          if (item.second.node->integrated_type_ == nullptr ||
             item.second.node->integrated_type_->basic_type == unknown_type) {
            // target struct has not been visited yet
            wrapping_structs_.push_back({struct_ptr, type_struct});
            for (const auto &it : struct_ptr->associated_items_) {
              if (it.second.node_type == type_function) {
                auto function_ptr = it.second.node;
                for (int i = 0; i < function_ptr->children_.size(); ++i) {
                  if (function_ptr->type_[i] == type_function_return_type) {
                    is_reading_type_ = true;
                    type_owner_ = function_ptr;
                    function_ptr->children_[i]->Accept(this);
                    function_ptr->integrated_type_->type_completed = true;
                    is_reading_type_ = false;
                    type_owner_ = nullptr;
                  } else if (function_ptr->type_[i] == type_function_parameters) {
                    function_ptr->children_[i]->Accept(this);
                  }
                }
                if (function_ptr->integrated_type_ == nullptr) {
                  function_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
                      false, false, false, true, 0);
                } else if (function_ptr->integrated_type_->basic_type == unknown_type) {
                  function_ptr->integrated_type_->basic_type = unit_type;
                  function_ptr->integrated_type_->type_completed = true;
                }
              } else { // it.second.node_type == type_constant_item
                auto const_item_ptr = it.second.node;
                is_reading_type_ = true;
                type_owner_ = const_item_ptr;
                const_item_ptr->children_[3]->Accept(this);
                const_item_ptr->integrated_type_->type_completed = true;
                is_reading_type_ = false;
                type_owner_ = nullptr;
              }
            }
            wrapping_structs_.pop_back();
            break;
          }
        }
        // now the struct type is ready
        if (struct_ptr->associated_items_[function_name].node_type != type_function) {
          Throw("Cannot apply method call operation to non-function.");
        }
        expression_ptr->info_in_namespace_ = struct_ptr->associated_items_[function_name];
        auto function_ptr = expression_ptr->info_in_namespace_.node;
        // has got function_ptr
        int call_expr_param_num = 0;
        if (expression_ptr->children_.size() == 3) {
          call_expr_param_num = static_cast<int>(expression_ptr->children_[2]->children_.size() + 1) / 2;
        }
        int declared_param_num = 0;
        int function_parameters_node_ind = -1;
        for (int i = 0; i < function_ptr->children_.size(); ++i) {
          if (function_ptr->type_[i] != type_function_parameters) {
            continue;
          }
          function_parameters_node_ind = i;
          declared_param_num = static_cast<int>(function_ptr->children_[i]->children_.size() + 1) / 2;
          break;
        }
        if (call_expr_param_num + 1 != declared_param_num) {
          Throw("Parameter number doesn't match.");
        }
        if (function_ptr->children_[function_parameters_node_ind]->type_[0] != type_self_param) {
          Throw("function without self parameter should be called with call expression.");
        }
        expression_ptr->integrated_type_ = function_ptr->integrated_type_;
        if (function_ptr->children_[function_parameters_node_ind]->children_[0]
            ->children_[0]->children_.size() == 3) { // the one who call the method should be mutable
          if (expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type) {
            if (!expression_ptr->children_[0]->integrated_type_->element_type->is_mutable) {
              Throw("Cannot call a method function with self parameter '&mut self' "
                  "with the reference of immutable variable.");
            }
          } else if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot call a method function with self parameter '&mut self' "
                "with the reference of immutable variable.");
          }
        }
        if (call_expr_param_num == 0) {
          break;
        }
        for (int i = 0; i < expression_ptr->children_[2]->children_.size(); i += 2) {
          expression_ptr->children_[2]->children_[i]->Accept(this);
          TryToMatch(function_ptr->children_[function_parameters_node_ind]->children_[i + 2]->integrated_type_,
              expression_ptr->children_[2]->children_[i]->integrated_type_, false);
        }
      }
      break;
    }
    case field_expr: {
      if (expression_ptr->children_[1]->children_[0]->children_.size() != 1) {
        Throw("Expect an identifier following '.'.");
      }
      std::string identifier_name = dynamic_cast<LeafNode *>(expression_ptr->children_[1]->children_[0]->children_[0])
          ->GetContent().GetStr();
      expression_ptr->children_[0]->Accept(this);
      Struct *struct_ptr = nullptr;
      if (expression_ptr->children_[0]->integrated_type_->basic_type == struct_type) {
        struct_ptr = dynamic_cast<Struct *>(expression_ptr->children_[0]->integrated_type_->struct_node);
      } else if (expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type) {
        auto pointer = expression_ptr->children_[0]->integrated_type_->element_type;
        if (pointer->basic_type != struct_type) {
          Throw("Cannot apply field operation to non-struct type.");
        }
        struct_ptr = dynamic_cast<Struct *>(pointer->struct_node);
      } else {
        Throw("Invalid type.");
      }
      if (!struct_ptr->field_items_.contains(identifier_name)) {
        Throw("Cannot find target element in the field of the struct.");
      }
      // make sure struct type is ready
      for (const auto &item : struct_ptr->field_items_) {
        if (item.second.node->integrated_type_ == nullptr ||
           item.second.node->integrated_type_->basic_type == unknown_type) {
          wrapping_structs_.push_back({struct_ptr, type_struct});
          for (const auto &it : struct_ptr->field_items_) {
            it.second.node->Accept(this);
          }
          wrapping_structs_.pop_back();
          break;
        }
      }
      // now struct type is ready
      expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
      *expression_ptr->integrated_type_ = *struct_ptr->field_items_[identifier_name].node->integrated_type_;
      if (expression_ptr->children_[0]->integrated_type_->basic_type == struct_type) {
        expression_ptr->integrated_type_->is_const = expression_ptr->children_[0]->integrated_type_->is_const;
        expression_ptr->integrated_type_->is_mutable = expression_ptr->children_[0]->integrated_type_->is_mutable;
      } else { // expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type
        expression_ptr->integrated_type_->is_const = expression_ptr->children_[0]->integrated_type_->element_type->is_const;
        expression_ptr->integrated_type_->is_mutable = expression_ptr->children_[0]->integrated_type_->element_type->is_mutable;
      }
      break;
    }
    case continue_expr: {
      if (wrapping_loop_.empty()) {
        Throw("Invalid to call continue outside a loop.");
      }
      expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(never_type,
          false, false, false, true, 0);
      break;
    }
    case break_expr: {
      if (wrapping_loop_.empty()) {
        Throw("Invalid to call break outside a loop.");
      }
      auto loop_info = wrapping_loop_.back();
      if (expression_ptr->children_.size() == 2) {
        expression_ptr->children_[1]->Accept(this);
        if (loop_info.node->integrated_type_ == nullptr) {
          loop_info.node->integrated_type_ = std::make_shared<IntegratedType>();
          *loop_info.node->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
          loop_info.node->integrated_type_->is_const = false;
        } else if (loop_info.node->integrated_type_->basic_type == unknown_type) {
          *loop_info.node->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
          loop_info.node->integrated_type_->is_const = false;
        } else {
          TryToMatch(expression_ptr->children_[1]->integrated_type_,
              loop_info.node->integrated_type_, false);
        }
      } else {
        if (loop_info.node->integrated_type_ == nullptr) {
          loop_info.node->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
        } else if (loop_info.node->integrated_type_->basic_type == unknown_type) {
          loop_info.node->integrated_type_->basic_type = unit_type;
          loop_info.node->integrated_type_->type_completed = true;
        } else {
          TryToMatch(std::make_shared<IntegratedType>(unit_type, false, false,
              false, true, 0), loop_info.node->integrated_type_, false);
        }
      }
      expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(never_type,
          false, false, false, true, 0);
      break;
    }
    case return_expr: {
      if (wrapping_function_.empty()) {
        Throw("Invalid to call return outside a function.");
      }
      auto function_info = wrapping_function_.back();
      if (expression_ptr->children_.size() == 2) {
        expression_ptr->children_[1]->Accept(this);
        TryToMatch(function_info.node->integrated_type_, expression_ptr->children_[1]->integrated_type_,
            false);
      } else if (function_info.node->integrated_type_->basic_type != unit_type) {
        Throw("Type mismatch.");
      }
      expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(never_type,
          false, false, false, true, 0);
      break;
    }
    case prefix_expr: {
      std::string prefix = dynamic_cast<LeafNode *>(expression_ptr->children_[0])
          ->GetContent().GetStr();
      if (prefix == "-") {
        expression_ptr->children_[1]->Accept(this);
        if (!expression_ptr->children_[1]->integrated_type_->is_int) {
          Throw("Cannot apply prefix '-' to non-integer expression.");
        }
        expression_ptr->integrated_type_ = expression_ptr->children_[1]->integrated_type_;
        if (expression_ptr->integrated_type_->is_const) {
          long long val = -expression_ptr->children_[1]->value_.int_value;
          if (val == -2147483648 &&
              dynamic_cast<Expression *>(expression_ptr->children_[1])->GetExprType() == literal_expr) {
            expression_ptr->integrated_type_->possible_types.insert(i32_type);
          }
          CheckOverflow(val, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = val;
        }
      } else if (prefix == "*") {
        expression_ptr->children_[1]->Accept(this);
        if(expression_ptr->children_[1]->integrated_type_->basic_type != pointer_type) {
          Throw("Cannot dereference a non-pointer type.");
        }
        expression_ptr->integrated_type_ = expression_ptr->children_[1]->integrated_type_->element_type;
        if (expression_ptr->integrated_type_->is_const) {
          expression_ptr->value_ = expression_ptr->children_[1]->value_.pointer_value->value_;
        }
      } else if (prefix == "!") {
        expression_ptr->children_[1]->Accept(this);
        expression_ptr->integrated_type_ = expression_ptr->children_[1]->integrated_type_;
        if (expression_ptr->integrated_type_->basic_type == bool_type) {
          if (expression_ptr->integrated_type_->is_const) {
            expression_ptr->value_.int_value = 1 - expression_ptr->children_[1]->value_.int_value;
          }
        } else if (expression_ptr->integrated_type_->is_int) {
          if (expression_ptr->integrated_type_->is_const) {
            expression_ptr->value_.int_value = ~expression_ptr->children_[1]->value_.int_value;
          }
        } else {
          Throw("Cannot apply prefix '!' to neither-bool-nor-int type.");
        }
      } else if (prefix == "&") {
        if (expression_ptr->children_.size() == 2) {
          expression_ptr->children_[1]->Accept(this);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(pointer_type,
              expression_ptr->children_[1]->integrated_type_->is_const, false,
              false, true, 0);
          expression_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_->element_type = *expression_ptr->children_[1]->integrated_type_;
          expression_ptr->integrated_type_->element_type->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            expression_ptr->value_.pointer_value = expression_ptr->children_[1];
          }
        } else { // expression_ptr->children_.size() == 3
          expression_ptr->children_[2]->Accept(this);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(pointer_type,
              expression_ptr->children_[2]->integrated_type_->is_const, false,
              false, true, 0);
          expression_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_->element_type = *expression_ptr->children_[2]->integrated_type_;
          expression_ptr->integrated_type_->element_type->is_mutable = true;
          if (expression_ptr->integrated_type_->is_const) {
            switch (dynamic_cast<Expression *>(expression_ptr->children_[2])->GetExprType()) {
              case literal_expr:
              case array_expr:
              case struct_expr: {
                expression_ptr->integrated_type_->is_const = false;
                expression_ptr->children_[2]->integrated_type_->is_const = false;
                break;
              }
              default: {
                Throw("A const value cannot have a mutable reference.");
              }
            }
          }
        }
      } else if (prefix == "&&") {
        if (expression_ptr->children_.size() == 2) {
          expression_ptr->children_[1]->Accept(this);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(pointer_type,
            expression_ptr->children_[1]->integrated_type_->is_const, false,
            false, true, 0);
          expression_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(pointer_type,
              expression_ptr->children_[1]->integrated_type_->is_const, false,
              false, true, 0);
          expression_ptr->integrated_type_->element_type->element_type = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_->element_type->element_type = *expression_ptr->children_[1]->integrated_type_;
          expression_ptr->integrated_type_->element_type->element_type->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            Throw("The value of '&&' type has not been completed yet.");
          }
        } else { // expression_ptr->children_.size() == 3
          expression_ptr->children_[2]->Accept(this);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(pointer_type,
            expression_ptr->children_[2]->integrated_type_->is_const, false,
            false, true, 0);
          expression_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(pointer_type,
              expression_ptr->children_[2]->integrated_type_->is_const, false,
              false, true, 0);
          expression_ptr->integrated_type_->element_type->element_type = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_->element_type->element_type = *expression_ptr->children_[2]->integrated_type_;
          expression_ptr->integrated_type_->element_type->element_type->is_mutable = true;
          if (expression_ptr->integrated_type_->is_const) {
            Throw("A const value cannot have a mutable double reference.");
          }
        }
      }
      break;
    }
    case call_params: {
      break;
    }
    default: { // operator_expr
      switch (expression_ptr->GetExprInfix()) {
        case add: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Add operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
          if (!expression_ptr->children_[0]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            const long long ans = expression_ptr->children_[0]->value_.int_value
                + expression_ptr->children_[1]->value_.int_value;
            CheckOverflow(ans, expression_ptr->integrated_type_);
            expression_ptr->value_.int_value = ans;
          }
          break;
        }
        case minus: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Minus operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
          if (!expression_ptr->children_[0]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            const long long ans = expression_ptr->children_[0]->value_.int_value
                - expression_ptr->children_[1]->value_.int_value;
            CheckOverflow(ans, expression_ptr->integrated_type_);
            expression_ptr->value_.int_value = ans;
          }
          break;
        }
        case multiply: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Multiply operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
          if (!expression_ptr->children_[0]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            const long long ans = expression_ptr->children_[0]->value_.int_value
                * expression_ptr->children_[1]->value_.int_value;
            CheckOverflow(ans, expression_ptr->integrated_type_);
            expression_ptr->value_.int_value = ans;
          }
          break;
        }
        case divide: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Divide operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
          if (!expression_ptr->children_[0]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            const long long ans = expression_ptr->children_[0]->value_.int_value
                / expression_ptr->children_[1]->value_.int_value;
            CheckOverflow(ans, expression_ptr->integrated_type_);
            expression_ptr->value_.int_value = ans;
          }
          break;
        }
        case mod: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Mod operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
          if (!expression_ptr->children_[0]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            const long long ans = expression_ptr->children_[0]->value_.int_value
                % expression_ptr->children_[1]->value_.int_value;
            CheckOverflow(ans, expression_ptr->integrated_type_);
            expression_ptr->value_.int_value = ans;
          }
          break;
        }
        case bitwise_and: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Bitwise-and operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
          if (!expression_ptr->children_[0]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            const long long ans = expression_ptr->children_[0]->value_.int_value
                & expression_ptr->children_[1]->value_.int_value;
            CheckOverflow(ans, expression_ptr->integrated_type_);
            expression_ptr->value_.int_value = ans;
          }
          break;
        }
        case bitwise_or: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Bitwise-or operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
          if (!expression_ptr->children_[0]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            const long long ans = expression_ptr->children_[0]->value_.int_value
                | expression_ptr->children_[1]->value_.int_value;
            CheckOverflow(ans, expression_ptr->integrated_type_);
            expression_ptr->value_.int_value = ans;
          }
          break;
        }
        case bitwise_xor: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Bitwise-xor operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[1]->integrated_type_;
          if (!expression_ptr->children_[0]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            const long long ans = expression_ptr->children_[0]->value_.int_value
                ^ expression_ptr->children_[1]->value_.int_value;
            CheckOverflow(ans, expression_ptr->integrated_type_);
            expression_ptr->value_.int_value = ans;
          }
          break;
        }
        case left_shift: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Left-shift operation is only available between integer values.");
          }
          if (expression_ptr->children_[1]->integrated_type_->is_const &&
              expression_ptr->children_[1]->value_.int_value < 0) {
            Throw("Left shift overflow.");
          }
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[0]->integrated_type_;
          if (!expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            const long long ans = expression_ptr->children_[0]->value_.int_value
                << expression_ptr->children_[1]->value_.int_value;
            CheckOverflow(ans, expression_ptr->integrated_type_);
            expression_ptr->value_.int_value = ans;
          }
          break;
        }
        case right_shift: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Right-shift operation is only available between integer values.");
          }
          if (expression_ptr->children_[1]->integrated_type_->is_const &&
              expression_ptr->children_[1]->value_.int_value < 0) {
            Throw("Right shift overflow.");
          }
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[0]->integrated_type_;
          if (!expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          if (expression_ptr->integrated_type_->is_const) {
            const long long ans = expression_ptr->children_[0]->value_.int_value
                >> expression_ptr->children_[1]->value_.int_value;
            CheckOverflow(ans, expression_ptr->integrated_type_);
            expression_ptr->value_.int_value = ans;
          }
          break;
        }
        case is_equal: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if ((expression_ptr->children_[0]->integrated_type_->basic_type == never_type &&
              expression_ptr->children_[1]->integrated_type_->basic_type != never_type) ||
              (expression_ptr->children_[0]->integrated_type_->basic_type != never_type &&
              expression_ptr->children_[1]->integrated_type_->basic_type == never_type)) {
            Throw("Cannot compare a never-type-value with non-never-type one.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(bool_type,
              false, false, false, true, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
            switch (expression_ptr->children_[1]->integrated_type_->basic_type) {
              case bool_type:
              case i32_type:
              case isize_type:
              case u32_type:
              case usize_type:
              case enumeration_type: {
                if (expression_ptr->children_[0]->value_.int_value ==
                    expression_ptr->children_[1]->value_.int_value) {
                  expression_ptr->value_.int_value = 1;
                } else {
                  expression_ptr->value_.int_value = 0;
                }
                break;
              }
              case char_type:
              case str_type:
              case string_type: {
                if (expression_ptr->children_[0]->value_.str_value ==
                    expression_ptr->children_[1]->value_.str_value) {
                  expression_ptr->value_.int_value = 1;
                } else {
                  expression_ptr->value_.int_value = 0;
                }
                break;
              }
              case array_type: {
                if (expression_ptr->children_[0]->value_.array_values ==
                    expression_ptr->children_[1]->value_.array_values) {
                  expression_ptr->value_.int_value = 1;
                } else {
                  expression_ptr->value_.int_value = 0;
                }
                break;
              }
              case struct_type: {
                if (expression_ptr->children_[0]->value_.struct_values ==
                    expression_ptr->children_[1]->value_.struct_values) {
                  expression_ptr->value_.int_value = 1;
                } else {
                  expression_ptr->value_.int_value = 0;
                }
                break;
              }
              case pointer_type: {
                if (expression_ptr->children_[0]->value_.pointer_value->value_ ==
                    expression_ptr->children_[1]->value_.pointer_value->value_) {
                  expression_ptr->value_.int_value = 1;
                } else {
                  expression_ptr->value_.int_value = 0;
                }
                break;
              }
              case never_type: {
                expression_ptr->value_.int_value = 1;
                break;
              }
              default:;
            }
          }
          break;
        }
        case is_not_equal: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if ((expression_ptr->children_[0]->integrated_type_->basic_type == never_type &&
              expression_ptr->children_[1]->integrated_type_->basic_type != never_type) ||
              (expression_ptr->children_[0]->integrated_type_->basic_type != never_type &&
              expression_ptr->children_[1]->integrated_type_->basic_type == never_type)) {
            Throw("Cannot compare a never-type-value with non-never-type one.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(bool_type,
              false, false, false, true, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
            switch (expression_ptr->children_[1]->integrated_type_->basic_type) {
              case bool_type:
              case i32_type:
              case isize_type:
              case u32_type:
              case usize_type:
              case enumeration_type: {
                if (expression_ptr->children_[0]->value_.int_value ==
                    expression_ptr->children_[1]->value_.int_value) {
                  expression_ptr->value_.int_value = 0;
                } else {
                  expression_ptr->value_.int_value = 1;
                }
                break;
              }
              case char_type:
              case str_type:
              case string_type: {
                if (expression_ptr->children_[0]->value_.str_value ==
                    expression_ptr->children_[1]->value_.str_value) {
                  expression_ptr->value_.int_value = 0;
                } else {
                  expression_ptr->value_.int_value = 1;
                }
                break;
              }
              case array_type: {
                if (expression_ptr->children_[0]->value_.array_values ==
                    expression_ptr->children_[1]->value_.array_values) {
                  expression_ptr->value_.int_value = 0;
                } else {
                  expression_ptr->value_.int_value = 1;
                }
                break;
              }
              case struct_type: {
                if (expression_ptr->children_[0]->value_.struct_values ==
                    expression_ptr->children_[1]->value_.struct_values) {
                  expression_ptr->value_.int_value = 0;
                } else {
                  expression_ptr->value_.int_value = 1;
                }
                break;
              }
              case pointer_type: {
                if (expression_ptr->children_[0]->value_.pointer_value->value_ ==
                    expression_ptr->children_[1]->value_.pointer_value->value_) {
                  expression_ptr->value_.int_value = 0;
                } else {
                  expression_ptr->value_.int_value = 1;
                }
                break;
              }
              case never_type: {
                expression_ptr->value_.int_value = 0;
                break;
              }
              default:;
            }
          }
          break;
        }
        case is_bigger: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Whether-bigger-compare is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(bool_type,
              false, false, false, true, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
            if (expression_ptr->children_[0]->value_.int_value > expression_ptr->children_[1]->value_.int_value) {
              expression_ptr->value_.int_value = 1;
            } else {
              expression_ptr->value_.int_value = 0;
            }
          }
          break;
        }
        case is_smaller: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Whether-smaller-compare is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(bool_type,
              false, false, false, true, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
            if (expression_ptr->children_[0]->value_.int_value < expression_ptr->children_[1]->value_.int_value) {
              expression_ptr->value_.int_value = 1;
            } else {
              expression_ptr->value_.int_value = 0;
            }
          }
          break;
        }
        case is_not_smaller: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Whether-not-smaller-compare is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(bool_type,
              false, false, false, true, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
            if (expression_ptr->children_[0]->value_.int_value >= expression_ptr->children_[1]->value_.int_value) {
              expression_ptr->value_.int_value = 1;
            } else {
              expression_ptr->value_.int_value = 0;
            }
          }
          break;
        }
        case is_not_bigger: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Whether-not-bigger-compare is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(bool_type,
              false, false, false, true, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
            if (expression_ptr->children_[0]->value_.int_value <= expression_ptr->children_[1]->value_.int_value) {
              expression_ptr->value_.int_value = 1;
            } else {
              expression_ptr->value_.int_value = 0;
            }
          }
          break;
        }
        case logic_or: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (expression_ptr->children_[0]->integrated_type_->basic_type != bool_type ||
              expression_ptr->children_[1]->integrated_type_->basic_type != bool_type) {
            Throw("Logic-or is only available between bool values.");
          }
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(bool_type,
              false, false, false, true, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
            if (expression_ptr->children_[0]->value_.int_value || expression_ptr->children_[1]->value_.int_value) {
              expression_ptr->value_.int_value = 1;
            } else {
              expression_ptr->value_.int_value = 0;
            }
          }
          break;
        }
        case logic_and: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (expression_ptr->children_[0]->integrated_type_->basic_type != bool_type ||
              expression_ptr->children_[1]->integrated_type_->basic_type != bool_type) {
            Throw("Logic-and is only available between bool values.");
          }
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(bool_type,
              false, false, false, true, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
            if (expression_ptr->children_[0]->value_.int_value && expression_ptr->children_[1]->value_.int_value) {
              expression_ptr->value_.int_value = 1;
            } else {
              expression_ptr->value_.int_value = 0;
            }
          }
          break;
        }
        case assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          switch (expression_ptr->children_[1]->integrated_type_->basic_type) {
            case bool_type:
            case i32_type:
            case isize_type:
            case u32_type:
            case usize_type:
            case enumeration_type: {
              expression_ptr->children_[0]->value_.int_value = expression_ptr->children_[1]->value_.int_value;
              break;
            }
            case char_type:
            case str_type:
            case string_type: {
              expression_ptr->children_[0]->value_.str_value = expression_ptr->children_[1]->value_.str_value;
              break;
            }
            case array_type: {
              expression_ptr->children_[0]->value_.array_values = expression_ptr->children_[1]->value_.array_values;
              break;
            }
            case struct_type: {
              expression_ptr->children_[0]->value_.struct_values = expression_ptr->children_[1]->value_.struct_values;
              break;
            }
            case pointer_type: {
              expression_ptr->children_[0]->value_.pointer_value = expression_ptr->children_[1]->value_.pointer_value;
              break;
            }
            default:;
          }
          break;
        }
        case add_assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Add-assign operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          break;
        }
        case minus_assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Minus-assign operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          break;
        }
        case multiply_assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Multiply-assign operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          break;
        }
        case divide_assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Divide-assign operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          break;
        }
        case mod_assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Mod-assign operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          break;
        }
        case bitwise_and_assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Bitwise-and-assign operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          break;
        }
        case bitwise_or_assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Bitwise-or-assign operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          break;
        }
        case bitwise_xor_assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Bitwise-xor-assign operation is only available between integer values.");
          }
          TryToMatch(expression_ptr->children_[0]->integrated_type_,
              expression_ptr->children_[1]->integrated_type_, false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          break;
        }
        case left_shift_assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Left-shift operation is only available between integer values.");
          }
          const auto target_type = std::make_shared<IntegratedType>(u32_type,
              false, false, true, true, 0);
          target_type->RemovePossibility(i32_type);
          target_type->RemovePossibility(isize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->integrated_type_,
              false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          break;
        }
        case right_shift_assign: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_mutable) {
            Throw("Cannot apply assignment operation to immutable expression.");
          }
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Right-shift operation is only available between integer values.");
              }
          const auto target_type = std::make_shared<IntegratedType>(u32_type,
              false, false, true, true, 0);
          target_type->RemovePossibility(i32_type);
          target_type->RemovePossibility(isize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->integrated_type_,
              false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, true, 0);
          break;
        }
        case type_cast: {
          expression_ptr->children_[0]->Accept(this);
          is_reading_type_ = true;
          type_owner_ = expression_ptr;
          expression_ptr->children_[1]->Accept(this);
          expression_ptr->integrated_type_->type_completed = true;
          is_reading_type_ = false;
          type_owner_ = nullptr;
          long long int_value = 0;
          switch (expression_ptr->children_[0]->integrated_type_->basic_type) {
            case bool_type: {
              int_value = expression_ptr->children_[0]->value_.int_value;
              break;
            }
            case char_type: {
              const std::string char_as_str = expression_ptr->children_[0]->value_.str_value;
              if (char_as_str.length() == 1) {
                int_value = static_cast<unsigned char>(char_as_str[0]);
              } else if (char_as_str.length() > 1) {
                switch (char_as_str[1]) {
                  case '\'': {
                    int_value = 39;
                    break;
                  }
                  case '"': {
                    int_value = 34;
                    break;
                  }
                  case 'n': {
                    int_value = 10;
                    break;
                  }
                  case 'r': {
                    int_value = 13;
                    break;
                  }
                  case 't': {
                    int_value = 9;
                    break;
                  }
                  case '\\': {
                    int_value = 92;
                    break;
                  }
                  case '0': {
                    int_value = 0;
                    break;
                  }
                  case 'x': {
                    if (char_as_str.length() != 4) {
                      Throw("Invalid escape character.");
                    }
                    if (char_as_str[2] < '0' || char_as_str[2] > '7') {
                      Throw("Invalid oct digit.");
                    }
                    int_value = (char_as_str[2] - '0') << 4;
                    if (char_as_str[3] >= '0' && char_as_str[3] <= '9') {
                      int_value += char_as_str[3] - '0';
                    } else if (char_as_str[3] >= 'a' && char_as_str[3] <= 'f') {
                      int_value += char_as_str[3] - 'a';
                    } else if (char_as_str[3] >= 'A' && char_as_str[3] <= 'F') {
                      int_value += char_as_str[3] - 'A';
                    } else {
                      Throw("Invalid hex digit.");
                    }
                  }
                  default: {
                    Throw("Invalid escape character.");
                  }
                }
              } else {
                Throw("Invalid char.");
              }
              break;
            }
            case i32_type:
            case isize_type:
            case u32_type:
            case usize_type: {
              int_value = expression_ptr->children_[0]->value_.int_value;
              break;
            }
            default: {
              Throw("Only bool/char/int type can be cast.");
            }
          }
          if (!expression_ptr->integrated_type_->is_int) {
            Throw("Invalid type cast.");
          }
          CheckOverflow(int_value, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = int_value;
          expression_ptr->integrated_type_->is_const =
              expression_ptr->children_[0]->integrated_type_->is_const;
          break;
        }
        default:;
      }
    }
  }
}