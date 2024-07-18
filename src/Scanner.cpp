#include "Scanner.hpp"
#include "Lox.hpp"

const std::unordered_map<std::string, TokenType> Scanner::keywords = {
    {"and",    TokenType::AND},
    {"class",  TokenType::CLASS},
    {"else",   TokenType::ELSE},
    {"false",  TokenType::FALSE},
    {"for",    TokenType::FOR},
    {"fun",    TokenType::FUN},
    {"if",     TokenType::IF},
    {"nil",    TokenType::NIL},
    {"or",     TokenType::OR},
    {"print",  TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super",  TokenType::SUPER},
    {"this",   TokenType::THIS},
    {"true",   TokenType::TRUE},
    {"var",    TokenType::VAR},
    {"while",  TokenType::WHILE}
};

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        // Loops through the source code and scans each token
        start = current;
        scanToken();
    }

    // Adds an end of file token to the end of the token list
    Token endToken(TokenType::END_OF_FILE, "", nullptr, line);
    tokens.emplace_back(endToken);
    return tokens;
}

void Scanner::identifier() {
    // Scans an identifier and adds it to the token list
    while (isAlphaNumeric(peek())) advance();
    std::string text = source.substr(start, current);
    TokenType type = keywords.find(text) != keywords.end() ? keywords.at(text) : TokenType::IDENTIFIER;
    addToken(type);
}

bool Scanner::isAtEnd() const {
    return current >= source.length();
}
char Scanner::advance() {
    // Increments the current character and returns the previous character
    current++;
    return source[current - 1];
}

void Scanner::scanToken() {
    // Gets the current character
    char c = advance();

    // Checks for the type of token
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL :TokenType::GREATER);
            break;
        case '/':
            if (match('/')) {
                // If the current character is a comment, then ignore the rest of the line
                while (peek() != '\n' && !isAtEnd()) advance();
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            line++;
            break;
        case '"': string(); break;
        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                // If the character is not a token, then it is a literal
                Lox::error(line, "Unexpected character.");
            }
            break;
    }
}

void Scanner::addToken(TokenType type) {
    addToken(type, nullptr);
}

void Scanner::addToken(TokenType type, std::shared_ptr<void> literal) {
    // Adds a token to the token list
    std::string text = source.substr(start, current - start);
    Token token(type, text, literal, line);
    tokens.emplace_back(token);
}

bool Scanner::match(char expected) {
    // Checks if the current character matches the expected character
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
}

char Scanner::peek() const{
    // Returns the current character
    if (isAtEnd()) return '\0';
    return source[current];
}

void Scanner::string() {
    // Scans a string literal
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        Lox::error(line, "Unterminated string.");
        return;
    }

    // The closing "
    advance();

    // Trim the surrounding quotes
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, std::make_shared<std::string>(value));
}

bool Scanner::isDigit(const char c) const {
    // Checks if the character is a digit
    return c >= '0' && c <= '9';
}

void Scanner::number() {
    // Scans a number literal
    while (isDigit(peek())) advance();

    // Look for a fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) advance();
    }

    addToken(TokenType::NUMBER, std::make_shared<double>(std::stod(source.substr(start, current - start))));
}

char Scanner::peekNext() const {
    // Returns the next character
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Scanner::isAlpha(const char c) const {
    // Checks if the character is an alphabet character
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Scanner::isAlphaNumeric(char c) const {
    // Checks if the character is an alphabet character or a digit
    return isAlpha(c) || isDigit(c);
}