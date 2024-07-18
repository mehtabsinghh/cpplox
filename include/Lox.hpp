#ifndef LOX_HPP
#define LOX_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>


class Lox {
    static bool hadError;
public:
    static void run(const std::string& source);
    static void error(int line, const std::string& message);
    
private:
    static void runFile(const std::string& path);
    static void runPrompt();
    static void run(const std::string& source);
    static void report(int line, const std::string& where, const std::string& message);
    
};

#endif