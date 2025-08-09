#ifndef REGEX_MATCHER_H
#define REGEX_MATCHER_H

#include <boost/regex.hpp>
#include "token.h"

struct MatchResult {
  std::string matched_str_;
  type type_ = DEFAULT;
};

const std::string STR_IDENTIFIER_OR_KEYWORD = R"([a-zA-Z][a-zA-Z0-9_]*)";
const std::string STR_WHITESPACE = R"([\s]*)";
const std::string STR_LINE_COMMENTS = R"(//[^\n]*)";
const std::string STR_CHAR_LITERAL = R"('([^'\\\n\r\t]|\\['"]|\\x[0-7][0-9a-fA-F]|\\[nrt\\0])'([a-zA-Z][a-zA-Z0-9_]*)?)";
const std::string STR_STRING_LITERAL = R"x("([^"\\\r]|\\['"]|\\x[0-7][0-9a-fA-F]|\\[nrt\\0]|\\(\r)?\n)*"([a-zA-Z][a-zA-Z0-9_]*)?)x";
const std::string STR_BYTE_LITERAL = R"(b'([\x00-\x7F&&[^'\\\n\r\t]]|\\x[0-7][0-9a-fA-F]|\\[nrt\\0'"])'([a-zA-Z][a-zA-Z0-9_]*)?)";
const std::string STR_BYTE_STRING_LITERAL = R"(b"([\x00-\x7F&&[^"\\\r]]|\\x[0-7][0-9a-fA-F]|\\[nrt\\0'"]|\\(\r)?\n)*"([a-zA-Z][a-zA-Z0-9_]*)?)";
const std::string STR_C_STRING_LITERAL = R"(c"([^"\\\r\x00]|(\\x(?!00)[0-7][0-9a-fA-F]|\\[nrt\\'"])|\\(\r)?\n)*"([a-zA-Z][a-zA-Z0-9_]*)?)";
const std::string STR_DEC_LITERAL = R"([0-9]([0-9_])*)";
const std::string STR_BIN_LITERAL = R"(0b([0-1_])*[0-1]([0-1_])*)";
const std::string STR_OCT_LITERAL = R"(0o([0-7_])*[0-7]([0-7_])*)";
const std::string STR_HEX_LITERAL = R"(0x([0-9a-fA-F_])*[0-9a-fA-F]([0-9a-fA-F_])*)";
const std::string STR_INTEGER_LITERAL = R"((([0-9]([0-9_])*)|(0b([0-1_])*[0-1]([0-1_])*)|(0o([0-7_])*[0-7]([0-7_])*)|(0x([0-9a-fA-F_])*[0-9a-fA-F]([0-9a-fA-F_])*))([a-df-zA-DF-Z][a-zA-Z0-9_]*)?)";
const std::string STR_FLOAT_LITERAL = R"((([0-9]([0-9_])*)\.[0-9]([0-9_])*([a-df-zA-DF-Z][a-zA-Z0-9_]*)?)|(([0-9]([0-9_])*)\.(?![._a-zA-Z])))";
const std::string STR_PUNCTUATION = R"(==|=>|=|<<=|<<|<=|<-|<|!=|!|>>=|>>|>=|>|&&|&=|&|\|\||\|=|\||~|\+=|\+|->|-=|-|\*=|\*|/=|/|%=|%|\^=|\^|@|\.\.\.|\.\.=|\.\.|\.|,|;|::|:|#|\$|\?|_|\{|\}|\[|\]|\(|\))";
const std::string STR_RESERVED_TOKEN = R"((#+"([^"\\\r]|\\['"]|\\x[0-7][0-9a-fA-F]|\\[nrt\\0]|\\(\r)?\n)*"([a-zA-Z][a-zA-Z0-9_]*)?)|(0b([0-1_])*[0-1]([0-1_])*[2-9])|(0o([0-7_])*[0-7]([0-7_])*[8-9])|(((0b([0-1_])*[0-1]([0-1_])*)|(0o([0-7_])*[0-7]([0-7_])*)|(0x([0-9a-fA-F_])*[0-9a-fA-F]([0-9a-fA-F_])*))\.(?![._a-zA-Z]))|(0b_*(?![0-1]))|(0o_*(?![0-7]))|(0x_*(?![0-9a-zA_Z])))";

const boost::regex REGEX_IDENTIFIER_OR_KEYWORD(STR_IDENTIFIER_OR_KEYWORD);
const boost::regex REGEX_WHITESPACE(STR_WHITESPACE);
const boost::regex REGEX_LINE_COMMENTS(STR_LINE_COMMENTS);
const boost::regex REGEX_CHAR_LITERAL(STR_CHAR_LITERAL);
const boost::regex REGEX_STRING_LITERAL(STR_STRING_LITERAL);
const boost::regex REGEX_BYTE_LITERAL(STR_BYTE_LITERAL);
const boost::regex REGEX_BYTE_STRING_LITERAL(STR_BYTE_STRING_LITERAL);
const boost::regex REGEX_C_STRING_LITERAL(STR_C_STRING_LITERAL);
const boost::regex REGEX_INTEGER_LITERAL(STR_INTEGER_LITERAL);
const boost::regex REGEX_FLOAT_LITERAL(STR_FLOAT_LITERAL);
const boost::regex REGEX_PUNCTUATION(STR_PUNCTUATION);
const boost::regex REGEX_RESERVED_TOKEN(STR_RESERVED_TOKEN);

class RegexMatcher {
public:
  RegexMatcher() = delete;
  RegexMatcher(const std::string &str);
  void SetStart(int start);
  MatchResult GetNext();

private:
  const std::string &str_;
  int start_ = 0;

  MatchResult MatchIdentifierOrKeyword();
  MatchResult MatchCharLiteral();
  MatchResult MatchStringLiteral();
  MatchResult MatchRawStringLiteral();
  MatchResult MatchByteLiteral();
  MatchResult MatchByteStringLiteral();
  MatchResult MatchRawByteStringLiteral();
  MatchResult MatchCStringLiteral();
  MatchResult MatchRawCStringLiteral();
  MatchResult MatchIntegerLiteral();
  MatchResult MatchFloatLiteral();
  MatchResult MatchPunctuation();
  MatchResult MatchReservedToken();
  MatchResult MatchLineComments();
  MatchResult MatchBlockComments();
  MatchResult MatchWhitespace();
};

#endif