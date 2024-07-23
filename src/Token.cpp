#include "Token.hpp"
#include "TokenInfo.hpp"

std::string Token::toString() const {
    std::string result = "Type: " + TOKENINFO_HPP::getTypeString(type) + ", Literal: ";
    if (literal == nullptr) {
        result += "null";
    } else {
        result += *static_cast<std::string*>(literal.get());
    };
    result += '\n';
    return result;
}

std::string Token::getLexeme() const {
    return lexeme;
}

TokenType Token::getType() const {
    return type;
}

std::shared_ptr<void> Token::getLiteral() const {
    return literal;
}

int Token::getLine() const {
    return line;
}