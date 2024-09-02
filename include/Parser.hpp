#ifndef Parser_HPP
#define Parser_HPP

#include <vector>
#include <memory>
#include <string>
#include "Token.hpp"
#include "Expr.hpp"
#include "ParserError.hpp"
#include "Stmt.hpp"

/**
 * @class Parser
 * @brief Responsible for parsing tokens into expressions and statements.
 * 
 * This class takes a vector of tokens produced by the scanner and
 * parses them into a syntax tree, producing expressions and statements
 * that can be executed by an interpreter or compiled.
 */
class Parser {
    const std::vector<Token> tokens; ///< List of tokens to parse.
    int current = 0; ///< Current position in the list of tokens.

public:
    /**
     * @brief Constructs a Parser with a vector of tokens.
     * 
     * @param tokens Vector of tokens to be parsed.
     */
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    /**
     * @brief Parses the token list into a list of statements.
     * 
     * @return A vector of parsed statements.
     */
    std::vector<std::shared_ptr<Stmt>> parse();

private:
    // High-level parsing functions

    /**
     * @brief These methods parse different types of statements.
     * 
     * The methods return a pointer to a parsed statement, handling various
     * statement types like print statements, expression statements, variable 
     * declarations, control structures (if, while, for), and function declarations.
     * 
     * @return A pointer to the parsed statement.
     */
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> declaration();
    std::vector<std::shared_ptr<Stmt>> block();
    std::shared_ptr<Stmt> function(const std::string& kind);
    std::shared_ptr<Stmt> returnStatement();
    std::shared_ptr<Stmt> ifStatement();
    std::shared_ptr<Stmt> whileStatement();
    std::shared_ptr<Stmt> forStatement();
    std::shared_ptr<Stmt> printStatement();
    std::shared_ptr<Stmt> expressionStatement();
    std::shared_ptr<Stmt> varDeclaration();

    // Expression parsing functions

    /**
     * @brief These methods parse different types of expressions.
     * 
     * The methods return a unique pointer to a parsed expression, handling
     * operations like assignments, logical expressions, equality checks, comparisons,
     * arithmetic operations, unary operations, and primary expressions.
     * 
     * @return A unique pointer to the parsed expression.
     */
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> logicalOr();
    std::unique_ptr<Expr> logicalAnd();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee);

    // Utility functions

    /**
     * @brief Utility functions used in parsing.
     * 
     * These methods help in token management, error handling, and 
     * parser state management, facilitating the parsing process.
     */
    Token consume(TokenType type, const std::string& message);
    bool match(std::vector<TokenType> types);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    ParseError error(Token token, const std::string& message);
    void synchronize();
};

#endif
