#include "module.h"

Keyword::Keyword(const Token &token, int &ptr) : token_(token) {
  ++ptr;
}

Identifier::Identifier(const Token &token, int &ptr): token_(token) {
  ++ptr;
}

Punctuation::Punctuation(const Token &token, int &ptr) : token_(token) {
  ++ptr;
}

