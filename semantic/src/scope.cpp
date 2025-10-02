#include "scope.h"
#include "node.h"
#include "item.h"
#include "type.h"
#include "functions.h"
#include "function_parameters.h"
#include "pattern.h"
#include "path.h"
#include "expression.h"
#include "statements.h"
#include "structs.h"
#include "enumerations.h"
#include "trait.h"

void SymbolVisitor::Visit(Crate *crate_ptr) {
  crate_ptr->scope_node_ = std::make_shared<ScopeNode>(nullptr);
  SetCurrentScope(crate_ptr->scope_node_);
  // put all the symbols in current scope
  for (const auto it : crate_ptr->children_) {
    // "it" always points to a node of item type
    switch (it->type_[0]) {
      case type_function: {
        const auto function_ptr = dynamic_cast<Function *>(it->children_[0]);
        current_scope_node_->ValueAdd(function_ptr->GetIdentifier(), type_function, it->children_[0], function_ptr->IsConst());
        break;
      }
      case type_struct: {
        const auto struct_ptr = dynamic_cast<Struct *>(it->children_[0]);
        current_scope_node_->ValueAdd(struct_ptr->GetIdentifier(), type_struct, it->children_[0], false);
        current_scope_node_->TypeAdd(struct_ptr->GetIdentifier(), type_struct, it->children_[0], false);
        break;
      }
      case type_enumeration: {
        const auto enumeration_ptr = dynamic_cast<Enumeration *>(it->children_[0]);
        current_scope_node_->ValueAdd(enumeration_ptr->GetIdentifier(), type_enumeration, it->children_[0], true);
        current_scope_node_->TypeAdd(enumeration_ptr->GetIdentifier(), type_enumeration, it->children_[0], true);
        break;
      }
      case type_constant_item: {
        const auto constant_item_ptr = dynamic_cast<ConstantItem *>(it->children_[0]);
        current_scope_node_->ValueAdd(constant_item_ptr->GetIdentifier(), type_constant_item, it->children_[0], true);
        break;
      }
      case type_trait: {
        const auto trait_ptr = dynamic_cast<Trait *>(it->children_[0]);
        current_scope_node_->TypeAdd(trait_ptr->GetIdentifier(), type_trait, it->children_[0], false);
        break;
      }
      default:;
    }
  }
  // call accept functions of child nodes
  for (const auto it : crate_ptr->children_) {
    if (it->type_[0] == type_implementation) {
      continue;
    }
    // it's not an item of implementation
    it->scope_node_ = crate_ptr->scope_node_;
    it->Accept(this);
  }
  for (const auto it : crate_ptr->children_) {
    if (it->type_[0] != type_implementation) {
      continue;
    }
    // it's an item of implementation
    it->scope_node_ = crate_ptr->scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Item *item_ptr) {
  item_ptr->children_[0]->scope_node_ = item_ptr->scope_node_;
  item_ptr->children_[0]->Accept(this);
}
void SymbolVisitor::Visit(Function *function_ptr) {
  int parameters_ind = -1;
  for (int i = 0; i < function_ptr->children_.size(); ++i) {
    if (function_ptr->type_[i] == type_block_expression) {
      // block expression
      if (parameters_ind == -1) {
        function_ptr->children_[i]->scope_node_ = std::make_shared<ScopeNode>(function_ptr->scope_node_);
      } else {
        function_ptr->children_[i]->scope_node_ = function_ptr->children_[parameters_ind]->scope_node_;
      }
      SetCurrentScope(function_ptr->children_[i]->scope_node_);
      function_ptr->children_[i]->Accept(this);
      SetCurrentScope(function_ptr->scope_node_);
    } else if (function_ptr->type_[i] == type_function_parameters) {
      // function parameters
      parameters_ind = i;
      function_ptr->children_[i]->scope_node_ = std::make_shared<ScopeNode>(function_ptr->scope_node_);
      SetCurrentScope(function_ptr->children_[i]->scope_node_);
      function_ptr->children_[i]->Accept(this);
      SetCurrentScope(function_ptr->scope_node_);
    } else {
      function_ptr->children_[i]->scope_node_ = current_scope_node_;
      function_ptr->children_[i]->Accept(this);
    }
  }
}
void SymbolVisitor::Visit(Struct *struct_ptr) {
  for (const auto &it : struct_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Enumeration *enumeration_ptr) {
  for (const auto &it : enumeration_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ConstantItem *constant_item_ptr) {
  for (const auto &it : constant_item_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Trait *trait_ptr) {
  int first_associated_item = -1;
  for (int i = 0; i < trait_ptr->children_.size(); ++i) {
    if (trait_ptr->type_[i] != type_associated_item) {
      trait_ptr->children_[i]->scope_node_ = current_scope_node_;
      trait_ptr->children_[i]->Accept(this);
    } else {
      if (first_associated_item == -1) {
        first_associated_item = i;
        trait_ptr->children_[i]->scope_node_ = std::make_shared<ScopeNode>(trait_ptr->scope_node_);
      } else {
        trait_ptr->children_[i]->scope_node_ = trait_ptr->children_[first_associated_item]->scope_node_;
      }
      SetCurrentScope(trait_ptr->children_[i]->scope_node_);
      trait_ptr->children_[i]->Accept(this);
      SetCurrentScope(trait_ptr->scope_node_);
    }
  }
}
void SymbolVisitor::Visit(Implementation *implementation_ptr) {
  const Node *target_type;
  if (implementation_ptr->type_[2] == type_keyword) { // trait impl
    target_type = implementation_ptr->children_[3];
  } else { // inherent impl
    target_type = implementation_ptr->children_[1];
  }
  std::string type_name;
  if (target_type->type_[0] == type_type_path) {
    type_name = dynamic_cast<LeafNode *>(target_type->children_[0]->children_[0])->GetContent().GetStr();
  } else {
    std::cerr << "Expect a name of a struct or an enumeration.\n";
    throw "";
  }
  ScopeNodeContent type_content = {false, nullptr, type_crate};
  std::shared_ptr<ScopeNode> scope_ptr = current_scope_node_;
  while (scope_ptr != nullptr) {
    if (scope_ptr->type_namespace.contains(type_name)) {
      type_content = scope_ptr->type_namespace[type_name];
      break;
    }
    scope_ptr = scope_ptr->parent;
  }
  if (type_content.node == nullptr) {
    std::cerr << "Cannot find target struct / enumeration.\n";
    throw "";
  }
  if (type_content.node_type == type_struct) {
    const auto struct_ptr = dynamic_cast<Struct *>(type_content.node);
    // add all the associated item into the unordered map
    for (int i = 0; i < implementation_ptr->children_.size(); ++i) {
      if (implementation_ptr->type_[i] != type_associated_item) {
        continue;
      }
      const auto associated_item_ptr = implementation_ptr->children_[i];
      if (associated_item_ptr->type_[0] == type_constant_item) {
        const auto const_item_ptr = dynamic_cast<ConstantItem *>(associated_item_ptr->children_[0]);
        const std::string associated_item_name = const_item_ptr->GetIdentifier();
        if (struct_ptr->associated_items_.contains(associated_item_name)) {
          std::cerr << "Double declaration!\n";
          throw "";
        }
        struct_ptr->associated_items_[associated_item_name] = {true, associated_item_ptr->children_[0], type_constant_item};
      } else { // associated_item_ptr->type_[0] == type_function
        const auto function_ptr = dynamic_cast<Function *>(associated_item_ptr->children_[0]);
        const std::string associated_item_name = function_ptr->GetIdentifier();
        if (struct_ptr->associated_items_.contains(associated_item_name)) {
          std::cerr << "Double declaration!\n";
          throw "";
        }
        struct_ptr->associated_items_[associated_item_name] = {false, associated_item_ptr->children_[0], type_function};
      }
    }
    // visit
    for (int i = 0; i < implementation_ptr->children_.size(); ++i) {
      if (implementation_ptr->type_[i] != type_associated_item) {
        implementation_ptr->children_[i]->scope_node_ = implementation_ptr->scope_node_;
        implementation_ptr->children_[i]->Accept(this);
      } else {
        implementation_ptr->children_[i]->scope_node_ = std::make_shared<ScopeNode>(struct_ptr->scope_node_);
        // add associated item info into new scope
        const auto associated_it_ptr = implementation_ptr->children_[i];
        std::string associated_item_name;
        if (associated_it_ptr->type_[0] == type_constant_item) {
          const auto const_item_ptr = dynamic_cast<ConstantItem *>(associated_it_ptr->children_[0]);
          associated_item_name = const_item_ptr->GetIdentifier();
          implementation_ptr->children_[i]->scope_node_->ValueAdd(associated_item_name, type_constant_item, const_item_ptr, true);
        } else { // associated_item_ptr->type_[0] == type_function
          const auto function_ptr = dynamic_cast<Function *>(associated_it_ptr->children_[0]);
          associated_item_name = function_ptr->GetIdentifier();
          implementation_ptr->children_[i]->scope_node_->ValueAdd(associated_item_name, type_function, function_ptr, false);
        }
        SetCurrentScope(implementation_ptr->children_[i]->scope_node_);
        implementation_ptr->children_[i]->Accept(this);
        SetCurrentScope(implementation_ptr->scope_node_);
      }
    }
  } else if (type_content.node_type == type_enumeration) {
    const auto enum_ptr = dynamic_cast<Enumeration *>(type_content.node);
    for (int i = 0; i < implementation_ptr->children_.size(); ++i) {
      if (implementation_ptr->type_[i] != type_associated_item) {
        continue;
      }
      const auto associated_item_ptr = implementation_ptr->children_[i];
      if (associated_item_ptr->type_[0] == type_constant_item) {
        const auto const_item_ptr = dynamic_cast<ConstantItem *>(associated_item_ptr->children_[0]);
        const std::string associated_item_name = const_item_ptr->GetIdentifier();
        if (enum_ptr->associated_items_.contains(associated_item_name)) {
          std::cerr << "Double declaration!\n";
          throw "";
        }
        enum_ptr->associated_items_[associated_item_name] = {true, associated_item_ptr->children_[0], type_constant_item};
      } else { // associated_item_ptr->type_[0] == type_function
        const auto function_ptr = dynamic_cast<Function *>(associated_item_ptr->children_[0]);
        const std::string associated_item_name = function_ptr->GetIdentifier();
        if (enum_ptr->associated_items_.contains(associated_item_name)) {
          std::cerr << "Double declaration!\n";
          throw "";
        }
        enum_ptr->associated_items_[associated_item_name] = {false, associated_item_ptr->children_[0], type_function};
      }
    }
    // visit
    for (int i = 0; i < implementation_ptr->children_.size(); ++i) {
      if (implementation_ptr->type_[i] != type_associated_item) {
        implementation_ptr->children_[i]->scope_node_ = implementation_ptr->scope_node_;
        implementation_ptr->children_[i]->Accept(this);
      } else {
        implementation_ptr->children_[i]->scope_node_ = std::make_shared<ScopeNode>(enum_ptr->scope_node_);
        // add associated item info into new scope
        const auto associated_it_ptr = implementation_ptr->children_[i];
        std::string associated_item_name;
        if (associated_it_ptr->type_[0] == type_constant_item) {
          const auto const_item_ptr = dynamic_cast<ConstantItem *>(associated_it_ptr->children_[0]);
          associated_item_name = const_item_ptr->GetIdentifier();
          implementation_ptr->children_[i]->scope_node_->ValueAdd(associated_item_name, type_constant_item, const_item_ptr, true);
        } else { // associated_item_ptr->type_[0] == type_function
          const auto function_ptr = dynamic_cast<Function *>(associated_it_ptr->children_[0]);
          associated_item_name = function_ptr->GetIdentifier();
          implementation_ptr->children_[i]->scope_node_->ValueAdd(associated_item_name, type_function, function_ptr, false);
        }
        SetCurrentScope(implementation_ptr->children_[i]->scope_node_);
        implementation_ptr->children_[i]->Accept(this);
        SetCurrentScope(implementation_ptr->scope_node_);
      }
    }
  } else {
    std::cerr << "The type does not correspond to a struct or an enumeration.\n";
    throw "";
  }
}
void SymbolVisitor::Visit(Keyword *keyword_ptr) {}
void SymbolVisitor::Visit(Identifier *identifier_ptr) {}
void SymbolVisitor::Visit(Punctuation *punctuation_ptr) {}
void SymbolVisitor::Visit(FunctionParameters *function_parameters_ptr) {
  for (const auto &it : function_parameters_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(FunctionReturnType *function_return_type_ptr) {
  for (const auto &it : function_return_type_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(BlockExpression *block_expression_ptr) {
  for (int i = 0; i < block_expression_ptr->children_.size(); ++i) {
    if (block_expression_ptr->type_[i] == type_statements) {
      block_expression_ptr->children_[i]->scope_node_ = std::make_shared<ScopeNode>(block_expression_ptr->scope_node_);
      SetCurrentScope(block_expression_ptr->children_[i]->scope_node_);
      block_expression_ptr->children_[i]->Accept(this);
      SetCurrentScope(block_expression_ptr->scope_node_);
    } else {
      block_expression_ptr->children_[i]->scope_node_ = block_expression_ptr->scope_node_;
      block_expression_ptr->children_[i]->Accept(this);
    }
  }
}
void SymbolVisitor::Visit(SelfParam *self_param_ptr) {
  for (const auto &it : self_param_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(FunctionParam *function_param_ptr) {
  for (const auto &it : function_param_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ShorthandSelf *shorthand_self_ptr) {
  for (const auto &it : shorthand_self_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Type *type_ptr) {
  for (const auto &it : type_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Pattern *pattern_ptr) {
  for (const auto &it : pattern_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ReferencePattern *reference_pattern_ptr) {
  for (const auto &it : reference_pattern_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(IdentifierPattern *identifier_pattern_ptr) {
  for (const auto &it : identifier_pattern_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(LiteralPattern *literal_pattern_ptr) {
  for (const auto &it : literal_pattern_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(PathInExpression *path_in_expression_ptr) {
  for (const auto &it : path_in_expression_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(LiteralExpression *literal_expression_ptr) {
  for (const auto &it : literal_expression_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(PathExprSegment *path_expr_segment_ptr) {
  for (const auto &it : path_expr_segment_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ReferenceType *reference_type_ptr) {
  for (const auto &it : reference_type_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ArrayType *array_type_ptr) {
  for (const auto &it : array_type_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(TypePath *type_path_ptr) {
  for (const auto &it : type_path_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(UnitType *unit_type_ptr) {
  for (const auto &it : unit_type_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Expression *expression_ptr) {
  for (const auto &it : expression_ptr->children_) {
    it->scope_node_ = expression_ptr->scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Statements *statements_ptr) {
  // add identifiers into current scope
  for (int i = 0; i < statements_ptr->children_.size(); ++i) {
    if (statements_ptr->type_[i] == type_statement) {
      if (statements_ptr->children_[i]->type_[0] == type_item) {
        auto item_ptr = dynamic_cast<Item *>(statements_ptr->children_[i]->children_[0]);
        switch (item_ptr->type_[0]) {
          case type_function: {
            const auto function_ptr = dynamic_cast<Function *>(item_ptr->children_[0]);
            current_scope_node_->ValueAdd(function_ptr->GetIdentifier(), type_function, item_ptr->children_[0], function_ptr->IsConst());
            break;
          }
          case type_struct: {
            const auto struct_ptr = dynamic_cast<Struct *>(item_ptr->children_[0]);
            current_scope_node_->ValueAdd(struct_ptr->GetIdentifier(), type_struct, item_ptr->children_[0], false);
            current_scope_node_->TypeAdd(struct_ptr->GetIdentifier(), type_struct, item_ptr->children_[0], false);
            break;
          }
          case type_enumeration: {
            const auto enumeration_ptr = dynamic_cast<Enumeration *>(item_ptr->children_[0]);
            current_scope_node_->ValueAdd(enumeration_ptr->GetIdentifier(), type_enumeration, item_ptr->children_[0], true);
            current_scope_node_->TypeAdd(enumeration_ptr->GetIdentifier(), type_enumeration, item_ptr->children_[0], true);
            break;
          }
          case type_constant_item: {
            const auto constant_item_ptr = dynamic_cast<ConstantItem *>(item_ptr->children_[0]);
            current_scope_node_->ValueAdd(constant_item_ptr->GetIdentifier(), type_constant_item, item_ptr->children_[0], true);
            break;
          }
          case type_trait: {
            const auto trait_ptr = dynamic_cast<Trait *>(item_ptr->children_[0]);
            current_scope_node_->TypeAdd(trait_ptr->GetIdentifier(), type_trait, item_ptr->children_[0], false);
            break;
          }
          default:;
        }
      }
    }
  }
  // call Accept()
  for (int i = 0; i < statements_ptr->children_.size(); ++i) {
    if (statements_ptr->type_[i] == type_expression) {
      statements_ptr->children_[i]->scope_node_ = statements_ptr->scope_node_;
      statements_ptr->children_[i]->Accept(this);
    } else {
      // statement
      if (statements_ptr->children_[i]->type_[0] == type_item && statements_ptr->children_[i]->children_[0]->type_[0] == type_implementation) {
        continue; // skip implement
      }
      statements_ptr->children_[i]->scope_node_ = statements_ptr->scope_node_;
      statements_ptr->children_[i]->Accept(this);
    }
  }
  for (int i = 0; i < statements_ptr->children_.size(); ++i) {
    if (statements_ptr->type_[i] == type_expression) {
      continue;
    }
    // only access implement
    if (statements_ptr->children_[i]->type_[0] == type_item && statements_ptr->children_[i]->children_[0]->type_[0] == type_implementation) {
      statements_ptr->children_[i]->scope_node_ = statements_ptr->scope_node_;
      statements_ptr->children_[i]->Accept(this);
    }
  }
}
void SymbolVisitor::Visit(Statement *statement_ptr) {
  for (const auto &it : statement_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(LetStatement *let_statement_ptr) {
  for (const auto &it : let_statement_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ExpressionStatement *expression_statement_ptr) {
  for (const auto &it : expression_statement_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(StructExprFields *struct_expr_fields_ptr) {
  for (const auto &it : struct_expr_fields_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(StructExprField *struct_expr_field_ptr) {
  for (const auto &it : struct_expr_field_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(CharLiteral *char_literal_ptr) {}
void SymbolVisitor::Visit(StringLiteral *string_literal_ptr) {}
void SymbolVisitor::Visit(RawStringLiteral *raw_string_literal_ptr) {}
void SymbolVisitor::Visit(CStringLiteral *c_string_literal_ptr) {}
void SymbolVisitor::Visit(RawCStringLiteral *raw_c_string_literal_ptr) {}
void SymbolVisitor::Visit(IntegerLiteral *integer_literal_ptr) {}
void SymbolVisitor::Visit(StructFields *struct_fields_ptr) {
  for (const auto &it : struct_fields_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(StructField *struct_field_ptr) {
  for (const auto &it : struct_field_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(EnumVariants *enum_variants_ptr) {
  for (const auto &it : enum_variants_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(AssociatedItem *associated_item_ptr) {
  associated_item_ptr->children_[0]->scope_node_ = associated_item_ptr->scope_node_;
  associated_item_ptr->children_[0]->Accept(this);
}

void SymbolVisitor::SetCurrentScope(const std::shared_ptr<ScopeNode> &new_current_scope_node) {
  current_scope_node_ = new_current_scope_node;
}