cmake_minimum_required(VERSION 3.28)
project(Compiler)

set(CMAKE_CXX_STANDARD 20)

include_directories(lexer/include parser/include semantic/include
    IR/include front_end_entry/include)

find_package(GTest REQUIRED)
add_executable(semantic_tests test/semantic_test.cpp
    lexer/src/token.cpp lexer/src/tokenizer.cpp
    lexer/src/regex_matcher.cpp parser/src/builder.cpp
    parser/src/enumerations.cpp parser/src/expression.cpp
    parser/src/function_parameters.cpp
    parser/src/functions.cpp parser/src/item.cpp
    parser/src/leaf_node.cpp parser/src/node.cpp
    parser/src/path.cpp parser/src/pattern.cpp
    parser/src/statements.cpp parser/src/structs.cpp
    parser/src/trait.cpp parser/src/type.cpp
    semantic/src/scope.cpp semantic/src/value_type.cpp
    semantic/src/visitor_manager.cpp)

add_executable(run_front_end
    lexer/src/token.cpp lexer/src/tokenizer.cpp
    lexer/src/regex_matcher.cpp parser/src/builder.cpp
    parser/src/enumerations.cpp parser/src/expression.cpp
    parser/src/function_parameters.cpp
    parser/src/functions.cpp parser/src/item.cpp
    parser/src/leaf_node.cpp parser/src/node.cpp
    parser/src/path.cpp parser/src/pattern.cpp
    parser/src/statements.cpp parser/src/structs.cpp
    parser/src/trait.cpp parser/src/type.cpp
    semantic/src/scope.cpp semantic/src/value_type.cpp
    semantic/src/visitor_manager.cpp IR/src/IR_generator.cpp
    front_end_entry/src/check_and_generate_IR.cpp)

target_link_libraries(semantic_tests GTest::GTest GTest::Main)

add_test(NAME tests COMMAND tests)