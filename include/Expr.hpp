#ifndef Expr_HPP
#define Expr_HPP

#include <memory>
#include "Token.hpp"

class Assign ;
class Binary ;
class Call ;
class Grouping ;
class Literal ;
class Logical ;
class Unary ;
class Variable ;

class ExprVisitor {
public:
    virtual void visitAssign (const Assign & Expr) = 0;
    virtual void visitBinary (const Binary & Expr) = 0;
    virtual void visitCall (const Call & Expr) = 0;
    virtual void visitGrouping (const Grouping & Expr) = 0;
    virtual void visitLiteral (const Literal & Expr) = 0;
    virtual void visitLogical (const Logical & Expr) = 0;
    virtual void visitUnary (const Unary & Expr) = 0;
    virtual void visitVariable (const Variable & Expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

class Assign  : public Expr {
public:
    Token name;
    std::unique_ptr<Expr> value;

    Assign (Token name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}

    void accept(ExprVisitor& visitor) const override {
        return visitor.visitAssign (*this);
    }
};

class Binary  : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary (std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        return visitor.visitBinary (*this);
    }
};

class Call  : public Expr {
public:
    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<Expr>> arguments;

    Call (std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}

    void accept(ExprVisitor& visitor) const override {
        return visitor.visitCall (*this);
    }
};

class Grouping  : public Expr {
public:
    std::unique_ptr<Expr> expression;

    Grouping (std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(ExprVisitor& visitor) const override {
        return visitor.visitGrouping (*this);
    }
};

class Literal  : public Expr {
public:
    TokenType type;
    std::shared_ptr<void> value;

    Literal (TokenType type, std::shared_ptr<void> value)
        : type(type), value(value) {}

    void accept(ExprVisitor& visitor) const override {
        return visitor.visitLiteral (*this);
    }
};

class Logical  : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Logical (std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        return visitor.visitLogical (*this);
    }
};

class Unary  : public Expr {
public:
    Token op;
    std::unique_ptr<Expr> right;

    Unary (Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        return visitor.visitUnary (*this);
    }
};

class Variable  : public Expr {
public:
    Token name;

    Variable (Token name)
        : name(name) {}

    void accept(ExprVisitor& visitor) const override {
        return visitor.visitVariable (*this);
    }
};

#endif
