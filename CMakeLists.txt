cmake_minimum_required(VERSION 3.12)
project(cpplox)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Add your source files here
set(SOURCES
    src/main.cpp
    src/Lox.cpp 
    src/Scanner.cpp 
    src/Token.cpp
    src/AstPrinter.cpp
    src/Parser.cpp
    src/Interpreter.cpp
    src/Environment.cpp
    # Add more source files here if needed
)
add_executable(cpplox ${SOURCES})
