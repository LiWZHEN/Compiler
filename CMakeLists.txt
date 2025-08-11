cmake_minimum_required(VERSION 3.28)
project(Compiler)

set(CMAKE_CXX_STANDARD 20)

include_directories(lexer/include parser/include)

add_executable(lexer lexer/lexer_test.cpp
    lexer/src/token.cpp lexer/src/tokenizer.cpp
    lexer/src/regex_matcher.cpp)

add_executable(parser parser/parser_test.cpp
    lexer/src/token.cpp lexer/src/tokenizer.cpp
    lexer/src/regex_matcher.cpp
    parser/src/builder.cpp parser/src/node.cpp
    parser/src/item.cpp parser/src/module.cpp
    parser/src/functions.cpp)