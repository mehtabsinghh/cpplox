#ifndef RUNTIMEERROR_HPP
#define RUNTIMEERROR_HPP

#include "Token.hpp"
#include <string>
#include <stdexcept>

class RuntimeError : public std::runtime_error {
public:
    const Token token;
    RuntimeError(const Token token, const std::string& message) : token(token), std::runtime_error(message) {}
};

#endif