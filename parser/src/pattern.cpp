#include "pattern.h"
#include "scope.h"

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
        std::cerr << "Pattern: Successfully handle literal pattern trying failure.\n";
        try {
          AddChild(type_identifier_pattern);
        } catch (...) {
          Restore(0, ptr_before_try);
          std::cerr << "Pattern: Successfully handle identifier pattern trying failure.\n";
          AddChild(type_path_in_expression);
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
    // -?
    if (tokens[ptr].GetStr() == "-") {
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_literal_pattern, "");
      }
    }
    // LiteralExpression
    AddChild(type_literal_expression);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

IdentifierPattern::IdentifierPattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // ref?
    if (tokens[ptr].GetStr() == "ref") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_identifier_pattern, "");
      }
    }
    // mut?
    if (tokens[ptr].GetStr() == "mut") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_identifier_pattern, "");
      }
    }
    // Identifier
    AddChild(type_identifier);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

WildcardPattern::WildcardPattern(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {}

ReferencePattern::ReferencePattern(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // &|&&
    if (tokens[ptr].GetStr() != "&" && tokens[ptr].GetStr() != "&&") {
      ThrowErr(type_reference_pattern, R"(Expect "&" or "&&".)");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_reference_pattern, "");
    }
    // mut?
    if (tokens[ptr].GetStr() == "mut") {
      AddChild(type_keyword);
      if (ptr >= tokens.size()) {
        ThrowErr(type_reference_pattern, "");
      }
    }
    // PatternWithoutRange
    AddChild(type_pattern);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

std::string Pattern::GetNodeLabel() const {
  return "Pattern";
}

std::string LiteralPattern::GetNodeLabel() const {
  return "LiteralPattern";
}

std::string IdentifierPattern::GetNodeLabel() const {
  return "IdentifierPattern";
}

std::string WildcardPattern::GetNodeLabel() const {
  return "WildcardPattern: " + token_.GetStr();
}

std::string ReferencePattern::GetNodeLabel() const {
  return "ReferencePattern";
}

void Pattern::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void LiteralPattern::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void IdentifierPattern::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void WildcardPattern::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void ReferencePattern::Accept(Visitor *visitor) {
  visitor->Visit(this);
}