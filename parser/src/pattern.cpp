#include "pattern.h"

Pattern::Pattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  const std::string next_token = tokens[ptr].GetStr();
  try {
    if (next_token == "_") {
      AddChild(type_wildcard_pattern);
    } else if (next_token == "&" || next_token == "&&") {
      AddChild(type_reference_pattern);
    } else if (next_token == "ref" || next_token == "mut") {
      AddChild(type_identifier_pattern);
    } else {
      try {
        AddChild(type_literal_pattern);
      } catch (...) {
        Restore(0, ptr_before_try);
        std::cerr << "Pattern: Successfully handle try-literal-pattern-failure.\n";
        try {
          AddChild(type_identifier_pattern);
        } catch (...) {
          Restore(0, ptr_before_try);
          std::cerr << "Pattern: Successfully handle try-identifier-pattern-failure.\n";
          try {
            AddChild(type_tuple_struct_pattern);
          } catch (...) {
            Restore(0, ptr_before_try);
            std::cerr << "Pattern: Successfully handle try-tuple-struct-pattern-failure.\n";
            AddChild(type_path_pattern);
          }
        }
      }
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

LiteralPattern::LiteralPattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    if (tokens[ptr].GetStr() == "-") {
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_literal_pattern, "");
      }
    }
    AddChild(type_literal_expression);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

IdentifierPattern::IdentifierPattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    if (tokens[ptr].GetStr() == "ref") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_identifier_pattern, "");
      }
    }
    if (tokens[ptr].GetStr() == "mut") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_identifier_pattern, "");
      }
    }
    AddChild(type_identifier);
    if (ptr < tokens.size() && tokens[ptr].GetStr() == "@") {
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_identifier_pattern, "");
      }
      AddChild(type_pattern);
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

WildcardPattern::WildcardPattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    if (tokens[ptr].GetStr() != "_") {
      ThrowErr(type_wildcard_pattern, "Expect \"_\".");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

ReferencePattern::ReferencePattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    if (tokens[ptr].GetStr() != "&" && tokens[ptr].GetStr() != "&&") {
      ThrowErr(type_reference_pattern, R"(Expect "&" or "&&".)");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_reference_pattern, "");
    }
    if (tokens[ptr].GetStr() == "mut") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_reference_pattern, "");
      }
    }
    AddChild(type_pattern);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

TupleStructPattern::TupleStructPattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    AddChild(type_path_in_expression);
    if (ptr >= tokens.size()) {
      ThrowErr(type_tuple_struct_pattern, "");
    }
    if (tokens[ptr].GetStr() != "(") {
      ThrowErr(type_tuple_struct_pattern, "Expect \"(\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_tuple_struct_pattern, "");
    }
    if (tokens[ptr].GetStr() != ")") {
      AddChild(type_tuple_struct_items);
    }
    if (tokens[ptr].GetStr() != ")") {
      ThrowErr(type_tuple_struct_pattern, "Expect \")\".");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

PathPattern::PathPattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    AddChild(type_path_in_expression);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

TupleStructItems::TupleStructItems(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    AddChild(type_pattern);
    while (ptr < tokens.size()) {
      if (tokens[ptr].GetStr() != ",") {
        return;
      }
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        return;
      }
      const int size_before_trying_pattern = static_cast<int>(children_.size()),
          ptr_before_trying_pattern = ptr;
      try {
        AddChild(type_pattern);
      } catch (...) {
        Restore(size_before_trying_pattern, ptr_before_trying_pattern);
        return;
      }
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}
