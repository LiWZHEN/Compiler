#include "expression.h"

double GetBP(Infix op, int side) {
  if (side == 0) {
    switch (op) {
      case brackets:
      case small_brackets:
        return 12.1;
      case dot:
        return 13.0;
      case add:
      case minus:
        return 9.0;
      case multiply:
      case divide:
      case mod:
        return 10.0;
      case bitwise_and:
        return 7.0;
      case bitwise_or:
        return 5.0;
      case bitwise_xor:
        return 6.0;
      case left_shift:
      case right_shift:
        return 8.0;
      case is_equal:
      case is_not_equal:
      case is_bigger:
      case is_smaller:
      case is_not_smaller:
      case is_not_bigger:
        return 4.0;
      case logic_or:
        return 2.0;
      case logic_and:
        return 3.0;
      case type_cast:
        return 11.0;
      case assign:
      case add_assign:
      case minus_assign:
      case multiply_assign:
      case divide_assign:
      case mod_assign:
      case bitwise_and_assign:
      case bitwise_or_assign:
      case bitwise_xor_assign:
      case left_shift_assign:
      case right_shift_assign:
        return 1.1;
      default:
        throw "";
    }
  }
  switch (op) {
    case brackets:
    case small_brackets:
      return 12.0;
    case dot:
      return 13.1;
    case add:
    case minus:
      return 9.1;
    case multiply:
    case divide:
    case mod:
      return 10.1;
    case bitwise_and:
      return 7.1;
    case bitwise_or:
      return 5.1;
    case bitwise_xor:
      return 6.1;
    case left_shift:
    case right_shift:
      return 8.1;
    case is_equal:
    case is_not_equal:
    case is_bigger:
    case is_smaller:
    case is_not_smaller:
    case is_not_bigger:
      return 4.1;
    case logic_or:
      return 2.1;
    case logic_and:
      return 3.1;
    case type_cast:
      return 11.1;
    case assign:
    case add_assign:
    case minus_assign:
    case multiply_assign:
    case divide_assign:
    case mod_assign:
    case bitwise_and_assign:
    case bitwise_or_assign:
    case bitwise_xor_assign:
    case left_shift_assign:
    case right_shift_assign:
      return 1.0;
    default:
      throw "";
  }
}

Infix GetInfix(const std::string &op) {
  if (op == "[") {
    return brackets;
  }
  if (op == "(") {
    return small_brackets;
  }
  if (op == ".") {
    return dot;
  }
  if (op == "+") {
    return add;
  }
  if (op == "-") {
    return minus;
  }
  if (op == "*") {
    return multiply;
  }
  if (op == "/") {
    return divide;
  }
  if (op == "%") {
    return mod;
  }
  if (op == "&") {
    return bitwise_and;
  }
  if (op == "|") {
    return bitwise_or;
  }
  if (op == "^") {
    return bitwise_xor;
  }
  if (op == "<<") {
    return left_shift;
  }
  if (op == ">>") {
    return right_shift;
  }
  if (op == "==") {
    return is_equal;
  }
  if (op == "!=") {
    return is_not_equal;
  }
  if (op == ">") {
    return is_bigger;
  }
  if (op == "<") {
    return is_smaller;
  }
  if (op == ">=") {
    return is_not_smaller;
  }
  if (op == "<=") {
    return is_not_bigger;
  }
  if (op == "||") {
    return logic_or;
  }
  if (op == "&&") {
    return logic_and;
  }
  if (op == "as") {
    return type_cast;
  }
  if (op == "=") {
    return assign;
  }
  if (op == "+=") {
    return add_assign;
  }
  if (op == "-=") {
    return minus_assign;
  }
  if (op == "*=") {
    return multiply_assign;
  }
  if (op == "/=") {
    return divide_assign;
  }
  if (op == "%=") {
    return mod_assign;
  }
  if (op == "&=") {
    return bitwise_and_assign;
  }
  if (op == "|=") {
    return bitwise_or_assign;
  }
  if (op == "^=") {
    return bitwise_xor_assign;
  }
  if (op == "<<=") {
    return left_shift_assign;
  }
  if (op == ">>=") {
    return right_shift_assign;
  }
  return not_infix;
}

LiteralExpression::LiteralExpression(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {
  const std::string next_token = token_.GetStr();
  const type next_type = token_.GetType();
  if (next_type != CHAR_LITERAL && next_type != STRING_LITERAL && next_type != RAW_STRING_LITERAL && next_type != C_STRING_LITERAL
      && next_type != RAW_C_STRING_LITERAL && next_type != INTEGER_LITERAL && next_token != "true" && next_token != "false") {
    --ptr_;
    ThrowErr(type_literal_expression, "Invalid literal expression.");
  }
}

ExprType Expression::GetNextExprType() const {
  const std::string next_token = tokens_[ptr_].GetStr();
  if (next_token == "{") {
    return  block_expr;
  }
  if (next_token == "const") {
    return const_block_expr;
  }
  if (next_token == "loop") {
    return infinite_loop_expr;
  }
  if (next_token == "while") {
    return predicate_loop_expr;
  }
  if (next_token == "if") {
    return if_expr;
  }
  if (next_token == "match") {
    return match_expr;
  }
  if (next_token == "(") {
    return grouped_expr;
  }
  if (next_token == "[") {
    return array_expr;
  }
  if (next_token == "continue") {
    return continue_expr;
  }
  if (next_token == "break") {
    return break_expr;
  }
  if (next_token == "return") {
    return return_expr;
  }
  if (next_token == "_") {
    return underscore_expr;
  }
  if (next_token == "true" || next_token == "false") {
    return literal_expr;
  }
  type next_token_type = tokens_[ptr_].GetType();
  if (next_token_type == CHAR_LITERAL || next_token_type == STRING_LITERAL || next_token_type == RAW_STRING_LITERAL
      || next_token_type == C_STRING_LITERAL || next_token_type == RAW_C_STRING_LITERAL || next_token_type == INTEGER_LITERAL) {
    return literal_expr;
  }
  return unknown;
}

Expression::Expression(Expression *lhs, Expression *rhs, Infix infix) : Node(tokens_, ptr_), expr_type_(unknown), infix_(infix) {
  children_.push_back(lhs);
  type_.push_back(type_expression);
  if (rhs != nullptr) {
    children_.push_back(rhs);
    type_.push_back(type_expression);
  }
}

Expression::Expression(const std::vector<Token> &tokens, int &ptr, ExprType expr_type, double min_bp) : Node(tokens, ptr), expr_type_(expr_type) {
  const int ptr_before_try = ptr_;
  Expression *lhs = nullptr, *rhs = nullptr;
  try {
    if (expr_type_ == block_expr) {
      // {
      if (tokens_[ptr_].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      // Statements?
      if (tokens_[ptr_].GetStr() != "}") {
        // Statements
        AddChild(type_statements);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
      }
      // }
      if (tokens_[ptr_].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type_ == const_block_expr) {
      // const
      if (tokens_[ptr_].GetStr() != "const") {
        ThrowErr(type_expression, "Expect \"const\".");
      }
      AddChild(type_keyword);
      // {
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      // Statements?
      if (tokens_[ptr_].GetStr() != "}") {
        // Statements
        AddChild(type_statements);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
      }
      // }
      if (tokens_[ptr_].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type_ == infinite_loop_expr) {
      // loop
      if (tokens_[ptr_].GetStr() != "loop") {
        ThrowErr(type_expression, "Expect \"loop\".");
      }
      AddChild(type_keyword);
      // {
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      // Statements?
      if (tokens_[ptr_].GetStr() != "}") {
        // Statements
        AddChild(type_statements);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
      }
      // }
      if (tokens_[ptr_].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type_ == predicate_loop_expr) {
      // while
      if (tokens_[ptr_].GetStr() != "while") {
        ThrowErr(type_expression, "Expect \"while\".");
      }
      AddChild(type_keyword);
      // Conditions
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      const int size_before_trying_let_chain = static_cast<int>(children_.size()),
          ptr_before_trying_let_chain = ptr_;
      try {
        // LetChain
        // LetChainCondition
        if (tokens_[ptr_].GetStr() == "let") {
          // let Pattern =
          // let
          AddChild(type_keyword);
          // Pattern
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
          AddChild(type_pattern);
          // =
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
          if (tokens_[ptr_].GetStr() != "=") {
            ThrowErr(type_expression, "Expect \"=\".");
          }
          AddChild(type_punctuation);
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
        }
        // Expression except ExcludedConditions
        AddChild(type_expression);
        ExprType new_expr_type = reinterpret_cast<Expression *>(children_.back())->expr_type_;
        if (new_expr_type == struct_expr || new_expr_type == lazy_boolean_expr
            || new_expr_type == assignment_expr || new_expr_type == compound_assignment_expr) {
          ThrowErr(type_expression, "Expression: Unexpected ExcludedConditions.");
        }
        // (&&LetChainCondition)*
        while (ptr_ >= tokens_.size() && tokens_[ptr_].GetStr() == "&&") {
          // &&LetChainCondition
          // &&
          AddChild(type_punctuation);
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
          // LetChainCondition
          if (tokens_[ptr_].GetStr() == "let") {
            // let Pattern =
            // let
            AddChild(type_keyword);
            // Pattern
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
            AddChild(type_pattern);
            // =
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
            if (tokens_[ptr_].GetStr() != "=") {
              ThrowErr(type_expression, "Expect \"=\".");
            }
            AddChild(type_punctuation);
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
          }
          // Expression except ExcludedConditions
          AddChild(type_expression);
          new_expr_type = reinterpret_cast<Expression *>(children_.back())->expr_type_;
          if (new_expr_type == struct_expr || new_expr_type == lazy_boolean_expr
              || new_expr_type == assignment_expr || new_expr_type == compound_assignment_expr) {
            ThrowErr(type_expression, "Expression: Unexpected ExcludedConditions.");
          }
        }
      } catch (...) {
        Restore(size_before_trying_let_chain, ptr_before_trying_let_chain);
        std::cerr << "Expression: Successfully handle LetChain trying failure.\n";
        // Expression except StructExpression
        AddChild(type_expression);
        if (reinterpret_cast<Expression *>(children_.back())->expr_type_ == struct_expr) {
          ThrowErr(type_expression, "Unexpected StructExpression.");
        }
      }
      // {
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      // Statements?
      if (tokens_[ptr_].GetStr() != "}") {
        // Statements
        AddChild(type_statements);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
      }
      // }
      if (tokens_[ptr_].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type_ == if_expr) {
      // if
      if (tokens_[ptr_].GetStr() != "if") {
        ThrowErr(type_expression, "Expect \"if\".");
      }
      AddChild(type_keyword);
      // Conditions
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      const int size_before_trying_let_chain = static_cast<int>(children_.size()),
          ptr_before_trying_let_chain = ptr_;
      try {
        // LetChain
        // LetChainCondition
        if (tokens_[ptr_].GetStr() == "let") {
          // let Pattern =
          // let
          AddChild(type_keyword);
          // Pattern
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
          AddChild(type_pattern);
          // =
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
          if (tokens_[ptr_].GetStr() != "=") {
            ThrowErr(type_expression, "Expect \"=\".");
          }
          AddChild(type_punctuation);
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
        }
        // Expression except ExcludedConditions
        AddChild(type_expression);
        ExprType new_expr_type = reinterpret_cast<Expression *>(children_.back())->expr_type_;
        if (new_expr_type == struct_expr || new_expr_type == lazy_boolean_expr
            || new_expr_type == assignment_expr || new_expr_type == compound_assignment_expr) {
          ThrowErr(type_expression, "Expression: Unexpected ExcludedConditions.");
        }
        // (&&LetChainCondition)*
        while (ptr_ >= tokens_.size() && tokens_[ptr_].GetStr() == "&&") {
          // &&LetChainCondition
          // &&
          AddChild(type_punctuation);
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
          // LetChainCondition
          if (tokens_[ptr_].GetStr() == "let") {
            // let Pattern =
            // let
            AddChild(type_keyword);
            // Pattern
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
            AddChild(type_pattern);
            // =
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
            if (tokens_[ptr_].GetStr() != "=") {
              ThrowErr(type_expression, "Expect \"=\".");
            }
            AddChild(type_punctuation);
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
          }
          // Expression except ExcludedConditions
          AddChild(type_expression);
          new_expr_type = reinterpret_cast<Expression *>(children_.back())->expr_type_;
          if (new_expr_type == struct_expr || new_expr_type == lazy_boolean_expr
              || new_expr_type == assignment_expr || new_expr_type == compound_assignment_expr) {
            ThrowErr(type_expression, "Expression: Unexpected ExcludedConditions.");
          }
        }
      } catch (...) {
        Restore(size_before_trying_let_chain, ptr_before_trying_let_chain);
        std::cerr << "Expression: Successfully handle LetChain trying failure.\n";
        // Expression except StructExpression
        AddChild(type_expression);
        if (reinterpret_cast<Expression *>(children_.back())->expr_type_ == struct_expr) {
          ThrowErr(type_expression, "Unexpected StructExpression.");
        }
      }
      // {
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      // Statements?
      if (tokens_[ptr_].GetStr() != "}") {
        // Statements
        AddChild(type_statements);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
      }
      // }
      if (tokens_[ptr_].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
      // (else(BlockExpression|IfExpression))?
      if (ptr_ < tokens_.size() && tokens_[ptr_].GetStr() == "else") {
        AddChild(type_keyword);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
        AddChild(type_expression);
        ExprType new_expr_type = reinterpret_cast<Expression *>(children_.back())->expr_type_;
        if (new_expr_type != block_expr && new_expr_type != if_expr) {
          ThrowErr(type_expression, "Expect BlockExpr or IfExpr.");
        }
      }
    } else if (expr_type_ == match_expr) {
      // match
      if (tokens_[ptr_].GetStr() != "match") {
        ThrowErr(type_expression, "Expect \"match\".");
      }
      AddChild(type_keyword);
      // Expression except StructExpression
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      AddChild(type_expression);
      if (reinterpret_cast<Expression *>(children_.back())->expr_type_ == struct_expr) {
        ThrowErr(type_expression, "Unexpected StructExpression.");
      }
      // {
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      // MatchArms?
      while (ptr_ < tokens_.size() && tokens_[ptr_].GetStr() != "}") {
        // Pattern
        AddChild(type_pattern);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
        // MatchArmGuard?
        if (tokens_[ptr_].GetStr() == "if") {
          // MatchArmGuard
          // if
          AddChild(type_keyword);
          // Expression
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
          AddChild(type_expression);
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
        }
        // =>
        if (tokens_[ptr_].GetStr() != "=>") {
          ThrowErr(type_expression, "Expect \"=>\".");
        }
        AddChild(type_punctuation);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
        AddChild(type_expression);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
        ExprType new_expr_type = reinterpret_cast<Expression *>(children_.back())->expr_type_;
        if (new_expr_type == block_expr || new_expr_type == const_block_expr
            || new_expr_type == infinite_loop_expr || new_expr_type == predicate_loop_expr
            || new_expr_type == if_expr || new_expr_type == match_expr) {
          // ,?
          if (tokens_[ptr_].GetStr() == ",") {
            AddChild(type_punctuation);
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
          }
        } else {
          // ,?
          if (tokens_[ptr_].GetStr() == ",") {
            AddChild(type_punctuation);
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
          } else {
            break;
          }
        }
      }
      // }
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type_ == grouped_expr) {
      // (
      if (tokens_[ptr_].GetStr() != "(") {
        ThrowErr(type_expression, "Expect \"(\".");
      }
      AddChild(type_punctuation);
      // Expression
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      AddChild(type_expression);
      // )
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != ")") {
        ThrowErr(type_expression, "Expect \")\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type_ == array_expr) {
      // [
      if (tokens_[ptr_].GetStr() != "[") {
        ThrowErr(type_expression, "Expect \"[\".");
      }
      AddChild(type_punctuation);
      // Expression
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() == "]") {
        AddChild(type_punctuation);
        return;
      }
      AddChild(type_expression);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() == ";") {
        // ; Expression
        // ;
        AddChild(type_punctuation);
        // Expression
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
        AddChild(type_expression);
        // ]
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
        if (tokens_[ptr_].GetStr() != "]") {
          ThrowErr(type_expression, "Expect \"]\".");
        }
        AddChild(type_punctuation);
      } else {
        while (ptr_ < tokens_.size() && tokens_[ptr_].GetStr() != "]") {
          // ,
          if (tokens_[ptr_].GetStr() != ",") {
            ThrowErr(type_expression, "Expect \",\".");
          }
          AddChild(type_punctuation);
          // Expression
          if (ptr_ >= tokens_.size()) {
            break;
          }
          if (tokens_[ptr_].GetStr() == "]") {
            break;
          }
          AddChild(type_expression);
        }
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
        if (tokens_[ptr_].GetStr() != "]") {
          ThrowErr(type_expression, "Expect \"]\".");
        }
        AddChild(type_punctuation);
      }
    } else if (expr_type_ == struct_expr) {
      // PathInExpression
      AddChild(type_path_in_expression);
      // {
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() == "}") {
        AddChild(type_punctuation);
        return;
      }
      // StructExprFields
      AddChild(type_struct_expr_fields);
      // }
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type_ == continue_expr) {
      if (tokens_[ptr_].GetStr() != "continue") {
        ThrowErr(type_expression, "Expect \"continue\".");
      }
      AddChild(type_keyword);
    } else if (expr_type_ == break_expr) {
      if (tokens_[ptr_].GetStr() != "break") {
        ThrowErr(type_expression, "Expect \"break\".");
      }
      AddChild(type_keyword);
      if (ptr_ < tokens_.size()) {
        const int size_before_trying_expression = static_cast<int>(children_.size()),
          ptr_before_trying_expression = ptr_;
        try {
          AddChild(type_expression);
        } catch (...) {
          Restore(size_before_trying_expression, ptr_before_trying_expression);
          std::cerr << "BreakExpr: Successfully handle expression try failure.\n";
          return;
        }
      }
    } else if (expr_type_ == return_expr) {
      if (tokens_[ptr_].GetStr() != "return") {
        ThrowErr(type_expression, "Expect \"return\".");
      }
      AddChild(type_keyword);
      if (ptr_ < tokens_.size()) {
        const int size_before_trying_expression = static_cast<int>(children_.size()),
          ptr_before_trying_expression = ptr_;
        try {
          AddChild(type_expression);
        } catch (...) {
          Restore(size_before_trying_expression, ptr_before_trying_expression);
          std::cerr << "ReturnExpr: Successfully handle expression try failure.\n";
          return;
        }
      }
    } else if (expr_type_ == underscore_expr) {
      if (tokens_[ptr_].GetStr() != "_") {
        ThrowErr(type_expression, "Expect \"_\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type_ == literal_expr) {
      const std::string next_token = tokens_[ptr_].GetStr();
      if (next_token == "true" || next_token == "false") {
        AddChild(type_keyword);
      }
      const type next_token_type = tokens_[ptr_].GetType();
      if (next_token_type == CHAR_LITERAL) {
        AddChild(type_char_literal);
      } else if (next_token_type == STRING_LITERAL) {
        AddChild(type_string_literal);
      } else if (next_token_type == RAW_STRING_LITERAL) {
        AddChild(type_raw_string_literal);
      } else if (next_token_type == C_STRING_LITERAL) {
        AddChild(type_c_string_literal);
      } else if (next_token_type == RAW_C_STRING_LITERAL) {
        AddChild(type_raw_c_string_literal);
      } else if (next_token_type == INTEGER_LITERAL) {
        AddChild(type_integer_literal);
      } else {
        ThrowErr(type_expression, R"(Expect LITERAL or "true" or "false".)");
      }
    } else if (expr_type_ == path_in_expr) {
      AddChild(type_path_in_expression);
    } else {
      // Pratt Parsing: Idea comes from https://www.bilibili.com/video/BV12d79zxEQn?vd_source=801f1864d3cf02d7adaecff6567a38bc
      ExprType next_type = GetNextExprType();
      // get lhs
      if (next_type != unknown) {
        lhs = new Expression(tokens_, ptr_, next_type, 0.0);
      } else {
        try {
          lhs = new Expression(tokens_, ptr_, struct_expr, 0.0);
        } catch (...) {
          delete lhs;
          lhs = nullptr;
          std::cerr << "Expression: Successfully handle the struct expression try failure.\n";
          lhs = new Expression(tokens_, ptr_, path_in_expr, 0.0);
        }
      }
      while (ptr_ < tokens_.size()) {
        // set op
        infix_ = GetInfix(tokens_[ptr_].GetStr());
        if (infix_ == not_infix) {
          // no more infix, expression comes to the end
          children_.push_back(lhs);
          type_.push_back(type_expression);
          return;
        }
        // valid infix, this is not the ending of the expression
        if (GetBP(infix_, 0) > min_bp) {
          // binding power is stronger than the op outside, continue to construct
          ++ptr_;
          rhs = new Expression(tokens_, ptr_, unknown, GetBP(infix_, 1));
          lhs = new Expression(lhs, rhs, infix_);
          rhs = nullptr;
          infix_ = not_infix;
        } else {
          // binding power is weaker than the op outside, construction ends
          children_.push_back(lhs);
          type_.push_back(type_expression);
          infix_ = not_infix;
          return;
        }
      }
    }
  } catch (...) {
    delete lhs;
    delete rhs;
    Restore(0, ptr_before_try);
    throw "";
  }
}

/*ExprType Expression::GetExprTypeForTest() const {
  return expr_type_;
}*/

Infix Expression::GetInfixForTest() const {
  return infix_;
}


StructExprField::StructExprField(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    if (tokens_[ptr_].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_struct_expr_field, "Expect IDENTIFIER.");
    }
    AddChild(type_identifier);
    if (ptr_ < tokens_.size() && tokens_[ptr_].GetStr() == ":") {
      // :
      AddChild(type_punctuation);
      // Expression
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_struct_expr_field, "");
      }
      AddChild(type_expression);
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}

StructExprFields::StructExprFields(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    AddChild(type_struct_expr_field);
    while (ptr_ < tokens_.size() && tokens_[ptr_].GetStr() == ",") {
      AddChild(type_punctuation);
      if (ptr_ >= tokens_.size()) {
        return;
      }
      const int size_before_trying_expression = static_cast<int>(children_.size()),
          ptr_before_trying_expression = ptr_;
      try {
        // Expression
        AddChild(type_expression);
      } catch (...) {
        Restore(size_before_trying_expression, ptr_before_trying_expression);
        std::cerr << "StructExprFields: Successfully handle expression try failure.\n";
        return;
      }
    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}