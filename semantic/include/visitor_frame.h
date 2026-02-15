#ifndef VISITOR_FRAME_H
#define VISITOR_FRAME_H

#include <unordered_map>
#include <string>
#include "node.h"

struct ScopeNodeContent {
  Node *node;
  NodeType node_type;
};

struct ScopeNode {
  std::shared_ptr<ScopeNode> parent;
  std::unordered_map<std::string, ScopeNodeContent> type_namespace;
  std::unordered_map<std::string, ScopeNodeContent> value_namespace;
  explicit ScopeNode(const std::shared_ptr<ScopeNode> &parent) : parent(parent) {
    type_namespace["bool"] = {nullptr, type_type};
    type_namespace["char"] = {nullptr, type_type};
    type_namespace["str"] = {nullptr, type_type};
    type_namespace["i32"] = {nullptr, type_type};
    type_namespace["u32"] = {nullptr, type_type};
    type_namespace["usize"] = {nullptr, type_type};
    type_namespace["isize"] = {nullptr, type_type};
    type_namespace["String"] = {nullptr, type_type};
    value_namespace["print"] = {nullptr, type_function};
    value_namespace["println"] = {nullptr, type_function};
    value_namespace["printInt"] = {nullptr, type_function};
    value_namespace["printlnInt"] = {nullptr, type_function};
    value_namespace["getString"] = {nullptr, type_function};
    value_namespace["getInt"] = {nullptr, type_function};
    value_namespace["exit"] = {nullptr, type_function};
  }
  void TypeAdd(const std::string &name, const ScopeNodeContent scope_node_content) {
    if (type_namespace.contains(name)) {
      std::cerr << "The name \"" << name << "\" is already in the type namespace.\n";
      throw "";
    }
    type_namespace[name] = scope_node_content;
  }
  void ValueAdd(const std::string &name, const ScopeNodeContent scope_node_content) {
    if (value_namespace.contains(name)) {
      std::cerr << "The name \"" << name << "\" is already in the value namespace.\n";
      throw "";
    }
    value_namespace[name] = scope_node_content;
  }
  ScopeNodeContent FindInType(const std::string &name) {
    auto finding = this;
    while (finding != nullptr) {
      if (finding->type_namespace.contains(name)) {
        return finding->type_namespace[name];
      }
      finding = finding->parent.get();
    }
    return {nullptr, type_crate};
  }
  ScopeNodeContent FindInValue(const std::string &name) {
    auto finding = this;
    while (finding != nullptr) {
      if (finding->value_namespace.contains(name)) {
        return finding->value_namespace[name];
      }
      finding = finding->parent.get();
    }
    return {nullptr, type_crate};
  }
};

class Visitor {
public:
  virtual ~Visitor() = default;
  virtual void Visit(Crate *crate_ptr) = 0;
  virtual void Visit(Item *item_ptr) = 0;
  virtual void Visit(Function *function_ptr) = 0;
  virtual void Visit(Struct *struct_ptr) = 0;
  virtual void Visit(Enumeration *enumeration_ptr) = 0;
  virtual void Visit(ConstantItem *constant_item_ptr) = 0;
  virtual void Visit(Trait *trait_ptr) = 0;
  virtual void Visit(Implementation *implementation_ptr) = 0;
  virtual void Visit(Keyword *keyword_ptr) = 0;
  virtual void Visit(Identifier *identifier_ptr) = 0;
  virtual void Visit(Punctuation *punctuation_ptr) = 0;
  virtual void Visit(FunctionParameters *function_parameters_ptr) = 0;
  virtual void Visit(FunctionReturnType *function_return_type_ptr) = 0;
  virtual void Visit(BlockExpression *block_expression_ptr) = 0;
  virtual void Visit(SelfParam *self_param_ptr) = 0;
  virtual void Visit(FunctionParam *function_param_ptr) = 0;
  virtual void Visit(ShorthandSelf *shorthand_self_ptr) = 0;
  virtual void Visit(Type *type_ptr) = 0;
  virtual void Visit(Pattern *pattern_ptr) = 0;
  virtual void Visit(ReferencePattern *reference_pattern_ptr) = 0;
  virtual void Visit(IdentifierPattern *identifier_pattern_ptr) = 0;
  virtual void Visit(PathInExpression *path_in_expression_ptr) = 0;
  virtual void Visit(PathExprSegment *path_expr_segment_ptr) = 0;
  virtual void Visit(ReferenceType *reference_type_ptr) = 0;
  virtual void Visit(ArrayType *array_type_ptr) = 0;
  virtual void Visit(TypePath *type_path_ptr) = 0;
  virtual void Visit(UnitType *unit_type_ptr) = 0;
  virtual void Visit(Expression *expression_ptr) = 0;
  virtual void Visit(Statements *statements_ptr) = 0;
  virtual void Visit(Statement *statement_ptr) = 0;
  virtual void Visit(LetStatement *let_statement_ptr) = 0;
  virtual void Visit(ExpressionStatement *expression_statement_ptr) = 0;
  virtual void Visit(StructExprFields *struct_expr_fields_ptr) = 0;
  virtual void Visit(StructExprField *struct_expr_field_ptr) = 0;
  virtual void Visit(CharLiteral *char_literal_ptr) = 0;
  virtual void Visit(StringLiteral *string_literal_ptr) = 0;
  virtual void Visit(RawStringLiteral *raw_string_literal_ptr) = 0;
  virtual void Visit(CStringLiteral *c_string_literal_ptr) = 0;
  virtual void Visit(RawCStringLiteral *raw_c_string_literal_ptr) = 0;
  virtual void Visit(IntegerLiteral *integer_literal_ptr) = 0;
  virtual void Visit(StructFields *struct_fields_ptr) = 0;
  virtual void Visit(StructField *struct_field_ptr) = 0;
  virtual void Visit(EnumVariants *enum_variants_ptr) = 0;
  virtual void Visit(AssociatedItem *associated_item_ptr) = 0;
};

#endif //VISITOR_FRAME_H