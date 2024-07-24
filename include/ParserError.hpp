#ifndef ParserError_HPP
#define ParserError_HPP

#include "Lox.hpp"
#include "Scanner.hpp"
#include <stdexcept>

class ParseError : public std::runtime_error {
public:
    ParseError(const Token& token, const std::string& message) : std::runtime_error(message) {}
};

#endif