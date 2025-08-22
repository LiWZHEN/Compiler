#include <gtest/gtest.h>
#include "tokenizer.h"
#include "classes.h"
#include "builder.h"
#include "expression.h"

TEST(FunctionTest, EmptyMainFunction) {
  std::vector<Token> tokens;
  const std::string str = "fn main() {}";
  Tokenizer tokenizer(str, tokens);
  tokenizer.Tokenize();
  Builder builder(tokens);
  Node *syntax_tree = builder.GetTree();

  // parsing succeeded
  ASSERT_NE(syntax_tree, nullptr);

  // Crate->Item
  ASSERT_EQ(syntax_tree->GetChildrenNum(), 1);
  std::vector<NodeType> expected;
  expected.push_back(type_item);
  ASSERT_EQ(syntax_tree->GetChildrenType(), expected);

  // Item->Function
  const auto item_ptr = syntax_tree->GetChildrenPtr()[0];
  ASSERT_NE(item_ptr, nullptr);
  ASSERT_EQ(item_ptr->GetChildrenNum(), 1);
  expected.clear();
  expected.push_back(type_function);
  ASSERT_EQ(item_ptr->GetChildrenType(), expected);

  // Function->Keyword:fn Identifier Punctuation:( Punctuation:) BlockExpression
  const auto function_ptr = item_ptr->GetChildrenPtr()[0];
  ASSERT_NE(function_ptr, nullptr);
  ASSERT_EQ(function_ptr->GetChildrenNum(), 5);
  expected.clear();
  expected = {type_keyword, type_identifier, type_punctuation, type_punctuation, type_block_expression};
  ASSERT_EQ(function_ptr->GetChildrenType(), expected);

  const auto function_children_ptr_vct = function_ptr->GetChildrenPtr();
  const auto function_child_ptr1 = reinterpret_cast<LeafNode *>(function_children_ptr_vct[0]);
  ASSERT_EQ(function_child_ptr1->GetContent().GetType(), IDENTIFIER_OR_KEYWORD);
  ASSERT_EQ(function_child_ptr1->GetContent().GetStr(), "fn");

  const auto function_child_ptr2 = reinterpret_cast<LeafNode *>(function_children_ptr_vct[1]);
  ASSERT_EQ(function_child_ptr2->GetContent().GetType(), IDENTIFIER_OR_KEYWORD);
  ASSERT_EQ(function_child_ptr2->GetContent().GetStr(), "main");

  const auto function_child_ptr3 = reinterpret_cast<LeafNode *>(function_children_ptr_vct[2]);
  ASSERT_EQ(function_child_ptr3->GetContent().GetType(), PUNCTUATION);
  ASSERT_EQ(function_child_ptr3->GetContent().GetStr(), "(");

  const auto function_child_ptr4 = reinterpret_cast<LeafNode *>(function_children_ptr_vct[3]);
  ASSERT_EQ(function_child_ptr4->GetContent().GetType(), PUNCTUATION);
  ASSERT_EQ(function_child_ptr4->GetContent().GetStr(), ")");

  // BlockExpression->Punctuation:{ Punctuation:}
  const auto function_child_ptr5 = function_children_ptr_vct[4];
  ASSERT_NE(function_child_ptr5, nullptr);
  ASSERT_EQ(function_child_ptr5->GetChildrenNum(), 2);

  expected.clear();
  expected = {type_punctuation, type_punctuation};
  ASSERT_EQ(function_child_ptr5->GetChildrenType(), expected);

  const auto block_child_ptr1 = reinterpret_cast<LeafNode *>(function_child_ptr5->GetChildrenPtr()[0]);
  const auto block_child_ptr2 = reinterpret_cast<LeafNode *>(function_child_ptr5->GetChildrenPtr()[1]);
  ASSERT_EQ(block_child_ptr1->GetContent().GetType(), PUNCTUATION);
  ASSERT_EQ(block_child_ptr1->GetContent().GetStr(), "{");
  ASSERT_EQ(block_child_ptr2->GetContent().GetType(), PUNCTUATION);
  ASSERT_EQ(block_child_ptr2->GetContent().GetStr(), "}");
  delete syntax_tree;
}

TEST(ExpressionTest, OperatorExpression1) {
  std::vector<Token> tokens;
  const std::string str = "a";
  Tokenizer tokenizer(str, tokens);
  tokenizer.Tokenize();
  int ptr = 0;
  Node *expr_tree = new Expression(tokens, ptr, unknown, 0);

  // parsing succeeded
  ASSERT_NE(expr_tree, nullptr);
  std::cout << expr_tree->GetStruct("", true);
  delete expr_tree;
}

TEST(ExpressionTest, OperatorExpression2) {
  std::vector<Token> tokens;
  const std::string str = "a * c + b";
  Tokenizer tokenizer(str, tokens);
  tokenizer.Tokenize();
  int ptr = 0;
  Node *expr_tree = new Expression(tokens, ptr, unknown, 0);

  // parsing succeeded
  ASSERT_NE(expr_tree, nullptr);
  std::cout << expr_tree->GetStruct("", true);
  delete expr_tree;
}

TEST(ExpressionTest, WithPrefixAndBracketInfixes) {
  std::vector<Token> tokens;
  const std::string str = "b.z * &d + e[1 + x()]";
  Tokenizer tokenizer(str, tokens);
  tokenizer.Tokenize();
  int ptr = 0;
  Node *expr_tree = new Expression(tokens, ptr, unknown, 0);

  // parsing succeeded
  ASSERT_NE(expr_tree, nullptr);
  std::cout << expr_tree->GetStruct("", true);
  delete expr_tree;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}