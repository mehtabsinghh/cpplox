#ifndef LOX_HPP
#define LOX_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "Token.hpp"
#include "Scanner.hpp"


class Lox {
    static bool hadError;
public:
    static void error(int line, const std::string& message);
    static void runFile(const std::string& path);
    static void runPrompt();
    
private:
    static void run(const std::string& source);
    static void report(int line, const std::string& where, const std::string& message);
    
};

#endif