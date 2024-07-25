#ifndef Stmt_HPP
#define Stmt_HPP

#include <memory>
#include "Token.hpp"
#include "Expr.hpp"

class Expression ;
class Print ;

class StmtVisitor {
public:
    virtual void visitExpression (const Expression & Stmt) = 0;
    virtual void visitPrint (const Print & Stmt) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

class Expression  : public Stmt {
public:
    std::unique_ptr<Expr> expression;

    Expression (std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        return visitor.visitExpression (*this);
    }
};

class Print  : public Stmt {
public:
    std::unique_ptr<Expr> expression;

    Print (std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        return visitor.visitPrint (*this);
    }
};

#endif
