#ifndef Parser_HPP
#define Parser_HPP

#include "Token.hpp"
#include "Expr.hpp"
#include "ParserError.hpp"
#include "Stmt.hpp"

class Parser {
    const std::vector<Token> tokens;
    int current = 0;

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
    std::vector<std::shared_ptr<Stmt>> parse();

private:
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    bool match(std::vector<TokenType> types);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
    Token consume(TokenType type, const std::string& message);
    ParseError error(Token token, const std::string& message);
    void synchronize();
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> printStatement();
    std::shared_ptr<Stmt> expressionStatement();
    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> varDeclaration();
    std::unique_ptr<Expr> assignment();
    std::vector<std::shared_ptr<Stmt>> block();

};

#endif