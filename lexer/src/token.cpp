#include "token.h"
#include <iomanip>

Token::Token(type type, const std::string &str, int row, int column) {
  type_ = type;
  str_ = str;
  row_ = row;
  column_ = column;
}

const std::string &Token::GetStr() const {
  return str_;
}

type Token::GetType() const {
  return type_;
}

int Token::GetLine() const {
  return row_;
}

int Token::GetColumn() const {
  return column_;
}

long long Token::GetInt() const {
  long long value = 0;
  if (str_.length() >= 3) {
    if (str_[0] == '0') {
      if (str_[1] == 'b') { // bin
        for (int i = 2; i < str_.length() - this->GetIntType().length(); ++i) {
          if (str_[i] == '_') {
            continue;
          }
          value <<= 1;
          value |= str_[i] - '0';
        }
      } else if (str_[1] == 'o') { // oct
        for (int i = 2; i < str_.length() - this->GetIntType().length(); ++i) {
          if (str_[i] == '_') {
            continue;
          }
          value <<= 3;
          value += str_[i] - '0';
        }
      } else if (str_[1] == 'x') { // hex
        for (int i = 2; i < str_.length() - this->GetIntType().length(); ++i) {
          if (str_[i] == '_') {
            continue;
          }
          value <<= 4;
          if (str_[i] >= '0' && str_[i] <= '9') {
            value += str_[i] - '0';
          } else if (str_[i] >= 'a' && str_[i] <= 'f') { // a ~ f
            value += str_[i] - 'a' + 10;
          } else { // A ~ F
            value += str_[i] - 'A' + 10;
          }
        }
      } else { // dec
        for (int i = 0; i < str_.length() - this->GetIntType().length(); ++i) {
          if (str_[i] == '_') {
            continue;
          }
          value *= 10;
          value += str_[i] - '0';
        }
      }
    } else { // dec
      for (int i = 0; i < str_.length() - this->GetIntType().length(); ++i) {
        if (str_[i] == '_') {
          continue;
        }
        value *= 10;
        value += str_[i] - '0';
      }
    }
  } else {
    for (int i = 0; i < str_.length(); ++i) {
      if (str_[i] == '_') {
        continue;
      }
      value *= 10;
      value += str_[i] - '0';
    }
  }
  return value;
}

std::string Token::GetStringContent() const {
  switch (type_) {
    case STRING_LITERAL: {
      return str_.substr(1, str_.length() - 2);
    }
    case RAW_STRING_LITERAL: {
      int begin = 1, end = static_cast<int>(str_.length()) - 1;
      while (str_[begin] == '#' && str_[end] == '#') {
        ++begin;
        --end;
      }
      ++begin;
      return str_.substr(begin, end - begin);
    }
    case C_STRING_LITERAL: {
      return str_.substr(2, str_.length() - 3);
    }
    case RAW_C_STRING_LITERAL: {
      int begin = 2, end = static_cast<int>(str_.length()) - 1;
      while (str_[begin] == '#' && str_[end] == '#') {
        ++begin;
        --end;
      }
      ++begin;
      return str_.substr(begin, end - begin);
    }
    default: {
      return "";
    }
  }
}

std::string Token::GetCharContent() const {
  return str_.substr(1, str_.length() - 2);
}

std::string Token::GetIntType() const {
  if (str_.substr(str_.length() - 3, 3) == "i32") {
    return "i32";
  }
  if (str_.substr(str_.length() - 3, 3) == "u32") {
    return "u32";
  }
  if (str_.substr(str_.length() - 5, 5) == "isize") {
    return "isize";
  }
  if (str_.substr(str_.length() - 5, 5) == "usize") {
    return "usize";
  }
  return "";
}

std::ostream &operator<<(std::ostream &output, const Token &token) {
  std::string type_str;
  switch (token.type_) {
  case IDENTIFIER_OR_KEYWORD:
    type_str = "identifier or keyword";
    break;
  case CHAR_LITERAL:
    type_str = "char literal";
    break;
  case STRING_LITERAL:
    type_str = "string literal";
    break;
  case RAW_STRING_LITERAL:
    type_str = "raw string literal";
    break;
  /*case BYTE_LITERAL:
    type_str = "byte literal";
    break;
  case BYTE_STRING_LITERAL:
    type_str = "byte string literal";
    break;
  case RAW_BYTE_STRING_LITERAL:
    type_str = "raw byte string literal";
    break;*/
  case C_STRING_LITERAL:
    type_str = "c string literal";
    break;
  case RAW_C_STRING_LITERAL:
    type_str = "raw c string literal";
    break;
  case INTEGER_LITERAL:
    type_str = "integer literal";
    break;
  case FLOAT_LITERAL:
    type_str = "float literal";
    break;
  case PUNCTUATION:
    type_str = "punctuation";
    break;
  case RESERVED_TOKEN:
    type_str = "reserved token";
    break;
  default:
    type_str = "default";
    break;
  }
  if (token.type_ != DEFAULT)
  output << std::setw(25) << type_str << ": " << token.str_ << " at ln " << token.row_ << ", col " << token.column_;
  return output;
}