cmake_minimum_required(VERSION 3.23)
project(translator)

set(CMAKE_CXX_STANDARD 20)

add_executable(translator main.cpp SyntaxAnalyzer.cpp SyntaxAnalyzer.h LexicalAnalyzer.cpp LexicalAnalyzer.h)
