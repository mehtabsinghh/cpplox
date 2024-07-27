#ifndef Stmt_HPP
#define Stmt_HPP

#include <memory>
#include "Token.hpp"

class Block ;
class Expression ;
class Print ;
class Var ;

class StmtVisitor {
public:
    virtual void visitBlock (const Block & Stmt) = 0;
    virtual void visitExpression (const Expression & Stmt) = 0;
    virtual void visitPrint (const Print & Stmt) = 0;
    virtual void visitVar (const Var & Stmt) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

class Block  : public Stmt {
public:
    std::vector<Stmt> statements;

    Block (std::vector<Stmt> statements)
        : statements(statements) {}

    void accept(StmtVisitor& visitor) const override {
        return visitor.visitBlock (*this);
    }
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

class Var  : public Stmt {
public:
    Token name;
    std::unique_ptr<Expr> initializer;

    Var (Token name, std::unique_ptr<Expr> initializer)
        : name(name), initializer(std::move(initializer)) {}

    void accept(StmtVisitor& visitor) const override {
        return visitor.visitVar (*this);
    }
};

#endif
