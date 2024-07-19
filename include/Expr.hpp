#ifndef Expr_HPP
#define Expr_HPP

#include <memory>
#include "Token.hpp"

class Expr {
public:
    virtual ~Expr() = default;
};

class Binary : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
};

class Grouping : public Expr {
public:
    Expr expression;

    Grouping(Expr expression)
        : expression(expression) {}
};

class Literal : public Expr {
public:
    std::shared_ptr<void> value;

    Literal(std::shared_ptr<void> value)
        : value(value) {}
};

class Unary : public Expr {
public:
    Token op;
    Expr right;

    Unary(Token op, Expr right)
        : op(op), right(right) {}
};

#endif
