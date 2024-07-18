#include "Token.hpp"
#include "TokenInfo.hpp"

std::string Token::toString() const {
    std::string result = "Type: " + TOKENINFO_HPP::getTypeString(type) + ", Literal: ";
    if (literal == nullptr) {
        result += "null";
    } else {
        result += *static_cast<std::string*>(literal.get());
    };
    return result;
}