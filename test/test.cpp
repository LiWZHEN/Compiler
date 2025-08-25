#include <gtest/gtest.h>
#include "tokenizer.h"
#include "classes.h"
#include "builder.h"

void SuccessCheck(const std::string &str, const bool expect_success) {
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
}

TEST(ParserTest, EmptyMainFunction1) {
  SuccessCheck("fn main();", true);
}

TEST(ParserTest, EmptyMainFunction2) {
  SuccessCheck("const fn main(){}", true);
}

TEST(ParserTest, InvalidIdentifier) {
  SuccessCheck("fn fn();", false);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}