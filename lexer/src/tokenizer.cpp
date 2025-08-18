#include "tokenizer.h"

Tokenizer::Tokenizer(const std::string &str, std::vector<Token> &tokens) : str_(str), tokens_(tokens) {}

void Tokenizer::Tokenize() {
  RegexMatcher matcher(str_);
  while (ptr_ < str_.length()) {
    MatchResult next_match_result = matcher.GetNext();
    Token next_token = {next_match_result.type_, next_match_result.matched_str_, row_, col_};
    if (next_match_result.type_ != DEFAULT) {
      tokens_.push_back(next_token);
    }
    for (const char &c : next_match_result.matched_str_) {
      if (c == '\n') {
        ++row_;
        col_ = 1;
      } else {
        ++col_;
      }
    }
    ptr_ += static_cast<int>(next_match_result.matched_str_.length());
    matcher.SetStart(ptr_);
  }
}