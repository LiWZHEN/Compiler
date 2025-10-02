#include "visitor_manager.h"

void VisitorManager::VisitAll(Crate *&root) {
  try {
    symbol_visitor_.Visit(root);
    value_type_visitor_.Visit(root);
  } catch (...) {
    delete root;
    root = nullptr;
  }
}