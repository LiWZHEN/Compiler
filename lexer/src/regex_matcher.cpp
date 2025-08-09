#include "regex_matcher.h"

RegexMatcher::RegexMatcher(const std::string &str) : str_(str) {}

void RegexMatcher::SetStart(int start) {
  start_ = start;
}

MatchResult RegexMatcher::MatchIdentifierOrKeyword() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_IDENTIFIER_OR_KEYWORD, boost::regex_constants::match_continuous)) {
    return {matches[0], IDENTIFIER_OR_KEYWORD};
  }
  return {};
}

MatchResult RegexMatcher::MatchCharLiteral() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_CHAR_LITERAL, boost::regex_constants::match_continuous)) {
    return {matches[0], CHAR_LITERAL};
  }
  return {};
}

MatchResult RegexMatcher::MatchStringLiteral() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_STRING_LITERAL, boost::regex_constants::match_continuous)) {
    return {matches[0], STRING_LITERAL};
  }
  return {};
}

MatchResult RegexMatcher::MatchRawStringLiteral() {
  int ptr = start_;
  if (str_[ptr] != 'r') {
    return {};
  }
  ++ptr;
  int num = 0;
  while (str_[ptr] == '#') {
    ++num;
    ++ptr;
  }
  if (str_[ptr] != '"') {
    return {};
  }
  ++ptr;
  while (ptr < str_.length()) {
    if (ptr + num < str_.length() && str_[ptr] == '"') {
      bool match = true;
      for (int i = 1; i <= num; ++i) {
        if (str_[ptr + i] != '#') {
          match = false;
          break;
        }
      }
      if (match) {
        ptr += num + 1;
        if (ptr >= str_.length() || !((str_[ptr] >= 'a' && str_[ptr] <= 'z') || (str_[ptr] >= 'A' && str_[ptr] <= 'Z'))) {
          return {str_.substr(start_, ptr - start_), RAW_STRING_LITERAL};
        }
        ++ptr;
        while (ptr < str_.length() && (str_[ptr] == '_' || (str_[ptr] >= 'a' && str_[ptr] <= 'z') || (str_[ptr] >= 'A' && str_[ptr] <= 'Z') || (str_[ptr] >= '0' && str_[ptr] <= '9'))) {
          ++ptr;
        }
        return {str_.substr(start_, ptr - start_), RAW_STRING_LITERAL};
      }
    }
    if (str_[ptr] == '\r') {
      return {};
    }
    ++ptr;
  }
  return {};
}

MatchResult RegexMatcher::MatchByteLiteral() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_BYTE_LITERAL, boost::regex_constants::match_continuous)) {
    return {matches[0], BYTE_LITERAL};
  }
  return {};
}

MatchResult RegexMatcher::MatchByteStringLiteral() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_BYTE_STRING_LITERAL, boost::regex_constants::match_continuous)) {
    return {matches[0], BYTE_STRING_LITERAL};
  }
  return {};
}

MatchResult RegexMatcher::MatchRawByteStringLiteral() {
  int ptr = start_;
  if (ptr + 1 >= str_.length() || str_[ptr] != 'b' || str_[ptr + 1] != 'r') {
    return {};
  }
  ptr += 2;
  int num = 0;
  while (str_[ptr] == '#') {
    ++num;
    ++ptr;
  }
  if (str_[ptr] != '"') {
    return {};
  }
  ++ptr;
  while (ptr < str_.length()) {
    if (ptr + num < str_.length() && str_[ptr] == '"') {
      bool match = true;
      for (int i = 1; i <= num; ++i) {
        if (str_[ptr + i] != '#') {
          match = false;
          break;
        }
      }
      if (match) {
        ptr += num + 1;
        if (ptr >= str_.length() || !((str_[ptr] >= 'a' && str_[ptr] <= 'z') || (str_[ptr] >= 'A' && str_[ptr] <= 'Z'))) {
          return {str_.substr(start_, ptr - start_), RAW_BYTE_STRING_LITERAL};
        }
        ++ptr;
        while (ptr < str_.length() && (str_[ptr] == '_' || (str_[ptr] >= 'a' && str_[ptr] <= 'z') || (str_[ptr] >= 'A' && str_[ptr] <= 'Z') || (str_[ptr] >= '0' && str_[ptr] <= '9'))) {
          ++ptr;
        }
        return {str_.substr(start_, ptr - start_), RAW_BYTE_STRING_LITERAL};
      }
    }
    if (str_[ptr] == '\r' || str_[ptr] < '\x00' || str_[ptr] > '\x7F') {
      return {};
    }
    ++ptr;
  }
  return {};
}

MatchResult RegexMatcher::MatchCStringLiteral() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_C_STRING_LITERAL, boost::regex_constants::match_continuous)) {
    return {matches[0], C_STRING_LITERAL};
  }
  return {};
}

MatchResult RegexMatcher::MatchRawCStringLiteral() {
  int ptr = start_;
  if (ptr + 1 >= str_.length() || str_[ptr] != 'c' || str_[ptr + 1] != 'r') {
    return {};
  }
  ptr += 2;
  int num = 0;
  while (str_[ptr] == '#') {
    ++num;
    ++ptr;
  }
  if (str_[ptr] != '"') {
    return {};
  }
  ++ptr;
  while (ptr < str_.length()) {
    if (ptr + num < str_.length() && str_[ptr] == '"') {
      bool match = true;
      for (int i = 1; i <= num; ++i) {
        if (str_[ptr + i] != '#') {
          match = false;
          break;
        }
      }
      if (match) {
        ptr += num + 1;
        if (ptr >= str_.length() || !((str_[ptr] >= 'a' && str_[ptr] <= 'z') || (str_[ptr] >= 'A' && str_[ptr] <= 'Z'))) {
          return {str_.substr(start_, ptr - start_), RAW_C_STRING_LITERAL};
        }
        ++ptr;
        while (ptr < str_.length() && (str_[ptr] == '_' || (str_[ptr] >= 'a' && str_[ptr] <= 'z') || (str_[ptr] >= 'A' && str_[ptr] <= 'Z') || (str_[ptr] >= '0' && str_[ptr] <= '9'))) {
          ++ptr;
        }
        return {str_.substr(start_, ptr - start_), RAW_C_STRING_LITERAL};
      }
    }
    if (str_[ptr] == '\r' || str_[ptr] == '\x00') {
      return {};
    }
    ++ptr;
  }
  return {};
}

MatchResult RegexMatcher::MatchIntegerLiteral() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_INTEGER_LITERAL, boost::regex_constants::match_continuous)) {
    return {matches[0], INTEGER_LITERAL};
  }
  return {};
}

MatchResult RegexMatcher::MatchFloatLiteral() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_FLOAT_LITERAL, boost::regex_constants::match_continuous)) {
    return {matches[0], FLOAT_LITERAL};
  }
  return {};
}

MatchResult RegexMatcher::MatchPunctuation() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_PUNCTUATION, boost::regex_constants::match_continuous)) {
    return {matches[0], PUNCTUATION};
  }
  return {};
}

MatchResult RegexMatcher::MatchReservedToken() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_RESERVED_TOKEN, boost::regex_constants::match_continuous)) {
    return {matches[0], RESERVED_TOKEN};
  }
  return {};
}

MatchResult RegexMatcher::MatchLineComments() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_LINE_COMMENTS, boost::regex_constants::match_continuous)) {
    return {matches[0], DEFAULT};
  }
  return {};
}

MatchResult RegexMatcher::MatchBlockComments() {
  int ptr = start_;
  if (!(ptr + 1 < str_.length() && str_[ptr] == '/' && str_[ptr + 1] == '*')) {
    return {};
  }
  int depth = 1;
  ptr += 2;
  while (ptr < str_.length()) {
    if (ptr + 1 < str_.length() && str_[ptr] == '/' && str_[ptr + 1] == '*') {
      ptr += 2;
      ++depth;
    } else if (ptr + 1 < str_.length() && str_[ptr] == '*' && str_[ptr + 1] == '/') {
      --depth;
      ptr += 2;
      if (depth == 0) {
        return {str_.substr(start_, ptr - start_), DEFAULT};
      }
    } else {
      ++ptr;
    }
  }
  return {};
}

MatchResult RegexMatcher::MatchWhitespace() {
  boost::smatch matches;
  if (boost::regex_search(str_.begin() + start_, str_.end(), matches, REGEX_WHITESPACE, boost::regex_constants::match_continuous)) {
    return {matches[0], DEFAULT};
  }
  return {};
}

MatchResult RegexMatcher::GetNext() {
  MatchResult result = {};
  MatchResult tmp = MatchIdentifierOrKeyword();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchCharLiteral();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchStringLiteral();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchRawStringLiteral();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchByteLiteral();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchByteStringLiteral();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchRawByteStringLiteral();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchCStringLiteral();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchRawCStringLiteral();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchIntegerLiteral();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchFloatLiteral();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchPunctuation();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchReservedToken();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchWhitespace();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchLineComments();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  tmp = MatchBlockComments();
  if (tmp.matched_str_.length() > result.matched_str_.length()) {
    result = tmp;
  }
  if (result.matched_str_.empty()) {
    throw "No matched type!";
  }
  return result;
}
