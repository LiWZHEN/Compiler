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
    it->AddSymbol(crate_ptr->scope_node_.get(), false, false, false,
        false, {nullptr, type_crate}, {nullptr, type_crate});
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
      function_ptr->children_[i]->AddSymbol(function_ptr->children_[i]->scope_node_.get(), false, false,
          false, false, {nullptr, type_crate}, {nullptr, type_crate});
      SetCurrentScope(function_ptr->children_[i]->scope_node_);
      function_ptr->children_[i]->Accept(this);
      SetCurrentScope(function_ptr->scope_node_);
    } else if (function_ptr->type_[i] == type_function_parameters) {
      // function parameters
      parameters_ind = i;
      function_ptr->children_[i]->scope_node_ = std::make_shared<ScopeNode>(function_ptr->scope_node_);
      function_ptr->children_[i]->AddSymbol(function_ptr->children_[i]->scope_node_.get(), false, true,
          false, false, {nullptr, type_crate}, {nullptr, type_crate});
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
  for (int i = 0; i < struct_ptr->children_.size(); ++i) {
    if (struct_ptr->type_[i] != type_struct_fields) {
      continue;
    }
    struct_ptr->children_[i]->AddSymbol(nullptr, false, false,
        false, true, {struct_ptr, type_struct},
        {nullptr, type_crate});
  }
  for (const auto it : struct_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Enumeration *enumeration_ptr) {
  for (int i = 0; i < enumeration_ptr->children_.size(); ++i) {
    if (enumeration_ptr->type_[i] != type_enum_variants) {
      continue;
    }
    enumeration_ptr->children_[i]->AddSymbol(nullptr, false, false,
        false, true, {enumeration_ptr, type_enumeration},
        {nullptr, type_crate});
  }
  for (const auto it : enumeration_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ConstantItem *constant_item_ptr) {
  for (const auto it : constant_item_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Trait *trait_ptr) {
  for (int i = 0; i < trait_ptr->children_.size(); ++i) {
    if (trait_ptr->type_[i] != type_associated_item) {
      continue;
    }
    trait_ptr->children_[i]->AddSymbol(nullptr, false, false, true,
        false, {trait_ptr, type_trait}, {nullptr, type_crate});
  }
  for (const auto it : trait_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Implementation *implementation_ptr) {
  const Node *target_type;
  if (implementation_ptr->type_[2] == type_keyword) { // trait impl
    target_type = implementation_ptr->children_[3];
    throw "";
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
  ScopeNodeContent type_content = current_scope_node_->FindInType(type_name);
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
      implementation_ptr->children_[i]->AddSymbol(nullptr, false, false, true,
        false, {struct_ptr, type_struct}, {nullptr, type_crate});
    }
    // visit
    for (int i = 0; i < implementation_ptr->children_.size(); ++i) {
      if (implementation_ptr->type_[i] != type_associated_item) {
        implementation_ptr->children_[i]->scope_node_ = implementation_ptr->scope_node_;
        implementation_ptr->children_[i]->Accept(this);
      } else {
        implementation_ptr->children_[i]->scope_node_ = struct_ptr->scope_node_;
        implementation_ptr->children_[i]->Accept(this);
      }
    }
  } else if (type_content.node_type == type_enumeration) {
    const auto enum_ptr = dynamic_cast<Enumeration *>(type_content.node);
    for (int i = 0; i < implementation_ptr->children_.size(); ++i) {
      if (implementation_ptr->type_[i] != type_associated_item) {
        continue;
      }
      implementation_ptr->children_[i]->AddSymbol(nullptr, false, false, true,
        false, {enum_ptr, type_struct}, {nullptr, type_crate});
    }
    // visit
    for (int i = 0; i < implementation_ptr->children_.size(); ++i) {
      if (implementation_ptr->type_[i] != type_associated_item) {
        implementation_ptr->children_[i]->scope_node_ = implementation_ptr->scope_node_;
        implementation_ptr->children_[i]->Accept(this);
      } else {
        implementation_ptr->children_[i]->scope_node_ = enum_ptr->scope_node_;
        implementation_ptr->children_[i]->Accept(this);
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
  for (const auto it : function_parameters_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(FunctionReturnType *function_return_type_ptr) {
  for (const auto it : function_return_type_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(BlockExpression *block_expression_ptr) {
  for (const auto it : block_expression_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(SelfParam *self_param_ptr) {
  for (const auto it : self_param_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(FunctionParam *function_param_ptr) {
  for (const auto it : function_param_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ShorthandSelf *shorthand_self_ptr) {
  for (const auto it : shorthand_self_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Type *type_ptr) {
  for (const auto it : type_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Pattern *pattern_ptr) {
  for (const auto it : pattern_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ReferencePattern *reference_pattern_ptr) {
  for (const auto it : reference_pattern_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(IdentifierPattern *identifier_pattern_ptr) {
  for (const auto it : identifier_pattern_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(PathInExpression *path_in_expression_ptr) {
  for (const auto it : path_in_expression_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(PathExprSegment *path_expr_segment_ptr) {
  for (const auto it : path_expr_segment_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ReferenceType *reference_type_ptr) {
  for (const auto it : reference_type_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ArrayType *array_type_ptr) {
  for (const auto it : array_type_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(TypePath *type_path_ptr) {
  for (const auto it : type_path_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(UnitType *unit_type_ptr) {
  for (const auto it : unit_type_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(Expression *expression_ptr) {
  switch (expression_ptr->GetExprType()) {
    case block_expr:
    case infinite_loop_expr:
    case predicate_loop_expr:
    case if_expr: {
      for (int i = 0; i < expression_ptr->children_.size(); ++i) {
        if (expression_ptr->type_[i] != type_statements) {
          expression_ptr->children_[i]->scope_node_ = expression_ptr->scope_node_;
          expression_ptr->children_[i]->Accept(this);
        } else {
          expression_ptr->children_[i]->scope_node_ = std::make_shared<ScopeNode>(expression_ptr->scope_node_);
          expression_ptr->children_[i]->AddSymbol(expression_ptr->children_[i]->scope_node_.get(),
              true, true, false, false,
              {nullptr, type_crate}, {nullptr, type_crate});
          expression_ptr->children_[i]->Accept(this);
        }
      }
      break;
    }
    default:;
  }
}
void SymbolVisitor::Visit(Statements *statements_ptr) {
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
  for (const auto it : statement_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(LetStatement *let_statement_ptr) {
  for (const auto it : let_statement_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(ExpressionStatement *expression_statement_ptr) {
  for (const auto it : expression_statement_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(StructExprFields *struct_expr_fields_ptr) {
  for (const auto it : struct_expr_fields_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(StructExprField *struct_expr_field_ptr) {
  for (const auto it : struct_expr_field_ptr->children_) {
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
  for (const auto it : struct_fields_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(StructField *struct_field_ptr) {
  for (const auto it : struct_field_ptr->children_) {
    it->scope_node_ = current_scope_node_;
    it->Accept(this);
  }
}
void SymbolVisitor::Visit(EnumVariants *enum_variants_ptr) {
  for (const auto it : enum_variants_ptr->children_) {
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