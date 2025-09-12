#include "expression.h"
#include "scope.h"

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
      case brackets_closure:
      case small_brackets_closure:
        return 0.0;
      default:
        throw;
    }
  }
  switch (op) {
    case brackets:
    case small_brackets:
      return 0.1;
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
      throw;
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
  if (op == "]") {
    return brackets_closure;
  }
  if (op == ")") {
    return small_brackets_closure;
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

BlockExpression::BlockExpression(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr;
  try {
    // {
    if (tokens[ptr].GetStr() != "{") {
      ThrowErr(type_block_expression, "Expect \"{\".");
    }
    AddChild(type_punctuation);
    if (ptr >= tokens.size()) {
      ThrowErr(type_block_expression, "");
    }
    // Statements?
    if (tokens[ptr].GetStr() != "}") {
      // Statements
      AddChild(type_statements);
    }
    // }
    if (ptr >= tokens.size()) {
      ThrowErr(type_block_expression, "");
    }
    if (tokens[ptr].GetStr() != "}") {
      ThrowErr(type_block_expression, "Expect \"}\".");
    }
    AddChild(type_punctuation);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw;
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
  if (next_token == "&" || next_token == "&&" || next_token == "*" || next_token == "-" || next_token == "!") {
    return prefix_expr;
  }
  type next_token_type = tokens_[ptr_].GetType();
  if (next_token_type == CHAR_LITERAL || next_token_type == STRING_LITERAL || next_token_type == RAW_STRING_LITERAL
      || next_token_type == C_STRING_LITERAL || next_token_type == RAW_C_STRING_LITERAL || next_token_type == INTEGER_LITERAL) {
    return literal_expr;
  }
  return unknown;
}

Expression::Expression(const std::vector<Token> &tokens, int &ptr, Expression *lhs, Expression *rhs, Infix infix) : Node(tokens, ptr), expr_type_(unknown), infix_(infix) {
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
      // (
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != "(") {
        ThrowErr(type_expression, "Expect \"(\".");
      }
      AddChild(type_punctuation);
      // Expression except StructExpression
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      AddChild(type_expression);
      if (reinterpret_cast<Expression *>(children_.back())->expr_type_ == struct_expr) {
        ThrowErr(type_expression, "Unexpected StructExpression.");
      }
      // )
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != ")") {
        ThrowErr(type_expression, "Expect \")\".");
      }
      AddChild(type_punctuation);
      // BlockExpression
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
      // (
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != "(") {
        ThrowErr(type_expression, "Expect \"(\".");
      }
      AddChild(type_punctuation);
      // Expression except StructExpression
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      AddChild(type_expression);
      if (reinterpret_cast<Expression *>(children_.back())->expr_type_ == struct_expr) {
        ThrowErr(type_expression, "Unexpected StructExpression.");
      }
      // )
      if (ptr_ >= tokens_.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens_[ptr_].GetStr() != ")") {
        ThrowErr(type_expression, "Expect \")\".");
      }
      AddChild(type_punctuation);
      // BlockExpression
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
    } else if (expr_type_ == prefix_expr) {
      std::string next_token = tokens_[ptr_].GetStr();
      if (next_token == "&" || next_token == "&&") {
        AddChild(type_punctuation);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
        if (tokens_[ptr_].GetStr() == "mut") {
          AddChild(type_keyword);
          if (ptr_ >= tokens_.size()) {
            ThrowErr(type_expression, "");
          }
        }
      } else if (next_token == "*" || next_token == "-" || next_token == "!") {
        AddChild(type_punctuation);
        if (ptr_ >= tokens_.size()) {
          ThrowErr(type_expression, "");
        }
      } else {
        ThrowErr(type_expression, R"(Expect "&" or "&&" or "*" or "-" or "!")");
      }
      // Add an expression with min_bp greater than 11.5
      const int size_before_trying_adding_expr = static_cast<int>(children_.size());
      const int ptr_before_trying_adding_expr = ptr_;
      try {
        children_.push_back(new Expression(tokens_, ptr_, unknown, 11.5));
        type_.push_back(type_expression);
      } catch (...) {
        Restore(size_before_trying_adding_expr, ptr_before_trying_adding_expr);
        throw;
      }
    } else if (expr_type_ == call_params) {
      while (ptr_ < tokens_.size() && tokens_[ptr_].GetStr() != ")") {
        AddChild(type_expression);
        if (ptr_ >= tokens_.size() || tokens_[ptr_].GetStr() == ")") {
          return;
        }
        if (tokens_[ptr_].GetStr() != ",") {
          ThrowErr(type_expression, "Expect \",\".");
        }
        AddChild(type_punctuation);
      }
    } else {
      // Pratt Parsing: Idea comes from https://www.bilibili.com/video/BV12d79zxEQn?vd_source=801f1864d3cf02d7adaecff6567a38bc
      ExprType next_type = GetNextExprType();
      // get lhs
      if (next_type != unknown) {
        lhs = new Expression(tokens_, ptr_, next_type, 0.0);
      } else {
        std::string next_token = tokens_[ptr_].GetStr();
        if (next_token == "&" || next_token == "&&" || next_token == "*" || next_token == "-" || next_token == "!") {
          lhs = new Expression(tokens_, ptr_, prefix_expr, 0.0);
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
      }
      while (ptr_ < tokens_.size()) {
        // set op
        infix_ = GetInfix(tokens_[ptr_].GetStr());
        if (infix_ == not_infix || infix_ == brackets_closure || infix_ == small_brackets_closure) {
          // no more infix, expression comes to the end
          break;
        }
        // valid infix, this is not the ending of the expression
        if (GetBP(infix_, 0) > min_bp) {
          // binding power is stronger than the op outside, continue to construct
          ++ptr_; // consume the infix
          if (infix_ == small_brackets) {
            // the following should be call params
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
            rhs = new Expression(tokens_, ptr_, call_params, 0.0);
            // expect small brackets closure
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
            if (tokens_[ptr_].GetStr() != ")") {
              ThrowErr(type_expression, "Expect \")\".");
            }
            ++ptr_; // consume it
            lhs = new Expression(tokens_, ptr_, lhs, rhs, infix_);
            rhs = nullptr;
            infix_ = not_infix;
          } else if (infix_ == brackets) {
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
            rhs = new Expression(tokens_, ptr_, unknown, 0.1);
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
            if (tokens_[ptr_].GetStr() != "]") {
              ThrowErr(type_expression, "Expect \"]\".");
            }
            ++ptr_;
            lhs = new Expression(tokens_, ptr_, lhs, rhs, infix_);
            rhs = nullptr;
            infix_ = not_infix;
          } else {
            if (ptr_ >= tokens_.size()) {
              ThrowErr(type_expression, "");
            }
            rhs = new Expression(tokens_, ptr_, unknown, GetBP(infix_, 1));
            lhs = new Expression(tokens_, ptr_, lhs, rhs, infix_);
            rhs = nullptr;
            infix_ = not_infix;
          }
        } else {
          // binding power is weaker than the op outside, construction ends
          break;
        }
      }
      // no more op
      infix_ = lhs->infix_;
      expr_type_ = lhs->expr_type_;
      children_ = lhs->children_;
      type_ = lhs->type_;
    }
    if (lhs != nullptr) {
      for (auto &it : lhs->children_) {
        it = nullptr;
      }
      delete lhs;
      lhs = nullptr;
    }
    delete rhs;
    rhs = nullptr;
  } catch (...) {
    delete lhs;
    delete rhs;
    Restore(0, ptr_before_try);
    throw;
  }
}

ExprType Expression::GetExprType() const {
  return expr_type_;
}

Infix Expression::GetInfixForTest() const {
  return infix_;
}

StructExprField::StructExprField(const std::vector<Token> &tokens, int &ptr) : Node(tokens, ptr) {
  const int ptr_before_try = ptr_;
  try {
    // Identifier
    if (tokens_[ptr_].GetType() != IDENTIFIER_OR_KEYWORD) {
      ThrowErr(type_struct_expr_field, "Expect Identifier.");
    }
    AddChild(type_identifier);
    // :
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_struct_expr_field, "");
    }
    if (tokens_[ptr].GetStr() != ":") {
      ThrowErr(type_struct_expr_field, "Expect \":\".");
    }
    AddChild(type_punctuation);
    // Expression
    if (ptr_ >= tokens_.size()) {
      ThrowErr(type_struct_expr_field, "");
    }
    AddChild(type_expression);
  } catch (...) {
    Restore(0, ptr_before_try);
    throw;
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
    throw;
  }
}

std::string Expression::GetNodeLabel() const {
  switch (expr_type_) {
    case unknown:
      switch (infix_) {
        case brackets:
          return "[]";
        case small_brackets:
          return "()";
        case dot:
          return ".";
        case add:
          return "+";
        case minus:
          return "-";
        case multiply:
          return "*";
        case divide:
          return "/";
        case mod:
          return "%";
        case bitwise_and:
          return "&";
        case bitwise_or:
          return "|";
        case bitwise_xor:
          return "^";
        case left_shift:
          return "<<";
        case right_shift:
          return ">>";
        case is_equal:
          return "==";
        case is_not_equal:
          return "!=";
        case is_bigger:
          return ">";
        case is_smaller:
          return "<";
        case is_not_smaller:
          return ">=";
        case is_not_bigger:
          return "<=";
        case logic_or:
          return "||";
        case logic_and:
          return "&&";
        case type_cast:
          return "as";
        case assign:
          return "=";
        case add_assign:
          return "+=";
        case minus_assign:
          return "-=";
        case multiply_assign:
          return "*=";
        case divide_assign:
          return "/=";
        case mod_assign:
          return "%=";
        case bitwise_and_assign:
          return "&=";
        case bitwise_or_assign:
          return "|=";
        case bitwise_xor_assign:
          return "^=";
        case left_shift_assign:
          return "<<=";
        case right_shift_assign:
          return ">>=";
        default:
          return "unknown";
      }
    case block_expr:
      return "block_expr";
    case const_block_expr:
      return "const_block_expr";
    case infinite_loop_expr:
      return "infinite_loop_expr";
    case predicate_loop_expr:
      return "predicate_loop_expr";
    case if_expr:
      return "if_expr";
    case literal_expr:
      return "literal_expr";
    case path_in_expr:
      return "path_in_expr";
    case operator_expr:
      return "operator_expr";
    case grouped_expr:
      return "grouped_expr";
    case array_expr:
      return "array_expr";
    case index_expr:
      return "index_expr";
    case struct_expr:
      return "struct_expr";
    case call_expr:
      return "call_expr";
    case method_call_expr:
      return "method_call_expr";
    case field_expr:
      return "field_expr";
    case continue_expr:
      return "continue_expr";
    case break_expr:
      return "break_expr";
    case return_expr:
      return "return_expr";
    case underscore_expr:
      return "underscore_expr";
    case lazy_boolean_expr:
      return "lazy_boolean_expr";
    case assignment_expr:
      return "assignment_expr";
    case compound_assignment_expr:
      return "compound_assignment_expr";
    case prefix_expr:
      return "prefix_expr";
    case call_params:
      return "call_params";
    default:
      throw;
  }
}

std::string LiteralExpression::GetNodeLabel() const {
  return "LiteralExpression: " + token_.GetStr();
}

std::string BlockExpression::GetNodeLabel() const {
  return "BlockExpression";
}

std::string StructExprField::GetNodeLabel() const {
  return "StructExprField";
}

std::string StructExprFields::GetNodeLabel() const {
  return "StructExprFields";
}

void LiteralExpression::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void BlockExpression::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void Expression::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void StructExprField::Accept(Visitor *visitor) {
  visitor->Visit(this);
}

void StructExprFields::Accept(Visitor *visitor) {
  visitor->Visit(this);
}
