#include "node.h"
#include "item.h"
#include "leaf_node.h"
#include "functions.h"
#include "structs.h"
#include "enumerations.h"
#include "constant_items.h"
#include "trait.h"
#include "implementation.h"
#include "block_expression.h"
#include "function_parameters.h"

Node::Node(const std::vector<Token> &tokens, int &ptr) : tokens_(tokens), ptr_(ptr) {}

Node::~Node() {
  for (auto &it : children_) {
    delete it;
    it = nullptr;
  }
  children_.resize(0);
  type_.resize(0);
}

void Node::AddChild(NodeType node_type) {
  int target = static_cast<int>(children_.size());
  switch (node_type) {
    case type_item :
      children_.push_back(nullptr);
      children_[target] = new Item(tokens_, ptr_);
      type_.push_back(type_item);
      break;
    case type_function :
      children_.push_back(nullptr);
      children_[target] = new Function(tokens_, ptr_);
      type_.push_back(type_function);
      break;
    case type_struct :
      children_.push_back(nullptr);
      children_[target] = new Struct(tokens_, ptr_);
      type_.push_back(type_struct);
      break;
    case type_enumeration :
      children_.push_back(nullptr);
      children_[target] = new Enumeration(tokens_, ptr_);
      type_.push_back(type_enumeration);
      break;
    case type_constant_item :
      children_.push_back(nullptr);
      children_[target] = new ConstantItem(tokens_, ptr_);
      type_.push_back(type_constant_item);
      break;
    case type_trait :
      children_.push_back(nullptr);
      children_[target] = new Trait(tokens_, ptr_);
      type_.push_back(type_trait);
      break;
    case type_implementation :
      children_.push_back(nullptr);
      children_[target] = new Implementation(tokens_, ptr_);
      type_.push_back(type_implementation);
      break;
    case type_keyword :
      children_.push_back(nullptr);
      children_[target] = new Keyword(tokens_, ptr_);
      type_.push_back(type_keyword);
      break;
    case type_identifier :
      children_.push_back(nullptr);
      children_[target] = new Identifier(tokens_, ptr_);
      type_.push_back(type_identifier);
      break;
    case type_punctuation :
      children_.push_back(nullptr);
      children_[target] = new Punctuation(tokens_, ptr_);
      type_.push_back(type_punctuation);
      break;
    /*case type_generic_params :
      children_.push_back(nullptr);
      children_[target] = new GenericParams(tokens_, ptr_);
      type_.push_back(type_generic_params);
      break;*/
    case type_function_parameters :
      children_.push_back(nullptr);
      children_[target] = new FunctionParameters(tokens_, ptr_);
      type_.push_back(type_function_parameters);
      break;
    case type_function_return_type :
      children_.push_back(nullptr);
      children_[target] = new FunctionReturnType(tokens_, ptr_);
      type_.push_back(type_function_return_type);
      break;
    /*case type_where_clause :
      children_.push_back(nullptr);
      children_[target] = new WhereClause(tokens_, ptr_);
      type_.push_back(type_where_clause);
      break;*/
    case type_block_expression :
      children_.push_back(nullptr);
      children_[target] = new BlockExpression(tokens_, ptr_);
      type_.push_back(type_block_expression);
      break;
    case type_struct_fields :
      children_.push_back(nullptr);
      children_[target] = new StructFields(tokens_, ptr_);
      type_.push_back(type_struct_fields);
      break;
    case type_enum_variants :
      children_.push_back(nullptr);
      children_[target] = new EnumVariants(tokens_, ptr_);
      type_.push_back(type_enum_variants);
      break;
    case type_type :
      children_.push_back(nullptr);
      children_[target] = new Type(tokens_, ptr_);
      type_.push_back(type_type);
      break;
    case type_expression :
      children_.push_back(nullptr);
      children_[target] = new Expression(tokens_, ptr_);
      type_.push_back(type_expression);
      break;
    case type_type_param_bounds :
      children_.push_back(nullptr);
      children_[target] = new TypeParamBounds(tokens_, ptr_);
      type_.push_back(type_type_param_bounds);
      break;
    case type_associated_item :
      children_.push_back(nullptr);
      children_[target] = new AssociatedItem(tokens_, ptr_);
      type_.push_back(type_associated_item);
      break;
    case type_type_path :
      children_.push_back(nullptr);
      children_[target] = new TypePath(tokens_, ptr_);
      type_.push_back(type_type_path);
      break;
    /*case type_generic_param :
      children_.push_back(nullptr);
      children_[target] = new GenericParam(tokens_, ptr_);
      type_.push_back(type_generic_param);
      break;*/
    case type_self_param :
      children_.push_back(nullptr);
      children_[target] = new SelfParam(tokens_, ptr_);
      type_.push_back(type_self_param);
      break;
    case type_shorthand_self :
      children_.push_back(nullptr);
      children_[target] = new ShorthandSelf(tokens_, ptr_);
      type_.push_back(type_shorthand_self);
      break;
    case type_typed_self :
      children_.push_back(nullptr);
      children_[target] = new TypedSelf(tokens_, ptr_);
      type_.push_back(type_typed_self);
      break;
    case type_function_param :
      children_.push_back(nullptr);
      children_[target] = new FunctionParam(tokens_, ptr_);
      type_.push_back(type_function_param);
      break;
    case type_function_param_pattern :
      children_.push_back(nullptr);
      children_[target] = new FunctionParamPattern(tokens_, ptr_);
      type_.push_back(type_function_param_pattern);
      break;
    /*case type_where_clause_item :
      children_.push_back(nullptr);
      children_[target] = new WhereClauseItem(tokens_, ptr_);
      type_.push_back(type_where_clause_item);
      break;*/
    case type_statements :
      children_.push_back(nullptr);
      children_[target] = new Statements(tokens_, ptr_);
      type_.push_back(type_statements);
      break;
    default:
      std::cerr << "Invalid type!\n";
      throw "";
  }
}

void Node::ThrowErr(const NodeType node_type, const std::string &info) {
  switch (node_type) {
    case type_crate:
      std::cerr << "Crate: ";
      break;
    case type_item:
      std::cerr << "Item: ";
      break;
    case type_function:
      std::cerr << "Function: ";
      break;
    case type_struct:
      std::cerr << "Struct: ";
      break;
    case type_enumeration:
      std::cerr << "Enumeration: ";
      break;
    case type_constant_item:
      std::cerr << "ConstantItem: ";
      break;
    case type_trait:
      std::cerr << "Trait: ";
      break;
    case type_implementation:
      std::cerr << "Implementation: ";
      break;
    case type_keyword:
      std::cerr << "Keyword: ";
      break;
    case type_identifier:
      std::cerr << "Identifier: ";
      break;
    case type_punctuation:
      std::cerr << "Punctuation: ";
      break;
    /*case type_generic_params:
      std::cerr << "GenericParams: ";
      break;*/
    case type_function_parameters:
      std::cerr << "FunctionParameters: ";
      break;
    case type_function_return_type:
      std::cerr << "ReturnType: ";
      break;
    /*case type_where_clause:
      std::cerr << "WhereClause: ";
      break;*/
    case type_block_expression:
      std::cerr << "BlockExpression: ";
      break;
    case type_struct_fields:
      std::cerr << "StructFields: ";
      break;
    case type_enum_variants:
      std::cerr << "EnumVariants: ";
      break;
    case type_type:
      std::cerr << "Type: ";
      break;
    case type_expression:
      std::cerr << "Expression: ";
      break;
    case type_type_param_bounds:
      std::cerr << "TypeParamBounds: ";
      break;
    case type_associated_item:
      std::cerr << "AssociatedItem: ";
      break;
    case type_type_path:
      std::cerr << "TypePath: ";
      break;
    case type_generic_param:
      std::cerr << "GenericParam: ";
      break;
    case type_self_param:
      std::cerr << "SelfParam: ";
      break;
    case type_shorthand_self:
      std::cerr << "ShorthandSelf: ";
      break;
    case type_typed_self:
      std::cerr << "TypedSelf: ";
      break;
    case type_function_param:
      std::cerr << "FunctionParam: ";
      break;
    case type_function_param_pattern:
      std::cerr << "FunctionParamPattern: ";
      break;
    case type_where_clause_item:
      std::cerr << "WhereClauseItem: ";
      break;
    case type_statements:
      std::cerr << "Statements: ";
      break;
    default:
      std::cerr << "No matched type!\n";
      throw "";
  }
  if (ptr_ >= tokens_.size()) {
    std::cerr << "Unexpected ending!\n";
  } else {
    std::cerr << "line " << tokens_[ptr_].GetLine() << " column " << tokens_[ptr_].GetColumn() << ": " << info << "\n";
  }
  throw "";
}

void Node::Restore(const int size_before_try, const int ptr_before_try) {
  for (int i = size_before_try; i < children_.size(); ++i) {
    delete children_[i];
    children_[i] = nullptr;
  }
  children_.resize(size_before_try);
  type_.resize(size_before_try);
  ptr_ = ptr_before_try;
}

Crate::Crate(const std::vector<Token> &tokens, int &ptr): Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    while (ptr < tokens.size()) {
      AddChild(type_item);
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

Item::Item(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    std::string next_token = tokens[ptr].GetStr();
    if (next_token == "fn") {
      AddChild(type_function);
    } else if (next_token == "struct") {
      AddChild(type_struct);
    } else if (next_token == "enum") {
      AddChild(type_enumeration);
    } else if (next_token == "const") {
      if (ptr + 1 >= tokens.size()) {
        ThrowErr(type_item, "");
      }
      if (tokens[ptr + 1].GetStr() == "fn") {
        AddChild(type_function);
      } else {
        AddChild(type_constant_item);
      }
    } else if (next_token == "trait") {
      AddChild(type_trait);
    } else if (next_token == "impl") {
      AddChild(type_implementation);
    } else {
      ThrowErr(type_item, "Invalid keyword!");
    }
  } catch (...) {
    Restore(0, ptr);
    throw "";
  }
}
