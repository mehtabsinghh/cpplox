#define BOOST_TEST_MODULE InterpreterTest
#include <boost/test/included/unit_test.hpp>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>


// Function to trim leading and trailing whitespace
std::string trimWhitespace(const std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r\f\v");
    auto end = str.find_last_not_of(" \t\n\r\f\v");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Function to read a file into a string
const std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not open file " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << buffer.str() << std::endl;
    return buffer.str();
}

// Function to run a file
const std::string runFile(const std::string& path) {
    const std::string command = "./cpplox " + path + " > output.txt";
    std::system(command.c_str());
    return trimWhitespace(readFile("output.txt"));
}

// Individual test cases for each Lox program
BOOST_AUTO_TEST_CASE(Test1) {
    std::string output = runFile("../test/lox_programs/test1.lox");
    std::string expectedOutput = readFile("../test/lox_programs/test1_expected.txt");
    BOOST_CHECK_EQUAL(output, expectedOutput);
}

BOOST_AUTO_TEST_CASE(Test2) {
    std::string output = runFile("../test/lox_programs/test2.lox");
    std::string expectedOutput = readFile("../test/lox_programs/test2_expected.txt");
    BOOST_CHECK_EQUAL(output, expectedOutput);
}

BOOST_AUTO_TEST_CASE(Test3) {
    std::string output = runFile("../test/lox_programs/test3.lox");
    std::string expectedOutput = readFile("../test/lox_programs/test3_expected.txt");
    BOOST_CHECK_EQUAL(output, expectedOutput);
}

BOOST_AUTO_TEST_CASE(Test4) {
    std::string output = runFile("../test/lox_programs/test4.lox");
    std::string expectedOutput = readFile("../test/lox_programs/test4_expected.txt");
    BOOST_CHECK_EQUAL(output, expectedOutput);
}

BOOST_AUTO_TEST_CASE(Test5) {
    std::string output = runFile("../test/lox_programs/test5.lox");
    std::string expectedOutput = readFile("../test/lox_programs/test5_expected.txt");
    BOOST_CHECK_EQUAL(output, expectedOutput);
}

BOOST_AUTO_TEST_CASE(Test6) {
    std::string output = runFile("../test/lox_programs/test6.lox");
    std::string expectedOutput = readFile("../test/lox_programs/test6_expected.txt");
    BOOST_CHECK_EQUAL(output, expectedOutput);
}