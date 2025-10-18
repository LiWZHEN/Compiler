#include <gtest/gtest.h>
#include "tokenizer.h"
#include "classes.h"
#include "builder.h"
#include "visitor_manager.h"

/*void SuccessCheck(const std::string &str, const bool expect_success) {
  std::vector<Token> tokens;
  Tokenizer tokenizer(str, tokens);
  tokenizer.Tokenize();
  Builder builder(tokens);
  Node *syntax_tree = builder.GetTree();
  if (expect_success) {
    ASSERT_NE(syntax_tree, nullptr);
    std::cout << syntax_tree->GetStruct("", true);
    delete syntax_tree;
  } else {
    ASSERT_EQ(syntax_tree, nullptr);
  }
}*/

void SemanticCheck(const std::string &str, const bool expect_success) {
  std::vector<Token> tokens;
  Tokenizer tokenizer(str, tokens);
  tokenizer.Tokenize();
  Builder builder(tokens);
  Crate *syntax_tree = builder.GetTree();
  VisitorManager visitor_manager;
  visitor_manager.VisitAll(syntax_tree);
  bool result_matched;
  if ((syntax_tree != nullptr && expect_success) ||
      (syntax_tree == nullptr && !expect_success)) {
    result_matched = true;
  } else {
    result_matched = false;
  }
  delete syntax_tree;
  ASSERT_EQ(result_matched, true);
}

TEST(SemanticTest, empty_main_1) {
  SemanticCheck("fn main() { exit(0) }", true);
}

TEST(SemanticTest, empty_main_without_exit_2) {
  SemanticCheck("fn main() {}", false);
}

TEST(SemanticTest, no_main_3) {
  SemanticCheck("fn f() {}", false);
}

TEST(SemanticTest, no_outmost_main_4) {
  SemanticCheck("fn f() {fn main() {exit(0)}}", false);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}