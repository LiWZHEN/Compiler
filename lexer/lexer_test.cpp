#include <iostream>
#include "tokenizer.h"

int main() {
  std::vector<Token> tokens;
  std::string str;
  char c;
  while (std::cin.get(c)) {
    str += c;
  }
  Tokenizer tokenizer(str, tokens);
  tokenizer.Tokenize();
  for (const auto &token : tokens) {
    std::cout << token << '\n';
  }
  return 0;
}