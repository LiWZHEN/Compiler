#include "expression.h"

LiteralExpression::LiteralExpression(const std::vector<Token> &tokens, int &ptr) : LeafNode(tokens, ptr) {
  const std::string next_token = token_.GetStr();
  const type next_type = token_.GetType();
  if (next_type != CHAR_LITERAL && next_type != STRING_LITERAL && next_type != RAW_STRING_LITERAL && next_type != C_STRING_LITERAL
      && next_type != RAW_C_STRING_LITERAL && next_type != INTEGER_LITERAL && next_token != "true" && next_token != "false") {
    --ptr;
    ThrowErr(type_literal_expression, "Invalid literal expression.");
  }
}

Expression::Expression(const std::vector<Token> &tokens, int &ptr, ExprType expr_type, int min_bp) : Node(tokens, ptr), expr_type_(expr_type) {
  if (expr_type == unknown) {
    const std::string next_token = tokens_[ptr_].GetStr();
    if (next_token == "{") {
      expr_type = block_expr;
      expr_type_ = block_expr;
    } else if (next_token == "const") {
      expr_type = const_block_expr;
      expr_type_ = const_block_expr;
    } else if (next_token == "loop") {
      expr_type = infinite_loop_expr;
      expr_type_ = infinite_loop_expr;
    } else if (next_token == "while") {
      expr_type = predicate_loop_expr;
      expr_type_ = predicate_loop_expr;
    } else if (next_token == "if") {
      expr_type = if_expr;
      expr_type_ = if_expr;
    } else if (next_token == "match") {
      expr_type = match_expr;
      expr_type_ = match_expr;
    } else if (next_token == "(") {
      expr_type = grouped_expr;
      expr_type_ = grouped_expr;
    } else if (next_token == "[") {
      expr_type = index_expr;
      expr_type_ = index_expr;
    } else if (next_token == "continue") {
      expr_type = continue_expr;
      expr_type_ = continue_expr;
    } else if (next_token == "break") {
      expr_type = break_expr;
      expr_type_ = break_expr;
    } else if (next_token == "return") {
      expr_type = return_expr;
      expr_type_ = return_expr;
    } else if (next_token == "_") {
      expr_type = underscore_expr;
      expr_type_ = underscore_expr;
    }
  }
  const int ptr_before_try = ptr;
  try {
    if (expr_type == block_expr) {
      // {
      if (tokens[ptr].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_expression, "");
      }
      // Statements?
      if (tokens[ptr].GetStr() != "}") {
        // Statements
        AddChild(type_statements);
        if (ptr >= tokens.size()) {
          ThrowErr(type_expression, "");
        }
      }
      // }
      if (tokens[ptr].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type == const_block_expr) {
      // const
      if (tokens[ptr].GetStr() != "const") {
        ThrowErr(type_expression, "Expect \"const\".");
      }
      AddChild(type_keyword);
      // {
      if (ptr >= tokens.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens[ptr].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_expression, "");
      }
      // Statements?
      if (tokens[ptr].GetStr() != "}") {
        // Statements
        AddChild(type_statements);
        if (ptr >= tokens.size()) {
          ThrowErr(type_expression, "");
        }
      }
      // }
      if (tokens[ptr].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type == infinite_loop_expr) {
      // loop
      if (tokens[ptr].GetStr() != "loop") {
        ThrowErr(type_expression, "Expect \"loop\".");
      }
      AddChild(type_keyword);
      // {
      if (ptr >= tokens.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens[ptr].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_expression, "");
      }
      // Statements?
      if (tokens[ptr].GetStr() != "}") {
        // Statements
        AddChild(type_statements);
        if (ptr >= tokens.size()) {
          ThrowErr(type_expression, "");
        }
      }
      // }
      if (tokens[ptr].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type == predicate_loop_expr) {
      // while
      if (tokens[ptr].GetStr() != "while") {
        ThrowErr(type_expression, "Expect \"while\".");
      }
      AddChild(type_keyword);
      // Conditions
      if (ptr >= tokens.size()) {
        ThrowErr(type_expression, "");
      }
      const int size_before_trying_let_chain = static_cast<int>(children_.size()),
          ptr_before_trying_let_chain = ptr;
      try {
        // LetChain
        // LetChainCondition
        if (tokens[ptr].GetStr() == "let") {
          // let Pattern =
          // let
          AddChild(type_keyword);
          // Pattern
          if (ptr >= tokens.size()) {
            ThrowErr(type_expression, "");
          }
          AddChild(type_pattern);
          // =
          if (ptr >= tokens.size()) {
            ThrowErr(type_expression, "");
          }
          if (tokens[ptr].GetStr() != "=") {
            ThrowErr(type_expression, "Expect \"=\".");
          }
          AddChild(type_punctuation);
          if (ptr >= tokens.size()) {
            ThrowErr(type_expression, "");
          }
        }
        // Expression except ExcludedConditions
        auto *expr_ptr = new Expression(tokens, ptr, unknown, 0);
        if (expr_ptr->expr_type_ == struct_expr || expr_ptr->expr_type_ == lazy_boolean_expr
            || expr_ptr->expr_type_ == assignment_expr || expr_ptr->expr_type_ == compound_assignment_expr) {
          delete expr_ptr;
          expr_ptr = nullptr;
          ThrowErr(type_expression, "Expression: Expect ExcludedConditions.");
        }
        children_.push_back(reinterpret_cast<Node *>(expr_type));
        type_.push_back(type_expression);
        // (&&LetChainCondition)*
        while (ptr >= tokens.size() && tokens[ptr].GetStr() == "&&") {
          // &&LetChainCondition
          // &&
          AddChild(type_punctuation);
          if (ptr >= tokens.size()) {
            ThrowErr(type_expression, "");
          }
          // LetChainCondition
          if (tokens[ptr].GetStr() == "let") {
            // let Pattern =
            // let
            AddChild(type_keyword);
            // Pattern
            if (ptr >= tokens.size()) {
              ThrowErr(type_expression, "");
            }
            AddChild(type_pattern);
            // =
            if (ptr >= tokens.size()) {
              ThrowErr(type_expression, "");
            }
            if (tokens[ptr].GetStr() != "=") {
              ThrowErr(type_expression, "Expect \"=\".");
            }
            AddChild(type_punctuation);
            if (ptr >= tokens.size()) {
              ThrowErr(type_expression, "");
            }
          }
          // Expression except ExcludedConditions
          expr_ptr = new Expression(tokens, ptr, unknown, 0);
          if (expr_ptr->expr_type_ == struct_expr || expr_ptr->expr_type_ == lazy_boolean_expr
            || expr_ptr->expr_type_ == assignment_expr || expr_ptr->expr_type_ == compound_assignment_expr) {
            delete expr_ptr;
            expr_ptr = nullptr;
          ThrowErr(type_expression, "Expression: Expect ExcludedConditions.");
          }
          children_.push_back(reinterpret_cast<Node *>(expr_type));
          type_.push_back(type_expression);
        }
      } catch (...) {
        Restore(size_before_trying_let_chain, ptr_before_trying_let_chain);
        std::cerr << "Expression: Successfully handle LetChain trying failure.\n";
        // Expression except StructExpression

      }
      // {
      if (ptr >= tokens.size()) {
        ThrowErr(type_expression, "");
      }
      if (tokens[ptr].GetStr() != "{") {
        ThrowErr(type_expression, "Expect \"{\".");
      }
      AddChild(type_punctuation);
      if (ptr >= tokens.size()) {
        ThrowErr(type_expression, "");
      }
      // Statements?
      if (tokens[ptr].GetStr() != "}") {
        // Statements
        AddChild(type_statements);
        if (ptr >= tokens.size()) {
          ThrowErr(type_expression, "");
        }
      }
      // }
      if (tokens[ptr].GetStr() != "}") {
        ThrowErr(type_expression, "Expect \"}\".");
      }
      AddChild(type_punctuation);
    } else if (expr_type == if_expr) {

    } else if (expr_type == match_expr) {

    } else if (expr_type == literal_expr) {

    } else if (expr_type == path_in_expr) {

    } else if (expr_type == grouped_expr) {

    } else if (expr_type == array_expr) {

    } else if (expr_type == struct_expr) {

    } else if (expr_type == continue_expr) {

    } else if (expr_type == break_expr) {

    } else if (expr_type == return_expr) {

    } else if (expr_type == underscore_expr) {

    } else {

    }
  } catch (...) {
    Restore(0, ptr_before_try);
    throw "";
  }
}