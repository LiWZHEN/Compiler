#include "statements.h"

Statements::Statements(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {

  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}
