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
    src/Parser.cpp
    src/Interpreter.cpp
    src/Environment.cpp
    src/LoxFunction.cpp
    # Add more source files here if needed
)

# Add the Boost.Test library
find_package(Boost 1.70.0 REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

# Add the executable for testing
add_executable(runUnitTests test/test.cpp)
target_link_libraries(runUnitTests ${Boost_LIBRARIES})

# Enable testing
enable_testing()
add_test(NAME runUnitTests COMMAND runUnitTests)

add_executable(cpplox ${SOURCES})
