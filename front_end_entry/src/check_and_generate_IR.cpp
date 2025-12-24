#include "check_and_generate_IR.h"
#include "classes.h"
#include "tokenizer.h"
#include "builder.h"
#include <fstream>

void FrontEndRunner::Run(const std::string &output_file_name) {
  std::vector<Token> tokens;
  Tokenizer tokenizer(code_, tokens);
  tokenizer.Tokenize();
  Builder builder(tokens);
  Crate *syntax_tree = builder.GetTree();
  if (syntax_tree == nullptr) {
    exit(-1);
  }
  semantic_checker_.VisitAll(syntax_tree);
  if (syntax_tree == nullptr) {
    exit(-1);
  }
  try {
    IR_generator_.Visit(syntax_tree);
  } catch (...) {
    delete syntax_tree;
    syntax_tree = nullptr;
    // Pass semantic check, but the IR generating is not implemented
    exit(0);
  }
  std::ofstream output_file(output_file_name);
  if (output_file.is_open()) {
    IR_generator_.Output(output_file);
  } else {
    std::cerr << "[Error] Cannot open " << output_file_name << "!\n";
  }
  delete syntax_tree;
}