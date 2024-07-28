#ifndef LOXCALLABLE_HPP
#define LOXCALLABLE_HPP

#include <vector>
#include <memory>
#include "Token.hpp"
#include <utility>
#include "Interpreter.hpp"

class LoxCallable {
public:
    virtual int arity() = 0;
    virtual std::pair<std::shared_ptr<void>, TokenType> call(Interpreter& interpreter, std::vector<std::pair<std::shared_ptr<void>, TokenType>> arguments) = 0;
    virtual std::string toString() const = 0;
};

#endif