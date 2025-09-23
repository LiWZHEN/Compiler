#include "statements.h"
#include "expression.h"
#include "scope.h"

Statements::Statements(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    bool has_statement = false;
    while (ptr_ < tokens_.size()) {
      const int ptr_before_trying_statement = ptr_;
      const int size_before_trying_statement = static_cast<int>(children_.size());
      try {
        AddChild(type_statement);
        has_statement = true;
      } catch (...) {
        Restore(size_before_trying_statement, ptr_before_trying_statement);
        break;
      }
    }
    if (ptr_ >= tokens_.size()) {
      if (!has_statement) {
        ThrowErr(type_statements, "Expect at least a statement or an expression without block.");
      }
      return;
    }
    const int ptr_before_trying_expression_without_block = ptr_;
    const int size_before_trying_expression_without_block = static_cast<int>(children_.size());
    try {
      AddChild(type_expression);
      const auto expr_ptr = reinterpret_cast<Expression *>(children_.back());
      ExprType expr_type = expr_ptr->GetExprType();
      if (expr_type == block_expr || expr_type == const_block_expr || expr_type == infinite_loop_expr
          || expr_type == predicate_loop_expr || expr_type == if_expr) {
        // ExpressionWithBlock
        ThrowErr(type_statements, "Expect expression without block.");
      }
    } catch (...) {
      Restore(size_before_trying_expression_without_block, ptr_before_trying_expression_without_block);
      std::cerr << "Statements: Successfully handle the expression without block try failure.\n";
      if (!has_statement) {
        ThrowErr(type_statements, "Expect at least a statement or an expression without block.");
      }
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw;
  }
}

Statement::Statement(const std::vector<Token> &tokens, int &ptr) : Node (tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    std::string next_token = tokens_[ptr_].GetStr();
    if (next_token == ";") {
      AddChild(type_punctuation);
    } else if (next_token == "let") {
      AddChild(type_let_statement);
    } else {
      try {
        AddChild(type_item);
      } catch (...) {
        Restore(0, ptr_before_try);
        std::cerr << "Statement: Successfully handle the item try failure.\n";
        AddChild(type_expression_statement);
      }
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw;
  }
}

LetStatement::LetStatement(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // let
    if (tokens_[ptr_].GetStr() != "let") {
      ThrowErr(type_let_statement, "Expect \"let\".");
    }
    AddChild(type_keyword);
    // PatternNoTopAlt
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_let_statement, "");
    }
    AddChild(type_pattern);
    // :
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_let_statement, "");
    }
    if (tokens_[ptr_].GetStr() != ":") {
      ThrowErr(type_let_statement, "Expect \":\".");
    }
    AddChild(type_punctuation);
    // Type
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_let_statement, "");
    }
    AddChild(type_type);
    // = Expression ;
    // =
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_let_statement, "");
    }
    if (tokens_[ptr_].GetStr() != "=") {
      ThrowErr(type_let_statement, "Expect \"=\".");
    }
    AddChild(type_punctuation);
    // Expression
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_let_statement, "");
    }
    AddChild(type_expression);
    // ;
    if (tokens_[ptr_].GetStr() != ";") {
      ThrowErr(type_let_statement, "Expect \";\".");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw;
  }
}

ExpressionStatement::ExpressionStatement(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    AddChild(type_expression);
    auto expr_ptr = reinterpret_cast<Expression *>(children_.back());
    ExprType expr_type = expr_ptr->GetExprType();
    if (expr_type == block_expr || expr_type == const_block_expr || expr_type == infinite_loop_expr
        || expr_type == predicate_loop_expr || expr_type == if_expr) {
      // ExpressionWithBlock
      // ;?
      if (ptr_ < tokens_.size() && tokens_[ptr_].GetStr() == ";") {
        AddChild(type_punctuation);
      }
    } else {
      // ExpressionWithoutBlock
      // ;
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression_statement, "");
      }
      if (tokens_[ptr_].GetStr() != ";") {
        ThrowErr(type_expression_statement, "Expect \";\".");
      }
      AddChild(type_punctuation);
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw;
  }
}

std::string Statements::GetNodeLabel() const {
  return "Statements";
}

std::string Statement::GetNodeLabel() const {
  return "Statement";
}

std::string LetStatement::GetNodeLabel() const {
  return "LetStatement";
}

std::string ExpressionStatement::GetNodeLabel() const {
  return "ExpressionStatement";
}

void Statements::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Statement::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void LetStatement::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void ExpressionStatement::Accept(Visitor *visitor) {
  visitor->Visit(this);
}