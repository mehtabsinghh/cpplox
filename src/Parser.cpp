#include "Parser.hpp"

#include <vector>
#include "Stmt.hpp"

// Parses an expression
std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

// Parses a declaration
std::shared_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::FUN})) return function("function");
        if (match({TokenType::VAR})) return varDeclaration();
        
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

    return call();
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee) {
    std::vector<std::unique_ptr<Expr>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Cannot have more than 255 arguments.");
            }
            arguments.emplace_back(expression());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_unique<Call>(std::move(callee), paren, std::move(arguments)); 
}      

// Parses a function call expression
std::unique_ptr<Expr> Parser::call() {
    std::unique_ptr<Expr> expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(std::move(expr));
        } else {
            break;
        }
    }

    return expr;
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
std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.emplace_back(declaration());
    }
    return statements;
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match({TokenType::FOR})) return forStatement();
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::PRINT})) return printStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::LEFT_BRACE})) return std::make_unique<Block>(block());

    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    std::shared_ptr<Stmt> initializer;
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({TokenType::VAR})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    std::unique_ptr<Expr> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    std::unique_ptr<Expr> increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    std::shared_ptr<Stmt> body = statement();

    if (increment != nullptr) {
        std::vector<std::shared_ptr<Stmt>> statements;
        statements.emplace_back(body);
        statements.emplace_back(std::make_unique<Expression>(std::move(increment)));
        body = std::make_shared<Block>(statements);
    }

    if (condition == nullptr) condition = std::make_unique<Literal>(TokenType::TRUE, nullptr);
    body = std::make_shared<While>(std::move(condition), body);
    if (initializer != nullptr){
        std::vector<std::shared_ptr<Stmt>> statements;
        statements.emplace_back(initializer);
        statements.emplace_back(body);
        body = std::make_shared<Block>(statements);
    }

    return body;
}

std::shared_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    std::shared_ptr<Stmt> thenBranch = statement();
    std::shared_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_shared<If>(std::move(condition), thenBranch, elseBranch);
}

std::shared_ptr<Stmt> Parser::printStatement() {
    std::unique_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<Print>(std::move(value));
}
std::shared_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<Var>(name, std::move(initializer));
}

std::shared_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    std::shared_ptr<Stmt> body = statement();

    return std::make_shared<While>(std::move(condition), body);
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<Expression>(std::move(expr));
}

std::shared_ptr<Stmt> Parser::function(const std::string& kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");

    std::vector<Token> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (params.size() >= 255) {
                error(peek(), "Cannot have more than 255 parameters.");
            }
            params.emplace_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::shared_ptr<Stmt>> body = block();
    return std::make_unique<Function>(name, params, body);
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
    std::vector<std::shared_ptr<Stmt>> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.emplace_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> expr = logicalOr();

    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment();

        if (dynamic_cast<Variable*>(expr.get())) {
            return std::make_unique<Assign>(dynamic_cast<Variable*>(expr.release())->name, std::move(value));
        }

        throw error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logicalOr() {
    std::unique_ptr<Expr> expr = logicalAnd();

    while (match({TokenType::OR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = logicalAnd();
        expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logicalAnd() {
    std::unique_ptr<Expr> expr = equality();

    while (match({TokenType::AND})) {
        Token op = previous();
        std::unique_ptr<Expr> right = equality();
        expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
    }

    return expr;
}