#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

#include "classes.h"
#include "node.h"

class EnumVariants final : public Node {
public:
  EnumVariants(const std::vector<Token> &tokens, int &ptr);
  [[nodiscard]] std::string GetNodeLabel() const override;
  void AddSymbol(ScopeNode *target_scope, bool need_type_add, bool need_value_add, bool associated_item_add,
      bool field_item_add, ScopeNodeContent target_node, ScopeNodeContent node_info) override;
private:
  void Accept(Visitor *visitor) override;
};

#endif