#include <gtest/gtest.h>
#include "tokenizer.h"
#include "classes.h"
#include "builder.h"

TEST(Parser1, array1) {
  std::vector<Token> tokens;
  const std::string str = "fn main() {\n    let numbers: [i32; 3] = [10, 20, 30];\n}";
  Tokenizer tokenizer(str, tokens);
  tokenizer.Tokenize();
  Builder builder(tokens);
  Node *syntax_tree = builder.GetTree();

  // parsing succeeded
  ASSERT_NE(syntax_tree, nullptr);

  // output
  std::cout << syntax_tree->GetStruct("", true);
  delete syntax_tree;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}