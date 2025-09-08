#include "node.h"
#include "item.h"
#include "leaf_node.h"
#include "functions.h"
#include "path.h"
#include "expression.h"
#include "statements.h"
#include "function_parameters.h"
#include "type.h"
#include "pattern.h"
#include "structs.h"
#include "enumerations.h"
#include "trait.h"
#include "scope.h"

Node::Node(const std::vector<Token> &tokens, int &ptr) : tokens_(tokens), ptr_(ptr) {}

Node::~Node() {
  for (auto &it : children_) {
    delete it;
    it = nullptr;
  }
  children_.resize(0);
  type_.resize(0);
}

int Node::GetChildrenNum() const {
  return static_cast<int>(children_.size());
}

std::vector<Node *> const &Node::GetChildrenPtr() const {
  return children_;
}

std::vector<NodeType> const &Node::GetChildrenType() const {
  return type_;
}

void Node::AddChild(NodeType node_type) {
  switch (node_type) {
    case type_item:
      children_.push_back(new Item(tokens_, ptr_));
      type_.push_back(type_item);
      break;
    case type_function:
      children_.push_back(new Function(tokens_, ptr_));
      type_.push_back(type_function);
      break;
    case type_struct:
      children_.push_back(new Struct(tokens_, ptr_));
      type_.push_back(type_struct);
      break;
    case type_enumeration:
      children_.push_back(new Enumeration(tokens_, ptr_));
      type_.push_back(type_enumeration);
      break;
    case type_constant_item:
      children_.push_back(new ConstantItem(tokens_, ptr_));
      type_.push_back(type_constant_item);
      break;
    case type_trait:
      children_.push_back(new Trait(tokens_, ptr_));
      type_.push_back(type_trait);
      break;
    case type_implementation:
      children_.push_back(new Implementation(tokens_, ptr_));
      type_.push_back(type_implementation);
      break;
    case type_keyword:
      children_.push_back(new Keyword(tokens_, ptr_));
      type_.push_back(type_keyword);
      break;
    case type_identifier:
      children_.push_back(new Identifier(tokens_, ptr_));
      type_.push_back(type_identifier);
      break;
    case type_punctuation:
      children_.push_back(new Punctuation(tokens_, ptr_));
      type_.push_back(type_punctuation);
      break;
    case type_function_parameters:
      children_.push_back(new FunctionParameters(tokens_, ptr_));
      type_.push_back(type_function_parameters);
      break;
    case type_function_return_type:
      children_.push_back(new FunctionReturnType(tokens_, ptr_));
      type_.push_back(type_function_return_type);
      break;
    case type_block_expression:
      children_.push_back(new BlockExpression(tokens_, ptr_));
      type_.push_back(type_block_expression);
      break;
    case type_self_param:
      children_.push_back(new SelfParam(tokens_, ptr_));
      type_.push_back(type_self_param);
      break;
    case type_function_param:
      children_.push_back(new FunctionParam(tokens_, ptr_));
      type_.push_back(type_function_param);
      break;
    case type_shorthand_self:
      children_.push_back(new ShorthandSelf(tokens_, ptr_));
      type_.push_back(type_shorthand_self);
      break;
    case type_typed_self:
      children_.push_back(new TypedSelf(tokens_, ptr_));
      type_.push_back(type_typed_self);
      break;
    case type_type:
      children_.push_back(new Type(tokens_, ptr_));
      type_.push_back(type_type);
      break;
    case type_pattern:
      children_.push_back(new Pattern(tokens_, ptr_));
      type_.push_back(type_pattern);
      break;
    case type_wildcard_pattern:
      children_.push_back(new WildcardPattern(tokens_, ptr_));
      type_.push_back(type_wildcard_pattern);
      break;
    case type_reference_pattern:
      children_.push_back(new ReferencePattern(tokens_, ptr_));
      type_.push_back(type_reference_pattern);
      break;
    case type_identifier_pattern:
      children_.push_back(new IdentifierPattern(tokens_, ptr_));
      type_.push_back(type_identifier_pattern);
      break;
    case type_literal_pattern:
      children_.push_back(new LiteralPattern(tokens_, ptr_));
      type_.push_back(type_literal_pattern);
      break;
    case type_path_in_expression:
      children_.push_back(new PathInExpression(tokens_, ptr_));
      type_.push_back(type_path_in_expression);
      break;
    case type_literal_expression:
      children_.push_back(new LiteralExpression(tokens_, ptr_));
      type_.push_back(type_literal_expression);
      break;
    case type_path_expr_segment:
      children_.push_back(new PathExprSegment(tokens_, ptr_));
      type_.push_back(type_path_expr_segment);
      break;
    case type_reference_type:
      children_.push_back(new ReferenceType(tokens_, ptr_));
      type_.push_back(type_reference_type);
      break;
    case type_array_type:
      children_.push_back(new ArrayType(tokens_, ptr_));
      type_.push_back(type_array_type);
      break;
    case type_type_path:
      children_.push_back(new TypePath(tokens_, ptr_));
      type_.push_back(type_type_path);
      break;
    case type_unit_type:
      children_.push_back(new UnitType(tokens_, ptr_));
      type_.push_back(type_unit_type);
      break;
    case type_expression:
      children_.push_back(new Expression(tokens_, ptr_, unknown, 0));
      type_.push_back(type_expression);
      break;
    case type_statements:
      children_.push_back(new Statements(tokens_, ptr_));
      type_.push_back(type_statements);
      break;
    case type_statement:
      children_.push_back(new Statement(tokens_, ptr_));
      type_.push_back(type_statement);
      break;
    case type_let_statement:
      children_.push_back(new LetStatement(tokens_, ptr_));
      type_.push_back(type_let_statement);
      break;
    case type_expression_statement:
      children_.push_back(new ExpressionStatement(tokens_, ptr_));
      type_.push_back(type_expression_statement);
      break;
    case type_struct_expr_fields:
      children_.push_back(new StructExprFields(tokens_, ptr_));
      type_.push_back(type_struct_expr_fields);
      break;
    case type_struct_expr_field:
      children_.push_back(new StructExprField(tokens_, ptr_));
      type_.push_back(type_struct_expr_field);
      break;
    case type_char_literal:
      children_.push_back(new CharLiteral(tokens_, ptr_));
      type_.push_back(type_char_literal);
      break;
    case type_string_literal:
      children_.push_back(new StringLiteral(tokens_, ptr_));
      type_.push_back(type_string_literal);
      break;
    case type_raw_string_literal:
      children_.push_back(new RawStringLiteral(tokens_, ptr_));
      type_.push_back(type_raw_string_literal);
      break;
    case type_c_string_literal:
      children_.push_back(new CStringLiteral(tokens_, ptr_));
      type_.push_back(type_c_string_literal);
      break;
    case type_raw_c_string_literal:
      children_.push_back(new RawCStringLiteral(tokens_, ptr_));
      type_.push_back(type_raw_c_string_literal);
      break;
    case type_integer_literal:
      children_.push_back(new IntegerLiteral(tokens_, ptr_));
      type_.push_back(type_integer_literal);
      break;
    case type_struct_fields:
      children_.push_back(new StructFields(tokens_, ptr_));
      type_.push_back(type_struct_fields);
      break;
    case type_struct_field:
      children_.push_back(new StructField(tokens_, ptr_));
      type_.push_back(type_struct_field);
      break;
    case type_enum_variants:
      children_.push_back(new EnumVariants(tokens_, ptr_));
      type_.push_back(type_enum_variants);
      break;
    case type_associated_item:
      children_.push_back(new AssociatedItem(tokens_, ptr_));
      type_.push_back(type_associated_item);
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
    case type_function_parameters:
      std::cerr << "FunctionParameters: ";
      break;
    case type_function_return_type:
      std::cerr << "ReturnType: ";
      break;
    case type_block_expression:
      std::cerr << "BlockExpression: ";
      break;
    case type_self_param:
      std::cerr << "SelfParam: ";
      break;
    case type_function_param:
      std::cerr << "FunctionParam: ";
      break;
    case type_shorthand_self:
      std::cerr << "ShorthandSelf: ";
      break;
    case type_typed_self:
      std::cerr << "TypedSelf: ";
      break;
    case type_type:
      std::cerr << "Type: ";
      break;
    case type_pattern:
      std::cerr << "Pattern: ";
      break;
    case type_wildcard_pattern:
      std::cerr << "WildcardPattern: ";
      break;
    case type_reference_pattern:
      std::cerr << "ReferencePattern: ";
      break;
    case type_identifier_pattern:
      std::cerr << "IdentifierPattern: ";
      break;
    case type_literal_pattern:
      std::cerr << "LiteralPattern: ";
      break;
    case type_path_in_expression:
      std::cerr << "PathInExpression: ";
      break;
    case type_literal_expression:
      std::cerr << "LiteralExpression: ";
      break;
    case type_path_expr_segment:
      std::cerr << "PathExprSegment: ";
      break;
    case type_reference_type:
      std::cerr << "ReferenceType: ";
      break;
    case type_array_type:
      std::cerr << "ArrayType: ";
      break;
    case type_type_path:
      std::cerr << "TypePath: ";
      break;
    case type_unit_type:
      std::cerr << "UnitType: ";
      break;
    case type_expression:
      std::cerr << "Expression: ";
      break;
    case type_statements:
      std::cerr << "Statements: ";
      break;
    case type_statement:
      std::cerr << "Statement: ";
      break;
    case type_let_statement:
      std::cerr << "LetStatement: ";
      break;
    case type_expression_statement:
      std::cerr << "ExpressionStatement: ";
      break;
    case type_struct_expr_fields:
      std::cerr << "StructExprFields: ";
      break;
    case type_struct_expr_field:
      std::cerr << "StructExprField: ";
      break;
    case type_char_literal:
      std::cerr << "CharLiteral: ";
      break;
    case type_string_literal:
      std::cerr << "StringLiteral: ";
      break;
    case type_raw_string_literal:
      std::cerr << "RawStringLiteral: ";
      break;
    case type_c_string_literal:
      std::cerr << "CStringLiteral: ";
      break;
    case type_raw_c_string_literal:
      std::cerr << "RawCStringLiteral: ";
      break;
    case type_integer_literal:
      std::cerr << "IntegerLiteral: ";
      break;
    case type_struct_fields:
      std::cerr << "StructFields: ";
      break;
    case type_struct_field:
      std::cerr << "StructField: ";
      break;
    case type_enum_variants:
      std::cerr << "EnumVariants: ";
      break;
    case type_associated_item:
      std::cerr << "AssociatedItem: ";
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

LeafNode::LeafNode(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr), token_(tokens[ptr]) {
  ++ptr;
}

Token const &LeafNode::GetContent() const {
  return token_;
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
    Restore(0, ptr_before_try);
    throw "";
  }
}

std::string Crate::GetNodeLabel() const {
  return "Crate";
}

std::string Item::GetNodeLabel() const {
  return "Item";
}

void Crate::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Item::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

// The following functions for printing is provided by deepseek
std::string Node::GetNodeLabel() const {
  return "Node";
}

std::string LeafNode::GetNodeLabel() const {
  return "Leaf: " + token_.GetStr();
}

std::string Node::GetStruct(const std::string& prefix, bool isLast) const {
  std::string result;
  result += prefix + (isLast ? "└── " : "├── ") + GetNodeLabel() + "\n"; // Show current node
  std::string childPrefix = prefix + (isLast ? "    " : "│   ");
  // Recursive processing
  for (size_t i = 0; i < children_.size(); ++i) {
    bool childIsLast = (i == children_.size() - 1);
    result += children_[i]->GetStruct(childPrefix, childIsLast);
  }
  return result;
}
