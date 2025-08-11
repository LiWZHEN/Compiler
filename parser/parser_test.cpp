#include <iostream>
#include "tokenizer.h"
#include "classes.h"
#include "builder.h"

int main() {
  std::vector<Token> tokens;
  std::string str;
  char c;
  while (std::cin.get(c)) {
    str += c;
  }
  Tokenizer tokenizer(str, tokens);
  tokenizer.Tokenize();
  Builder builder(tokens);
  Node *syntax_tree = builder.GetTree();
  return 0;
}