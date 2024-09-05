#include "Parser.hpp"

#include <vector>
#include "Stmt.hpp"

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;

    // Parse statements until the end of the file
    while (!isAtEnd()) {
        statements.emplace_back(declaration());
    }

    return statements;
}

// Parses a declaration
std::shared_ptr<Stmt> Parser::declaration() {
    try {
        // Check for different types of declarations
        if (match({TokenType::FUN})) return function("function");
        if (match({TokenType::VAR})) return varDeclaration();
        
        return statement();
    } catch (ParseError error) {
        // If an error occurs, synchronize the parser and return nullptr
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::statement() {
    // Check for different types of statements
    if (match({TokenType::FOR})) return forStatement();
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::PRINT})) return printStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::LEFT_BRACE})) return std::make_shared<Block>(block());

    // If no other statement type matches, parse an expression statement
    return expressionStatement();
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
    std::vector<std::shared_ptr<Stmt>> statements;

    // Parse statements until the end of the block
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.emplace_back(declaration());
    }

    // Consume the closing brace
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::shared_ptr<Stmt> Parser::function(const std::string& kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");

    // Parse function parameters
    std::vector<Token> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (params.size() >= 255) {
                // Throw an error if the number of parameters exceeds 255
                error(peek(), "Cannot have more than 255 parameters.");
            }
            params.emplace_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match({TokenType::COMMA})); // Continue parsing parameters until a comma is not found
    }

    // Consume the closing parenthesis and opening brace
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");

    // Create Function statement with the parsed parameters and body
    std::vector<std::shared_ptr<Stmt>> body = block();
    return std::make_unique<Function>(name, params, body);
}

std::shared_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;

    // Parse the return value if it exists
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }

    // Consume the semicolon and return the Return statement
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_shared<Return>(keyword, std::move(value));
}

std::shared_ptr<Stmt> Parser::ifStatement() {
    // Parse the if condition and get expresison
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    // Parse the then branch and else branch
    std::shared_ptr<Stmt> thenBranch = statement();
    std::shared_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    // Create and return the If statement
    return std::make_shared<If>(std::move(condition), thenBranch, elseBranch);
}

std::shared_ptr<Stmt> Parser::whileStatement() {
    // Parse the while condition and body
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    std::shared_ptr<Stmt> body = statement();

    // Create and return the While statement
    return std::make_shared<While>(std::move(condition), body);
}

std::shared_ptr<Stmt> Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    // Parse the initializer
    std::shared_ptr<Stmt> initializer;
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({TokenType::VAR})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    // Parse the condition
    std::unique_ptr<Expr> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    // Parse the increment
    std::unique_ptr<Expr> increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    // Parse the body of the for loop
    std::shared_ptr<Stmt> body = statement();

    // Create a block statement with the increment and body
    if (increment != nullptr) {
        std::vector<std::shared_ptr<Stmt>> statements;
        statements.emplace_back(body);
        statements.emplace_back(std::make_unique<Expression>(std::move(increment)));
        body = std::make_shared<Block>(statements);
    }

    if (condition == nullptr) condition = std::make_unique<Literal>(TokenType::TRUE, nullptr); // If no condition is provided, default to true

    // Create and return the While statement
    body = std::make_shared<While>(std::move(condition), body);
    if (initializer != nullptr){
        std::vector<std::shared_ptr<Stmt>> statements;
        statements.emplace_back(initializer);
        statements.emplace_back(body);
        body = std::make_shared<Block>(statements);
    }

    return body;
}


std::shared_ptr<Stmt> Parser::printStatement() {
    // Parse the value to print and consume the semicolon
    std::unique_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<Print>(std::move(value));
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    // Parse the expression and consume the semicolon
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<Expression>(std::move(expr));
}

std::shared_ptr<Stmt> Parser::varDeclaration() {
    // Parse the variable name
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    // Parse the initializer if it exists
    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    // Consume the semicolon and return the Var statement
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<Var>(name, std::move(initializer));
}

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    // Parse the left-hand side of the assignment
    std::unique_ptr<Expr> expr = logicalOr();

    // Check for assignment operator
    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment(); // Parse the right-hand side of the assignment recursively

        if (dynamic_cast<Variable*>(expr.get())) {
            // If the left-hand side is a variable, return an Assign expression
            return std::make_unique<Assign>(dynamic_cast<Variable*>(expr.release())->name, std::move(value));
        }

        throw error(equals, "Invalid assignment target."); // Throw an error if the left-hand side is not a variable
    }

    // If no assignment operator is found, return the parsed expression
    return expr;
}

std::unique_ptr<Expr> Parser::logicalOr() {
    std::unique_ptr<Expr> expr = logicalAnd();

    // Check for logical OR operators
    while (match({TokenType::OR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = logicalAnd();
        expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logicalAnd() {
    std::unique_ptr<Expr> expr = equality();

    // Check for logical AND operators
    while (match({TokenType::AND})) {
        Token op = previous();
        std::unique_ptr<Expr> right = equality();
        expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();

    // Check for equality operators
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

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

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    // Check for addition and subtraction operators
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();

    // Check for multiplication and division operators
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    // Check for unary operators
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<Unary>(op, std::move(right));
    }

    return call();
}


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

    // Check for identifiers
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

std::unique_ptr<Expr> Parser::call() {
    // Parse the primary expression
    std::unique_ptr<Expr> expr = primary();

    // Check for function calls
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(std::move(expr));
        } else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee) {
    // Parse the arguments to the function call
    std::vector<std::unique_ptr<Expr>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Cannot have more than 255 arguments.");
            }
            arguments.emplace_back(expression());
        } while (match({TokenType::COMMA}));
    }

    // Consume the closing parenthesis and return the Call expression
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_unique<Call>(std::move(callee), paren, std::move(arguments)); 
}

Token Parser::consume(TokenType type, const std::string& message) {
    // Consumes the current token if it is of the given type, otherwise throws an error
    if (check(type)) return advance();
    throw error(peek(), message);
}

bool Parser::match(std::vector<TokenType> types) {
    // Checks if the current token is one of the given types
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    // Checks if type of current token is equal to the given type
    if (isAtEnd()) return false;
    return peek().getType() == type;
}

Token Parser::advance() {
    // Advances the current token and returns the previous token
    if (!isAtEnd()) current++;
    return previous();
}


bool Parser::isAtEnd() {
    // Checks if the current token is the end of the file
    return peek().getType() == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

ParseError Parser::error(Token token, const std::string& message) {
    Lox::error(token, message);
    return ParseError(token, message);
}

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