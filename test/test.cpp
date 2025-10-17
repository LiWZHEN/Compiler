#include <gtest/gtest.h>
#include "tokenizer.h"
#include "classes.h"
#include "builder.h"
#include "visitor_manager.h"

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

void SemanticCheck(const std::string &str, const bool expect_success) {
  std::vector<Token> tokens;
  Tokenizer tokenizer(str, tokens);
  tokenizer.Tokenize();
  Builder builder(tokens);
  Crate *syntax_tree = builder.GetTree();
  VisitorManager visitor_manager;
  visitor_manager.VisitAll(syntax_tree);
  if (expect_success) {
    ASSERT_NE(syntax_tree, nullptr);
    delete syntax_tree;
  } else {
    ASSERT_EQ(syntax_tree, nullptr);
  }
}

TEST(ParserTest, ExpressionTest1) {
  SuccessCheck("fn main(){while(true){print(a)}exit(0);}", true);
}

TEST(ParserTest, ExpressionTest2) {
  SuccessCheck("fn main(){if(true){print(a)}else{print(b)}}", true);
}

TEST(ParserTest, ExpressionTest3) {
  SuccessCheck("fn main(){loop{1}}", true);
}

TEST(ParserTest, ExpressionTest4) {
  SuccessCheck(R"(fn main(){A::a.f(x, y, z)})", true);
}

TEST(ParserTest, ExpressionTest5) {
  SuccessCheck("fn main() {f(a)}", true);
}

TEST(ParserTest, ExpressionTest6) {
  SuccessCheck("fn main() {f(\"str\")}", true);
}

TEST(ParserTest, ExpressionTest7) {
  SuccessCheck("fn main() {-1 + *A}", true);
}

TEST(ParserTest, ExpressionTest8) {
  SuccessCheck("fn main() {let mut a : t = 1;}", true);
}

TEST(ParserTest, ExpressionTest9) {
  SuccessCheck("fn main() {const a : t = 1;}", true);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}