#ifndef CHECK_AND_GENERATE_IR_H
#define CHECK_AND_GENERATE_IR_H

#include <string>
#include "IR_generator.h"
#include "visitor_manager.h"

class FrontEndRunner {
public:
  explicit FrontEndRunner(const std::string &code) : code_(code) {}
  void Run();
private:
  const std::string &code_;
  VisitorManager semantic_checker_;
  IRVisitor IR_generator_;
};

#endif