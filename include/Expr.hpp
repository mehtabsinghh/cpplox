#ifndef Expr_HPP
#define Expr_HPP

#include <memory>
#include "Token.hpp"

template<typename R>
class ExprVisitor {
public:
    virtual R visitBinary (const Binary & Expr) = 0;
    virtual R visitGrouping (const Grouping & Expr) = 0;
    virtual R visitLiteral (const Literal & Expr) = 0;
    virtual R visitUnary (const Unary & Expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept() const = 0;
};

class Binary  : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary (std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    template<typename R>
    R accept(ExprVisitor<R>& visitor) const {
        return visitor.visitBinary (*this);
    }
};

class Grouping  : public Expr {
public:
    std::unique_ptr<Expr> expression;

    Grouping (std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    template<typename R>
    R accept(ExprVisitor<R>& visitor) const {
        return visitor.visitGrouping (*this);
    }
};

class Literal  : public Expr {
public:
    std::shared_ptr<void> value;

    Literal (std::shared_ptr<void> value)
        : value(value) {}
    template<typename R>
    R accept(ExprVisitor<R>& visitor) const {
        return visitor.visitLiteral (*this);
    }
};

class Unary  : public Expr {
public:
    Token op;
    std::unique_ptr<Expr> right;

    Unary (Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}
    template<typename R>
    R accept(ExprVisitor<R>& visitor) const {
        return visitor.visitUnary (*this);
    }
};

#endif
