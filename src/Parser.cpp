#include "Parser.hpp"

#include <vector>
#include "Stmt.hpp"

// Parses an expression
std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

// Parses a declaration
std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::VAR})) {
            return varDeclaration();
        }
        return statement();
    } catch (ParseError error) {
        synchronize();
        return nullptr;
    }
}

// Parses an equality expression
std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();

    // Check for equality and inequality operators
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Checks if the current token is one of the given types
bool Parser::match(std::vector<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

// Checks if type of current token is equal to the given type
bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().getType() == type;
}

// Advances the current token and returns the previous token
Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

// Checks if the current token is the end of the file
bool Parser::isAtEnd() {
    return peek().getType() == TokenType::END_OF_FILE;
}


// Returns the current token
Token Parser::peek() {
    return tokens[current];
}

// Returns the previous token
Token Parser::previous() {
    return tokens[current - 1];
}

// Parses a comparison expression
std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();

    // Check for comparison operators
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}


// Parses a term expression
std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    // Check for subtraction and addition operators
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Parses a factor expression
std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();

    // Check for division and multiplication operators§
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Parses a unary expression
std::unique_ptr<Expr> Parser::unary() {
    // Check for unary operators
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<Unary>(op, std::move(right));
    }

    return primary();
}

// Parses a primary expression
std::unique_ptr<Expr> Parser::primary() {
    auto nilptr = std::shared_ptr<void>();

    // Check for boolean literals
    if (match({TokenType::FALSE})) {
        return std::make_unique<Literal>(TokenType::FALSE, nilptr);
    } else if (match({TokenType::TRUE})) {
        return std::make_unique<Literal>(TokenType::TRUE, nilptr);
    }

    // Check for nil literal
    else if (match({TokenType::NIL})) {
        auto nilptr = std::shared_ptr<void>();
        return std::make_unique<Literal>(TokenType::NIL, nilptr);
    }

    // Check for number and string literals
    else if (match({TokenType::NUMBER, TokenType::STRING})) {
        auto prevLiteral = previous().getLiteral();
        return std::make_unique<Literal>(previous().getType(), prevLiteral);
    }

    else if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<Variable>(previous());
    }

    // Check for grouped expressions
    else if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }

    // If none of the above cases match, throw an error
    throw error(peek(), "Expect expression.");
}

// Consumes the current token if it is of the given type, otherwise throws an error
Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

// Throws a parse error with the given message
ParseError Parser::error(Token token, const std::string& message) {
    Lox::error(token, message);
    return ParseError(token, message);
}

// Synchronizes the parser after an error
void Parser::synchronize() {
    advance();

    // Skip tokens until a statement boundary is reached
    while (!isAtEnd()) {
        // Skip tokens until a semicolon is reached
        if (previous().getType() == TokenType::SEMICOLON) return;

        // Check for statement boundaries
        switch (peek().getType()) {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}

// Parses the input tokens and returns the resulting expression
std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.emplace_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::PRINT})) {
        return printStatement();
    }

    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::printStatement() {
    std::unique_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<Print>(std::move(value));
}
std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<Var>(name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<Expression>(std::move(expr));
}