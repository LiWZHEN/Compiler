#include "builder.h"

Builder::Builder(const std::vector<Token> &tokens) : tokens_(tokens) {}

Node *Builder::GetTree() {
  if (tokens_.empty()) {
    return nullptr;
  }
  Node *root = nullptr;
  try {
    root = new Crate(tokens_, ptr_);
  } catch (...) {
    delete root;
    root = nullptr;
  }
  return root;
}
