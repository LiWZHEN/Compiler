#include "classes.h"
#include "check_and_generate_IR.h"
#include <fstream>
#include <gtest/gtest.h>

TEST(IRTestBatch, mutable_range_test) {
  for (int i = 1; i <= 1; ++i) {
    const std::string file = "../RCompiler-Testcases/IR-1/src/comprehensive" + std::to_string(i) +
        "/comprehensive" + std::to_string(i);
    const std::string code_file = file + ".rx";
    const std::string IR_file = file + ".ll";
    std::ifstream reader(code_file);
    if (!reader.is_open()) {
      std::cerr << "Cannot open " << code_file << '\n';
      ASSERT_EQ(0, 1);
    }
    std::string code;
    std::string line;
    while (std::getline(reader, line)) {
      code += line;
    }
    FrontEndRunner runner(code);
    runner.Run(IR_file);
  }
}

TEST(IRTestBatch, run_all) {
  for (int i = 1; i <= 50; ++i) {
    const std::string file = "../RCompiler-Testcases/IR-1/src/comprehensive" + std::to_string(i) +
        "/comprehensive" + std::to_string(i);
    const std::string code_file = file + ".rx";
    const std::string IR_file = file + ".ll";
    std::ifstream reader(code_file);
    if (!reader.is_open()) {
      std::cerr << "Cannot open " << code_file << '\n';
      ASSERT_EQ(0, 1);
    }
    std::string code;
    std::string line;
    while (std::getline(reader, line)) {
      code += line;
    }
    FrontEndRunner runner(code);
    runner.Run(IR_file);
  }
}

TEST(IRTestSingle, run_debugging) {
  const std::string file = "../RCompiler-Testcases/working_space/debugging";
  const std::string code_file = file + ".rx";
  const std::string IR_file = file + ".ll";
  std::ifstream reader(code_file);
  if (!reader.is_open()) {
    std::cerr << "Cannot open " << code_file << '\n';
    ASSERT_EQ(0, 1);
  }
  std::string code;
  std::string line;
  while (std::getline(reader, line)) {
    code += line;
  }
  FrontEndRunner runner(code);
  runner.Run(IR_file);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}