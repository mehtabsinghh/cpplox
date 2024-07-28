#ifndef RETURN_HPP
#define RETURN_HPP

#include "Token.hpp"
#include <stdexcept>
#include <memory>


class ReturnException : public std::runtime_error {
public:
    ReturnException(std::shared_ptr<void> &value, TokenType type)
        : std::runtime_error("Return statement"), value(value), type(type) {}

    std::shared_ptr<void> value;
    TokenType type;
};

#endif // !RETURN_HPP