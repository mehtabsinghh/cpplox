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

/**
 * @class Lox
 * @brief Main class for the Lox interpreter
 * 
 * The Lox class is the main class for the Lox interpreter. It provides methods
 * for running Lox code from a file or from the command line, as well as methods
 * for reporting errors and exceptions.
 */
class Lox {
    static bool hadError; // Flag to indicate if an error occurred
    static bool hadRuntimeError; // Flag to indicate if a runtime error occurred
public:
    /**
     * @brief Runs the Lox interpreter with the given source code
     * 
     * @param source The source code to run
     */
    static void runFile(const std::string& path);

    /**
     * @brief Runs the Lox interpreter in interactive mode
     */
    static void runPrompt();

    /**
     * @brief Reports a syntax error
     * 
     * @param line The line number where the error occurred
     * @param message The error message
     */
    static void error(int line, const std::string& message);

    /**
     * @brief Reports a syntax error
     * 
     * @param token The token where the error occurred
     * @param message The error message
     */
    static void error(Token token, const std::string& message);

    /**
     * @brief Reports a runtime error
     * 
     * @param error The RuntimeError object
     */
    static void runtimeError(RuntimeError error);
    
private:
    /**
     * @brief Runs the Lox interpreter with the given source code
     * 
     * @param source The source code to run
     */
    static void run(const std::string& source);

    /**
     * @brief Reads the source code from a file
     * 
     * @param path The path to the file
     * @return The source code read from the file
     */
    static void report(int line, const std::string& where, const std::string& message);
    
};

#endif // LOX_HPP