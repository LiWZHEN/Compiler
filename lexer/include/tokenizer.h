#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
#include "token.h"
#include "regex_matcher.h"

class Tokenizer {
public:
  Tokenizer() = delete;
  Tokenizer(const std::string &str, std::vector<Token> &tokens);
  void Tokenize();
private:
  const std::string &str_;
  std::vector<Token> &tokens_;
  int ptr_ = 0;
  int row_ = 1;
  int col_ = 1;
};

#endif