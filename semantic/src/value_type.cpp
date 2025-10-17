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
    for (const auto it : expr_type->possible_types) {
      if (!target_type->possible_types.contains(it)) {
        expr_type->RemovePossibility(it);
      }
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
    is_reading_type_ = false;
    type_owner_ = nullptr;
  }
  if (function_ptr->integrated_type_->basic_type == unknown_type) {
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
}
void ValueTypeVisitor::Visit(Struct *struct_ptr) {
  // check whether the struct has been analyzed
  if (!struct_ptr->field_items_.empty()) {
    if (struct_ptr->field_items_.begin()->second.node->integrated_type_ != nullptr) {
      return;
    }
  }
  if (!struct_ptr->associated_items_.empty()) {
    if (struct_ptr->associated_items_.begin()->second.node->integrated_type_ != nullptr) {
      return;
    }
  }
  // has not been analyzed
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
  const std::string constant_item_name = dynamic_cast<LeafNode *>(constant_item_ptr->children_[1])->GetContent().GetStr();
  constant_item_ptr->scope_node_->ValueAdd(constant_item_name,
      {constant_item_ptr->children_[1], type_identifier});
}
void ValueTypeVisitor::Visit(BlockExpression *block_expression_ptr) {
  if (block_expression_ptr->children_.size() == 2) {
    block_expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type, true,
        false, false, false, 0);
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
              false, false, false, false, 0);
          self_param_ptr->integrated_type_->element_type->struct_node = target_struct.node;
          break;
        }
        case type_enumeration: {
          self_param_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(enumeration_type,
              false, false, false, false, 0);
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
              false, true, false, false, 0);
          self_param_ptr->integrated_type_->element_type->struct_node = target_struct.node;
          break;
        }
        case type_enumeration: {
          self_param_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(enumeration_type,
              false, true, false, false, 0);
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
      false, true, false, 0);
  target_size_ptr->RemovePossibility(u32_type);
  target_size_ptr->RemovePossibility(i32_type);
  target_size_ptr->RemovePossibility(isize_type);
  TryToMatch(target_size_ptr, array_type_ptr->children_[3]->integrated_type_, true);
  (*current_type_ptr)->size = array_type_ptr->children_[3]->value_.int_value;
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
  } else if (type_name == "u32") {
    (*current_type_ptr)->basic_type = u32_type;
  } else if (type_name == "isize") {
    (*current_type_ptr)->basic_type = isize_type;
  } else if (type_name == "usize") {
    (*current_type_ptr)->basic_type = usize_type;
  } else if (type_name == "bool") {
    (*current_type_ptr)->basic_type = bool_type;
  } else if (type_name == "char") {
    (*current_type_ptr)->basic_type = char_type;
  } else if (type_name == "str") {
    (*current_type_ptr)->basic_type = str_type;
  } else if (type_name == "String") {
    (*current_type_ptr)->basic_type = string_type;
  } else {
    auto type_node = type_path_ptr->scope_node_->FindInType(type_name);
    if (type_node.node == nullptr) {
      Throw("Undefined type name.");
    }
    (*current_type_ptr)->basic_type = struct_type;
    (*current_type_ptr)->struct_node = type_node.node;
  }
}
void ValueTypeVisitor::Visit(Statements *statements_ptr) {
  for (const auto it : statements_ptr->children_) {
    it->Accept(this);
  }
  if (statements_ptr->type_.back() == type_expression) {
    statements_ptr->integrated_type_ = statements_ptr->children_.back()->integrated_type_;
    statements_ptr->value_ = statements_ptr->children_.back()->value_;
  } else {
    auto statement_ptr = statements_ptr->children_.back();
    if (statement_ptr->type_[0] == type_expression_statement) {
      if (statement_ptr->children_[0]->children_.size() == 1) {
        statements_ptr->integrated_type_ = statement_ptr->children_[0]->children_[0]->integrated_type_;
      } else {
        statements_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
            false, false, false, false, 0);
      }
    } else {
      statements_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
          false, false, false, false, 0);
    }
  }
}
void ValueTypeVisitor::Visit(Statement *statement_ptr) {
  statement_ptr->children_[0]->Accept(this);
  if (statement_ptr->type_[0] != type_expression_statement) {
    statement_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
        false, false, false, false, 0);
  } else {
    statement_ptr->integrated_type_ = statement_ptr->children_[0]->integrated_type_;
  }
}
void ValueTypeVisitor::Visit(LetStatement *let_statement_ptr) {
  let_statement_ptr->integrated_type_ = std::make_shared<IntegratedType>();
  is_reading_type_ = true;
  type_owner_ = let_statement_ptr->children_[1];
  let_statement_ptr->children_[3]->Accept(this); // visit type
  is_reading_type_ = false;
  type_owner_ = nullptr;
  let_statement_ptr->children_[5]->Accept(this); // visit expression
  TryToMatch(let_statement_ptr->children_[1]->integrated_type_,
      let_statement_ptr->children_[5]->integrated_type_, false);
  auto identifier_pattern_ptr = let_statement_ptr->children_[1]->children_[0];
  std::string identifier_pattern_name;
  if (identifier_pattern_ptr->children_.size() > 1) {
    let_statement_ptr->integrated_type_->is_mutable = true;
    identifier_pattern_name = dynamic_cast<LeafNode *>(identifier_pattern_ptr->children_[1])->GetContent().GetStr();
  } else {
    identifier_pattern_name = dynamic_cast<LeafNode *>(identifier_pattern_ptr->children_[0])->GetContent().GetStr();
  }
  let_statement_ptr->scope_node_->ValueAdd(identifier_pattern_name,
      {let_statement_ptr->children_[1], type_pattern});
}
void ValueTypeVisitor::Visit(ExpressionStatement *expression_statement_ptr) {
  expression_statement_ptr->children_[0]->Accept(this);
  if (expression_statement_ptr->children_.size() > 1) {
    expression_statement_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
        false, false, false, false, 0);
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
  is_reading_type_ = false;
  type_owner_ = nullptr;
}
void ValueTypeVisitor::Visit(Expression *expression_ptr) {
  switch (expression_ptr->GetExprType()) {
    case block_expr: {
      if (expression_ptr->children_.size() == 2) {
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
            false, false, false, false, 0);
      } else {
        expression_ptr->children_[1]->Accept(this);
        expression_ptr->integrated_type_ = expression_ptr->children_[1]->integrated_type_;
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
        case type_keyword: {
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(bool_type,
              true, false, false, false, 0);
          if (dynamic_cast<LeafNode *>(expression_ptr->children_[0])->GetContent().GetStr() == "true") {
            expression_ptr->value_.int_value = 1;
          } else {
            expression_ptr->value_.int_value = 0;
          }
          break;
        }
        case type_char_literal: {
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(char_type,
              true, false, false, false, 0);
          expression_ptr->value_.str_value = dynamic_cast<LeafNode *>(expression_ptr->children_[0])
              ->GetContent().GetCharContent();
          break;
        }
        case type_string_literal:
        case type_raw_string_literal:
        case type_c_string_literal:
        case type_raw_c_string_literal: {
          expression_ptr->children_[0]->integrated_type_ = std::make_shared<IntegratedType>(str_type,
              true, false, false, false, 0);
          expression_ptr->children_[0]->value_.str_value = dynamic_cast<LeafNode *>(expression_ptr->children_[0])
              ->GetContent().GetStringContent();
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(pointer_type,
              true, false, false, false, 0);
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
        if (path_node_info.node == nullptr) {
          Throw("Cannot find the path name in value namespace.");
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
        const auto first_path_node_info = expression_ptr->scope_node_->FindInType(first_path_name);
        if (first_path_node_info.node == nullptr) {
          Throw("Cannot find the path name in type namespace.");
        }
        // visit struct/enum node if hasn't
        switch (first_path_node_info.node_type) {
          case type_struct: {
            auto struct_ptr = dynamic_cast<Struct *>(first_path_node_info.node);
            if (!struct_ptr->associated_items_.empty()) {
              if (struct_ptr->associated_items_.begin()->second.node->integrated_type_->basic_type == unknown_type) {
                first_path_node_info.node->Accept(this);
                break;
              }
            } else {
              Throw("Struct with no associated-items!");
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
              true, false, false, false, 0);
          expression_ptr->value_.pointer_value = enum_ptr;
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
            true, false, false, false, 0);
        break;
      }
      auto array_elements_expr = expression_ptr->children_[1];
      if (array_elements_expr->children_.size() == 3 &&
          dynamic_cast<LeafNode *>(array_elements_expr->children_[1])->GetContent().GetStr() == ";") {
        // [Expression ; Expression]
        array_elements_expr->children_[0]->Accept(this);
        array_elements_expr->children_[2]->Accept(this);
        if (!array_elements_expr->children_[2]->integrated_type_->is_const) {
          Throw("The length operand must be a constant expression.");
        }
        auto target_type = std::make_shared<IntegratedType>(usize_type, false,
            false, false, false, 0);
        target_type->RemovePossibility(i32_type);
        target_type->RemovePossibility(isize_type);
        target_type->RemovePossibility(u32_type);
        TryToMatch(target_type, array_elements_expr->children_[2]->integrated_type_,
          true);
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(array_type,
            array_elements_expr->children_[0]->integrated_type_->is_const, false,
            false, false, array_elements_expr->children_[2]->value_.int_value);
        expression_ptr->integrated_type_->element_type = expression_ptr->children_[0]->integrated_type_;
        for (int i = 0; i < expression_ptr->children_[2]->value_.int_value; ++i) {
          expression_ptr->value_.array_values.push_back(expression_ptr->children_[0]->value_);
        }
      } else { // [Expression ( , Expression )* ,?]
        for (int i = 0; i < array_elements_expr->children_.size(); i += 2) {
          array_elements_expr->children_[i]->Accept(this);
        }
        const int elem_num = static_cast<int>(array_elements_expr->children_.size() + 1) / 2;
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(array_type,
            array_elements_expr->children_[0]->integrated_type_->is_const, false,
            false, false, elem_num);
        expression_ptr->integrated_type_->element_type = expression_ptr->children_[0]->integrated_type_;
        for (int i = 0; i < array_elements_expr->children_.size(); i += 2) {
          TryToMatch(array_elements_expr->children_[i]->integrated_type_,
              expression_ptr->integrated_type_->element_type, false);
          expression_ptr->value_.array_values.push_back(array_elements_expr->children_[i]->value_);
          if (!array_elements_expr->children_[i]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
        }
      }
      break;
    }
    case index_expr: {
      expression_ptr->children_[0]->Accept(this);
      expression_ptr->children_[2]->Accept(this);
      auto target_type = std::make_shared<IntegratedType>(usize_type, false,
          false, false, false, 0);
      target_type->RemovePossibility(i32_type);
      target_type->RemovePossibility(isize_type);
      target_type->RemovePossibility(u32_type);
      TryToMatch(target_type, expression_ptr->children_[2]->integrated_type_,
          false);
      if (expression_ptr->children_[0]->integrated_type_->basic_type != array_type) {
        Throw("Cannot apply index operation to non-array expression.");
      }
      *expression_ptr->integrated_type_ = *expression_ptr->children_[0]->integrated_type_->element_type;
      expression_ptr->integrated_type_->is_const = false;
      if (expression_ptr->children_[2]->integrated_type_->is_const) {
        if (expression_ptr->children_[2]->value_.int_value >=
            expression_ptr->children_[0]->integrated_type_->size) {
          Throw("Index out of bound.");
        }
        if (expression_ptr->children_[0]->integrated_type_->is_const) {
          expression_ptr->integrated_type_->is_const = true;
          expression_ptr->value_ = expression_ptr->value_.array_values[expression_ptr->children_[2]->value_.int_value];
        }
      }
      break;
    }
    case struct_expr: {
      const std::string struct_name = dynamic_cast<LeafNode *>(expression_ptr->children_[0]->children_[0])
          ->GetContent().GetStr();
      const auto struct_info = expression_ptr->scope_node_->FindInType(struct_name);
      if (struct_info.node == nullptr) {
        Throw("Cannot find target struct.");
      }
      const auto struct_ptr = dynamic_cast<Struct *>(struct_info.node);
      if (!struct_ptr->field_items_.empty() &&
          struct_ptr->field_items_.begin()->second.node->integrated_type_->basic_type == unknown_type) {
        struct_info.node->Accept(this);
      }
      expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(struct_type,
          true, false, false, false, 0);
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
              false, false, false, false, 0);
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
              false, false, false, false, 0);
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
              false, false, false, false, 0);
          if (expression_ptr->children_.size() == 1 || (expression_ptr->children_[1]->children_.size() + 1) / 2 != 1) {
            Throw("printInt(n : i32) should be called with one parameter.");
          }
          expression_ptr->children_[1]->children_[0]->Accept(this);
          auto target_type = std::make_shared<IntegratedType>(i32_type,
              false, false, false, false, 0);
          target_type->RemovePossibility(isize_type);
          target_type->RemovePossibility(u32_type);
          target_type->RemovePossibility(usize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->children_[0]->integrated_type_, false);
        } else if (function_name == "printlnInt") {
          // call parameter type: i32
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, false, 0);
          if (expression_ptr->children_.size() == 1 || (expression_ptr->children_[1]->children_.size() + 1) / 2 != 1) {
            Throw("printlnInt(n : i32) should be called with one parameter.");
          }
          expression_ptr->children_[1]->children_[0]->Accept(this);
          auto target_type = std::make_shared<IntegratedType>(i32_type,
              false, false, false, false, 0);
          target_type->RemovePossibility(isize_type);
          target_type->RemovePossibility(u32_type);
          target_type->RemovePossibility(usize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->children_[0]->integrated_type_, false);
        } else if (function_name == "getString") {
          // no call parameter
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(string_type,
              false, false, false, false, 0);
          if (expression_ptr->children_.size() != 1) {
            Throw("getString() should be called with no parameter");
          }
        } else if (function_name == "getInt") {
          // no call parameter
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(i32_type,
              false, false, false, false, 0);
          expression_ptr->integrated_type_->RemovePossibility(isize_type);
          expression_ptr->integrated_type_->RemovePossibility(u32_type);
          expression_ptr->integrated_type_->RemovePossibility(usize_type);
          if (expression_ptr->children_.size() != 1) {
            Throw("getString() should be called with no parameter");
          }
        } else if (function_name == "exit") {
          // call parameter type: i32
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, false, 0);
          if (expression_ptr->children_.size() == 1 || (expression_ptr->children_[1]->children_.size() + 1) / 2 != 1) {
            Throw("exit(code : i32) should be called with one parameter.");
          }
          expression_ptr->children_[1]->children_[0]->Accept(this);
          auto target_type = std::make_shared<IntegratedType>(i32_type,
              false, false, false, false, 0);
          target_type->RemovePossibility(isize_type);
          target_type->RemovePossibility(u32_type);
          target_type->RemovePossibility(usize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->children_[0]->integrated_type_, false);
          if (wrapping_function_.size() != 1) { // not outermost
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
            Throw("Unexpected main function block ptr.");
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
          if (call_expr_param_num == 0) {
            break;
          }
          if (function_info.node->integrated_type_->basic_type == unknown_type) {
            function_info.node->Accept(this);
          }
          expression_ptr->integrated_type_ = function_info.node->integrated_type_;
          for (int i = 0; i < expression_ptr->children_[1]->children_.size(); i += 2) {
            expression_ptr->children_[1]->children_[i]->Accept(this);
            TryToMatch(function_info.node->children_[function_parameters_node_ind]->children_[i]->integrated_type_,
                expression_ptr->children_[1]->children_[i]->integrated_type_, false);
          }
        }
      } else { // expression_ptr->children_[0]->children_.size == 3
        std::string struct_name = dynamic_cast<LeafNode *>(expression_ptr->children_[0]->children_[0])
            ->GetContent().GetStr();
        const auto struct_info = expression_ptr->scope_node_->FindInType(struct_name);
        if (struct_info.node == nullptr) {
          Throw("Cannot find target struct in the scope.");
        }
        std::string function_name = dynamic_cast<LeafNode *>(expression_ptr->children_[0]->children_[3])
            ->GetContent().GetStr();
        auto struct_ptr = dynamic_cast<Struct *>(struct_info.node);
        if (!struct_ptr->associated_items_.contains(function_name)) {
          Throw("There is no target function in the associated-item field of the struct.");
        }
        const auto function_info = struct_ptr->associated_items_[function_name];
        auto function_ptr = dynamic_cast<Function *>(function_info.node);
        if (function_ptr == nullptr) {
          Throw("Cannot call a non-function.");
        }
        if (function_ptr->integrated_type_->basic_type == unknown_type) {
          function_info.node->Accept(this);
        }
        int call_expr_param_num = 0;
        if (expression_ptr->children_.size() == 2) {
          call_expr_param_num = static_cast<int>(expression_ptr->children_[1]->children_.size() + 1) / 2;
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
        if (call_expr_param_num != declared_param_num) {
          Throw("Parameter number doesn't match.");
        }
        if (call_expr_param_num == 0) {
          break;
        }
        expression_ptr->integrated_type_ = function_ptr->integrated_type_;
        for (int i = 0; i < expression_ptr->children_[1]->children_.size(); i += 2) {
          expression_ptr->children_[1]->children_[i]->Accept(this);
          TryToMatch(function_ptr->children_[function_parameters_node_ind]->children_[i]->integrated_type_,
              expression_ptr->children_[1]->children_[i]->integrated_type_, false);
        }
      }
      break;
    }
    case method_call_expr: {
      // todo
      // to_string() of type String : u32, usize
      // as_str() of type &str : String
      // len() of u32 : [T; N], &[T], String, &str
      break;
    }
    case field_expr: {
      if (expression_ptr->children_[1]->children_[0]->children_.size() != 1) {
        Throw("Expect an identifier following '.'.");
      }
      std::string identifier_name = dynamic_cast<LeafNode *>(expression_ptr->children_[1]->children_[0]->children_[0])
          ->GetContent().GetStr();
      expression_ptr->children_[0]->Accept(this);
      if (expression_ptr->children_[0]->integrated_type_->basic_type == struct_type) {
        auto struct_ptr = dynamic_cast<Struct *>(expression_ptr->children_[0]->integrated_type_->struct_node);
        // check if the struct has been visited
        if (!struct_ptr->field_items_.empty()) {
          if (struct_ptr->field_items_.begin()->second.node->integrated_type_->basic_type == unknown_type) {
            expression_ptr->children_[0]->integrated_type_->struct_node->Accept(this);
          }
        } else if (!struct_ptr->associated_items_.empty()) {
          if (struct_ptr->associated_items_.begin()->second.node->integrated_type_->basic_type == unknown_type) {
            expression_ptr->children_[0]->integrated_type_->struct_node->Accept(this);
          }
        } else {
          Throw("Empty struct field.");
        }
        // the struct is visited
        // todo
      } else if (expression_ptr->children_[0]->integrated_type_->basic_type == pointer_type) {
        // todo: need auto dereference
      } else {
        Throw("Invalid type.");
      }
      break;
    }
    case continue_expr: {
      expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(never_type,
          false, false, false, false, 0);
      break;
    }
    case break_expr: {
      if (expression_ptr->children_.size() == 2) {
        expression_ptr->children_[1]->Accept(this);
      }
      // todo
      break;
    }
    case return_expr: {
      // todo
      break;
    }
    case prefix_expr: {
      expression_ptr->children_[1]->Accept(this);
      std::string prefix = dynamic_cast<LeafNode *>(expression_ptr->children_[0])
          ->GetContent().GetStr();
      if (prefix == "-") {
        if (!expression_ptr->children_[1]->integrated_type_->is_int) {
          Throw("Cannot apply prefix '-' to non-integer expression.");
        }
        expression_ptr->integrated_type_ = expression_ptr->children_[1]->integrated_type_;
        if (expression_ptr->integrated_type_->is_const) {
          long long val = -expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(val, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = val;
        }
      } else if (prefix == "*") {
        if(expression_ptr->children_[1]->integrated_type_->basic_type != pointer_type) {
          Throw("Cannot dereference a non-pointer type.");
        }
        expression_ptr->integrated_type_ = expression_ptr->children_[1]->integrated_type_->element_type;
        if (expression_ptr->integrated_type_->is_const) {
          expression_ptr->value_ = expression_ptr->children_[1]->value_.pointer_value->value_;
        }
      } else if (prefix == "!") {
        if (expression_ptr->children_[1]->integrated_type_->basic_type != bool_type) {
          Throw("Cannot apply prefix '!' to non-bool expression.");
        }
        expression_ptr->integrated_type_ = expression_ptr->children_[1]->integrated_type_;
        if (expression_ptr->integrated_type_->is_const) {
          expression_ptr->value_.int_value = 1 - expression_ptr->children_[1]->value_.int_value;
        }
      } else if (prefix == "&") {
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(pointer_type,
            expression_ptr->children_[1]->integrated_type_->is_const, expression_ptr->children_[1]->integrated_type_->is_mutable,
            false, false, 0);
        expression_ptr->integrated_type_->element_type = expression_ptr->children_[1]->integrated_type_;
        if (expression_ptr->integrated_type_->is_const) {
          expression_ptr->value_.pointer_value = expression_ptr->children_[1];
        }
      } else if (prefix == "&&") {
        expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(pointer_type,
            expression_ptr->children_[1]->integrated_type_->is_const, expression_ptr->children_[1]->integrated_type_->is_mutable,
            false, false, 0);
        expression_ptr->integrated_type_->element_type = std::make_shared<IntegratedType>(pointer_type,
            expression_ptr->children_[1]->integrated_type_->is_const, expression_ptr->children_[1]->integrated_type_->is_mutable,
            false, false, 0);
        expression_ptr->integrated_type_->element_type->element_type = expression_ptr->children_[1]->integrated_type_;
        if (expression_ptr->integrated_type_->is_const) {
          Throw("The value of '&&' type has not been completed yet.");
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
          const long long ans = expression_ptr->children_[0]->value_.int_value
              + expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = ans;
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
          const long long ans = expression_ptr->children_[0]->value_.int_value
              - expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = ans;
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
          const long long ans = expression_ptr->children_[0]->value_.int_value
              * expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = ans;
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
          const long long ans = expression_ptr->children_[0]->value_.int_value
              / expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = ans;
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
          const long long ans = expression_ptr->children_[0]->value_.int_value
              % expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = ans;
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
          const long long ans = expression_ptr->children_[0]->value_.int_value
              & expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = ans;
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
          const long long ans = expression_ptr->children_[0]->value_.int_value
              | expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = ans;
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
          const long long ans = expression_ptr->children_[0]->value_.int_value
              ^ expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = ans;
          break;
        }
        case left_shift: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Left-shift operation is only available between integer values.");
          }
          const auto target_type = std::make_shared<IntegratedType>(u32_type,
              false, false, true, false, 0);
          target_type->RemovePossibility(i32_type);
          target_type->RemovePossibility(isize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->integrated_type_,
              false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[0]->integrated_type_;
          if (!expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          const long long ans = expression_ptr->children_[0]->value_.int_value
              << expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = ans;
          break;
        }
        case right_shift: {
          expression_ptr->children_[0]->Accept(this);
          expression_ptr->children_[1]->Accept(this);
          if (!expression_ptr->children_[0]->integrated_type_->is_int ||
              !expression_ptr->children_[1]->integrated_type_->is_int) {
            Throw("Right-shift operation is only available between integer values.");
          }
          const auto target_type = std::make_shared<IntegratedType>(u32_type,
              false, false, true, false, 0);
          target_type->RemovePossibility(i32_type);
          target_type->RemovePossibility(isize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->integrated_type_,
              false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>();
          *expression_ptr->integrated_type_ = *expression_ptr->children_[0]->integrated_type_;
          if (!expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = false;
          }
          expression_ptr->integrated_type_->is_mutable = false;
          const long long ans = expression_ptr->children_[0]->value_.int_value
              >> expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->integrated_type_);
          expression_ptr->value_.int_value = ans;
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
              false, false, false, false, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
          }
          switch (expression_ptr->children_[1]->integrated_type_->basic_type) {
            case bool_type:
            case i32_type:
            case isize_type:
            case u32_type:
            case usize_type: {
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
            case enumeration_type: {
              if (expression_ptr->children_[0]->value_.enum_value ==
                  expression_ptr->children_[1]->value_.enum_value) {
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
              false, false, false, false, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
          }
          switch (expression_ptr->children_[1]->integrated_type_->basic_type) {
            case bool_type:
            case i32_type:
            case isize_type:
            case u32_type:
            case usize_type: {
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
            case enumeration_type: {
              if (expression_ptr->children_[0]->value_.enum_value ==
                  expression_ptr->children_[1]->value_.enum_value) {
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
              false, false, false, false, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
          }
          if (expression_ptr->children_[0]->value_.int_value > expression_ptr->children_[1]->value_.int_value) {
            expression_ptr->value_.int_value = 1;
          } else {
            expression_ptr->value_.int_value = 0;
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
              false, false, false, false, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
          }
          if (expression_ptr->children_[0]->value_.int_value < expression_ptr->children_[1]->value_.int_value) {
            expression_ptr->value_.int_value = 1;
          } else {
            expression_ptr->value_.int_value = 0;
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
              false, false, false, false, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
          }
          if (expression_ptr->children_[0]->value_.int_value >= expression_ptr->children_[1]->value_.int_value) {
            expression_ptr->value_.int_value = 1;
          } else {
            expression_ptr->value_.int_value = 0;
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
              false, false, false, false, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
          }
          if (expression_ptr->children_[0]->value_.int_value <= expression_ptr->children_[1]->value_.int_value) {
            expression_ptr->value_.int_value = 1;
          } else {
            expression_ptr->value_.int_value = 0;
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
              false, false, false, false, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
          }
          if (expression_ptr->children_[0]->value_.int_value || expression_ptr->children_[1]->value_.int_value) {
            expression_ptr->value_.int_value = 1;
          } else {
            expression_ptr->value_.int_value = 0;
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
              false, false, false, false, 0);
          if (expression_ptr->children_[0]->integrated_type_->is_const &&
              expression_ptr->children_[1]->integrated_type_->is_const) {
            expression_ptr->integrated_type_->is_const = true;
          }
          if (expression_ptr->children_[0]->value_.int_value && expression_ptr->children_[1]->value_.int_value) {
            expression_ptr->value_.int_value = 1;
          } else {
            expression_ptr->value_.int_value = 0;
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
              false, false, false, false, 0);
          switch (expression_ptr->children_[1]->integrated_type_->basic_type) {
            case bool_type:
            case i32_type:
            case isize_type:
            case u32_type:
            case usize_type: {
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
            case enumeration_type: {
              expression_ptr->children_[0]->value_.enum_value = expression_ptr->children_[1]->value_.enum_value;
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
              false, false, false, false, 0);
          const long long ans = expression_ptr->children_[0]->value_.int_value
              + expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->children_[0]->integrated_type_);
          expression_ptr->children_[0]->value_.int_value = ans;
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
              false, false, false, false, 0);
          const long long ans = expression_ptr->children_[0]->value_.int_value
              - expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->children_[0]->integrated_type_);
          expression_ptr->children_[0]->value_.int_value = ans;
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
              false, false, false, false, 0);
          const long long ans = expression_ptr->children_[0]->value_.int_value
              * expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->children_[0]->integrated_type_);
          expression_ptr->children_[0]->value_.int_value = ans;
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
              false, false, false, false, 0);
          const long long ans = expression_ptr->children_[0]->value_.int_value
              / expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->children_[0]->integrated_type_);
          expression_ptr->children_[0]->value_.int_value = ans;
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
              false, false, false, false, 0);
          const long long ans = expression_ptr->children_[0]->value_.int_value
              % expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->children_[0]->integrated_type_);
          expression_ptr->children_[0]->value_.int_value = ans;
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
              false, false, false, false, 0);
          const long long ans = expression_ptr->children_[0]->value_.int_value
              & expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->children_[0]->integrated_type_);
          expression_ptr->children_[0]->value_.int_value = ans;
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
              false, false, false, false, 0);
          const long long ans = expression_ptr->children_[0]->value_.int_value
              | expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->children_[0]->integrated_type_);
          expression_ptr->children_[0]->value_.int_value = ans;
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
              false, false, false, false, 0);
          const long long ans = expression_ptr->children_[0]->value_.int_value
              ^ expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->children_[0]->integrated_type_);
          expression_ptr->children_[0]->value_.int_value = ans;
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
              false, false, true, false, 0);
          target_type->RemovePossibility(i32_type);
          target_type->RemovePossibility(isize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->integrated_type_,
              false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, false, 0);
          const long long ans = expression_ptr->children_[0]->value_.int_value
              << expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->children_[0]->integrated_type_);
          expression_ptr->children_[0]->value_.int_value = ans;
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
              false, false, true, false, 0);
          target_type->RemovePossibility(i32_type);
          target_type->RemovePossibility(isize_type);
          TryToMatch(target_type, expression_ptr->children_[1]->integrated_type_,
              false);
          expression_ptr->integrated_type_ = std::make_shared<IntegratedType>(unit_type,
              false, false, false, false, 0);
          const long long ans = expression_ptr->children_[0]->value_.int_value
              >> expression_ptr->children_[1]->value_.int_value;
          CheckOverflow(ans, expression_ptr->children_[0]->integrated_type_);
          expression_ptr->children_[0]->value_.int_value = ans;
          break;
        }
        case type_cast: {
          expression_ptr->children_[0]->Accept(this);
          is_reading_type_ = true;
          type_owner_ = expression_ptr;
          expression_ptr->children_[1]->Accept(this);
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
          break;
        }
        default:;
      }
    }
  }
}