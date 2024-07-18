#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "Token.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Scanner {
private:
    const std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
    static const std::unordered_map<std::string, TokenType> keywords;
    
public:
    Scanner(const std::string& source) : source(source) {}
    std::vector<Token> scanTokens();
    bool isAtEnd() const;
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, std::shared_ptr<void> literal);
    bool match(char expected);
    char peek() const;
    void string();
    bool isDigit(const char c) const;
    void number();
    char peekNext() const;
    void identifier();
    bool isAlpha(const char c) const;
    bool isAlphaNumeric(char c) const;
    
};

#endif