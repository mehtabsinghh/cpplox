#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "TokenInfo.hpp"
#include <string>
#include <memory>

class Token{
    const TokenType type;
    const std::string lexeme;
    std::shared_ptr<void> literal;
    const int line;
public:
    Token(TokenType type, const std::string& lexeme, std::shared_ptr<void> literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}
    
    std::string toString() const;
};

#endif