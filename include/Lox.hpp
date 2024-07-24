#ifndef LOX_HPP
#define LOX_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "Token.hpp"
#include "Scanner.hpp"
#include "Parser.hpp"
#include "AstPrinter.hpp"
#include "RuntimeError.hpp"
#include "Interpreter.hpp"

class Lox {
    static bool hadError;
    static bool hadRuntimeError;
public:
    static void error(int line, const std::string& message);
    static void error(Token token, const std::string& message);
    static void runFile(const std::string& path);
    static void runPrompt();
    static void runtimeError(RuntimeError error);
    
private:
    static void run(const std::string& source);
    static void report(int line, const std::string& where, const std::string& message);
    
};

#endif