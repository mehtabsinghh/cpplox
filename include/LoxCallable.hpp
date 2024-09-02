#ifndef LOXCALLABLE_HPP
#define LOXCALLABLE_HPP

#include <vector>
#include <memory>
#include "Token.hpp"
#include <utility>
#include "Interpreter.hpp"

/**
 * @class LoxCallable
 * @brief Abstract class for Lox callables
 * 
 * The LoxCallable class is an abstract class for Lox callables, which are
 * objects that can be called like functions in Lox code. It provides a
 * method for getting the arity of the callable, a method for calling the
 * callable with a list of arguments, and a method for converting the
 * callable to a string.
 */
class LoxCallable {
public:
    virtual int arity() = 0;
    virtual std::pair<std::shared_ptr<void>, TokenType> call(Interpreter& interpreter, std::vector<std::pair<std::shared_ptr<void>, TokenType>> arguments) = 0;
    virtual std::string toString() const = 0;
};

#endif