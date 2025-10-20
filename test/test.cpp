#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include "tokenizer.h"
#include "classes.h"
#include "builder.h"
#include "visitor_manager.h"

namespace fs = std::filesystem;

class SemanticTestBatch : public ::testing::Test {
protected:
  void SetUp() override {
    testcases_base_path_ = DetectTestcasesPath();
    std::cout << "Testcases base path: " << testcases_base_path_ << std::endl;
  }

  std::string DetectTestcasesPath() {
    // 先尝试上级目录（本地调试环境）
    if (fs::exists("../RCompiler-Testcases/semantic-1/src")) {
      return "../RCompiler-Testcases";
    }

    // 如果找不到，抛出错误
    std::cerr << "Error: Cannot find RCompiler-Testcases directory" << std::endl;
    std::cerr << "Current working directory: " << fs::current_path() << std::endl;
    std::cerr << "Tried paths:" << std::endl;
    std::cerr << "  - ../RCompiler-Testcases/semantic-1/src" << std::endl;
    return "../RCompiler-Testcases";
  }

  std::string testcases_base_path_;
};

class StderrRedirector {
public:
  StderrRedirector(const std::string &filename) {
    original_stderr_ = std::cerr.rdbuf();
    file_.open(filename, std::ios::out);
    if (file_.is_open()) {
      std::cerr.rdbuf(file_.rdbuf());
    }
  }

  ~StderrRedirector() {
    if (file_.is_open()) {
      file_.close();
    }
    std::cerr.rdbuf(original_stderr_);
  }

private:
  std::streambuf *original_stderr_;
  std::ofstream file_;
};

struct TestCaseInfo {
  std::string code;
  std::string expected_verdict;
  std::string comment;
};

TestCaseInfo ParseTestFile(const std::string &filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Cannot open file: " << filepath << std::endl;
    return {"", "", ""};
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content = buffer.str();

  TestCaseInfo info;
  info.code = content;

  // Extract expected verdict using regex
  std::regex verdict_regex(R"(Verdict:\s*(\w+))");
  std::smatch verdict_match;
  if (std::regex_search(content, verdict_match, verdict_regex)) {
    info.expected_verdict = verdict_match[1];
  }

  // Extract comment using regex
  std::regex comment_regex(R"(Comment:\s*([^\n\r]+))");
  std::smatch comment_match;
  if (std::regex_search(content, comment_match, comment_regex)) {
    info.comment = comment_match[1];
  }

  return info;
}

bool SemanticCheck(const std::string &test_name, const std::string &code, const bool output_tree = false) {
  std::vector<Token> tokens;
  Tokenizer tokenizer(code, tokens);
  tokenizer.Tokenize();
  Builder builder(tokens);
  Crate *syntax_tree = builder.GetTree();

  if (syntax_tree == nullptr) {
    std::cerr << "[" << test_name << "] Syntax analysis failed" << std::endl;
    return false;
  }

  if (output_tree) {
    std::cerr << syntax_tree->GetStruct("", true) << std::endl;
  }

  VisitorManager visitor_manager;
  visitor_manager.VisitAll(syntax_tree);

  bool success = (syntax_tree != nullptr);
  delete syntax_tree;

  if (success) {
    std::cerr << "[" << test_name << "] Semantic analysis successful" << std::endl;
  } else {
    std::cerr << "[" << test_name << "] Semantic analysis failed" << std::endl;
  }

  return success;
}

void RunBatchTests(const std::vector<std::string> &test_cases, const std::string &batch_name, const std::string &testcases_base_path) {
  std::cout << "=== Running Batch: " << batch_name << " ===" << std::endl;

  int passed_count = 0;
  int failed_count = 0;

  for (const auto &test_case: test_cases) {
    std::string test_path = testcases_base_path + "/semantic-1/src/" + test_case + "/" + test_case + ".rx";
    std::string output_path = testcases_base_path + "/semantic-1/src/" + test_case + "/" + test_case + ".out";

    TestCaseInfo test_info = ParseTestFile(test_path);
    if (test_info.code.empty()) {
      std::cout << "Skipping test case: " << test_case << " (file not found or empty)" << std::endl;
      continue;
    } {
      StderrRedirector redirector(output_path);
      std::cerr << "=== Test Case: " << test_case << " ===" << std::endl;
      std::cerr << "Expected Verdict: " << test_info.expected_verdict << std::endl;
      std::cerr << "Comment: " << test_info.comment << std::endl;
      std::cerr << "Input code:\n" << test_info.code << "\n" << std::endl;

      bool actual_result = SemanticCheck(test_case, test_info.code);
      std::cerr << "Actual result: " << (actual_result ? "PASS" : "FAIL") << std::endl;

      // Convert expected verdict to boolean
      bool expected_result = (test_info.expected_verdict == "Success");

      // Check if result matches expectation
      if (actual_result == expected_result) {
        std::cerr << "✓ TEST PASSED: Expected " << test_info.expected_verdict
            << ", got " << (actual_result ? "Success" : "Fail") << std::endl;
        passed_count++;
      } else {
        std::cerr << "✗ TEST FAILED: Expected " << test_info.expected_verdict
            << ", but got " << (actual_result ? "Success" : "Fail") << std::endl;
        failed_count++;
      }
      if (actual_result == expected_result) {
        std::cerr << "Final result: PASS" << std::endl;
        std::cout << test_case << ": PASS" << std::endl;
      } else {
        std::cerr << "Final result: FAIL" << std::endl;
        std::cout << test_case << ": FAIL!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      }
    }

    std::cout << "Completed: " << test_case << std::endl;
  }

  std::cout << "=== Finished Batch: " << batch_name << " ===" << std::endl;
  std::cout << "Summary: " << passed_count << " passed, " << failed_count << " failed" << std::endl << std::endl;
  ASSERT_EQ(failed_count, 0);
}

// Type tests
TEST_F(SemanticTestBatch, TypeTests) {
  std::vector<std::string> test_cases;
  for (int i = 1; i <= 20; i++) {
    test_cases.push_back("type" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Type Tests", testcases_base_path_);
}

// Array tests
TEST_F(SemanticTestBatch, ArrayTests) {
  std::vector<std::string> test_cases;
  for (int i = 1; i <= 8; i++) {
    test_cases.push_back("array" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Array Tests", testcases_base_path_);
}

// Autoref tests
TEST_F(SemanticTestBatch, AutorefTests) {
  std::vector<std::string> test_cases;
  for (int i = 1; i <= 9; i++) {
    test_cases.push_back("autoref" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Autoref Tests", testcases_base_path_);
}

// Basic tests - divided into smaller batches
TEST_F(SemanticTestBatch, BasicTests_1_10) {
  std::vector<std::string> test_cases;
  for (int i = 1; i <= 10; i++) {
    test_cases.push_back("basic" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Basic Tests 1-10", testcases_base_path_);
}

TEST_F(SemanticTestBatch, BasicTests_11_20) {
  std::vector<std::string> test_cases;
  for (int i = 11; i <= 20; i++) {
    test_cases.push_back("basic" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Basic Tests 11-20", testcases_base_path_);
}

TEST_F(SemanticTestBatch, BasicTests_21_30) {
  std::vector<std::string> test_cases;
  for (int i = 21; i <= 30; i++) {
    test_cases.push_back("basic" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Basic Tests 21-30", testcases_base_path_);
}

TEST_F(SemanticTestBatch, BasicTests_31_40) {
  std::vector<std::string> test_cases;
  for (int i = 31; i <= 40; i++) {
    test_cases.push_back("basic" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Basic Tests 31-40", testcases_base_path_);
}

// Expression tests - divided into smaller batches
TEST_F(SemanticTestBatch, ExpressionTests_1_10) {
  std::vector<std::string> test_cases;
  for (int i = 1; i <= 10; i++) {
    test_cases.push_back("expr" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Expression Tests 1-10", testcases_base_path_);
}

TEST_F(SemanticTestBatch, ExpressionTests_11_20) {
  std::vector<std::string> test_cases;
  for (int i = 11; i <= 20; i++) {
    test_cases.push_back("expr" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Expression Tests 11-20", testcases_base_path_);
}

TEST_F(SemanticTestBatch, ExpressionTests_21_30) {
  std::vector<std::string> test_cases;
  for (int i = 21; i <= 30; i++) {
    test_cases.push_back("expr" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Expression Tests 21-30", testcases_base_path_);
}

TEST_F(SemanticTestBatch, ExpressionTests_31_40) {
  std::vector<std::string> test_cases;
  for (int i = 31; i <= 40; i++) {
    test_cases.push_back("expr" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Expression Tests 31-40", testcases_base_path_);
}

// If tests
TEST_F(SemanticTestBatch, IfTests) {
  std::vector<std::string> test_cases;
  for (int i = 1; i <= 15; i++) {
    test_cases.push_back("if" + std::to_string(i));
  }
  RunBatchTests(test_cases, "If Tests", testcases_base_path_);
}

// Loop tests
TEST_F(SemanticTestBatch, LoopTests) {
  std::vector<std::string> test_cases;
  for (int i = 1; i <= 10; i++) {
    test_cases.push_back("loop" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Loop Tests", testcases_base_path_);
}

// Misc tests - divided into smaller batches
TEST_F(SemanticTestBatch, MiscTests_1_20) {
  std::vector<std::string> test_cases;
  for (int i = 1; i <= 20; i++) {
    test_cases.push_back("misc" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Misc Tests 1-20", testcases_base_path_);
}

TEST_F(SemanticTestBatch, MiscTests_21_40) {
  std::vector<std::string> test_cases;
  for (int i = 21; i <= 40; i++) {
    test_cases.push_back("misc" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Misc Tests 21-40", testcases_base_path_);
}

TEST_F(SemanticTestBatch, MiscTests_41_65) {
  std::vector<std::string> test_cases;
  for (int i = 41; i <= 65; i++) {
    test_cases.push_back("misc" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Misc Tests 41-65", testcases_base_path_);
}

// Return tests
TEST_F(SemanticTestBatch, ReturnTests) {
  std::vector<std::string> test_cases;
  for (int i = 1; i <= 15; i++) {
    test_cases.push_back("return" + std::to_string(i));
  }
  RunBatchTests(test_cases, "Return Tests", testcases_base_path_);
}

// Single test for debugging
TEST_F(SemanticTestBatch, SingleTest_Debug) {
  std::string test_case = "debugging"; // Change this to any test case you want to debug
  std::string test_path = testcases_base_path_ + "/working_space/" + test_case + ".rx";
  std::string output_path = testcases_base_path_ + "/working_space/" + test_case + ".out";

  TestCaseInfo test_info = ParseTestFile(test_path);
  ASSERT_FALSE(test_info.code.empty()) << "Test file is empty or not found: " << test_path;

  StderrRedirector redirector(output_path);
  std::cerr << "=== Debug Test Case: " << test_case << " ===" << std::endl;
  std::cerr << "Expected Verdict: " << test_info.expected_verdict << std::endl;
  std::cerr << "Comment: " << test_info.comment << std::endl;
  std::cerr << "Input code:\n" << test_info.code << "\n" << std::endl;

  bool actual_result = SemanticCheck(test_case, test_info.code, true);
  std::cerr << "Actual result: " << (actual_result ? "PASS" : "FAIL") << std::endl;

  bool expected_result = (test_info.expected_verdict == "Success");

  if (actual_result == expected_result) {
    std::cerr << "✓ TEST PASSED: Expected " << test_info.expected_verdict
        << ", got " << (actual_result ? "Success" : "Fail") << std::endl;
  } else {
    std::cerr << "✗ TEST FAILED: Expected " << test_info.expected_verdict
        << ", but got " << (actual_result ? "Success" : "Fail") << std::endl;
  }

  std::cerr << "Final result: " << (actual_result == expected_result ? "PASS" : "FAIL") << std::endl;

  // Also assert in the test framework
  EXPECT_EQ(actual_result, expected_result);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  // Uncomment below lines to run specific batches for debugging
  // ::testing::GTEST_FLAG(filter) = "SemanticTestBatch.ArrayTests";
  // ::testing::GTEST_FLAG(filter) = "SemanticTestBatch.BasicTests_1_10";
  // ::testing::GTEST_FLAG(filter) = "SemanticTestBatch.SingleTest_Debug";

  return RUN_ALL_TESTS();
}