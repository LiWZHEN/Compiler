#ifndef VISITOR_MANAGER_H
#define VISITOR_MANAGER_H

#include "scope.h"
#include "value_type.h"

class VisitorManager {
public:
  void VisitAll(Crate *&root);
private:
  SymbolVisitor symbol_visitor_;
  ValueTypeVisitor value_type_visitor_;
};

#endif //VISITOR_MANAGER_H