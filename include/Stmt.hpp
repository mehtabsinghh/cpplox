#ifndef Stmt_HPP
#define Stmt_HPP

#include <memory>
#include "Token.hpp"

class Block ;
class Expression ;
class Function ;
class If ;
class Print ;
class Return ;
class Var ;
class While ;

class StmtVisitor {
public:
    virtual void visitBlock (const Block & Stmt) = 0;
    virtual void visitExpression (const Expression & Stmt) = 0;
    virtual void visitFunction (const Function & Stmt) = 0;
    virtual void visitIf (const If & Stmt) = 0;
    virtual void visitPrint (const Print & Stmt) = 0;
    virtual void visitReturn (const Return & Stmt) = 0;
    virtual void visitVar (const Var & Stmt) = 0;
    virtual void visitWhile (const While & Stmt) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

class Block  : public Stmt {
public:
    std::vector<std::shared_ptr<Stmt>> statements;

    Block (std::vector<std::shared_ptr<Stmt>> statements)
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

class Function  : public Stmt {
public:
    Token name;
    std::vector<Token> params;
    std::vector<std::shared_ptr<Stmt>> body;

    Function (Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
        : name(name), params(params), body(body) {}

    void accept(StmtVisitor& visitor) const override {
        return visitor.visitFunction (*this);
    }
};

class If  : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;

    If (std::unique_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(thenBranch), elseBranch(elseBranch) {}

    void accept(StmtVisitor& visitor) const override {
        return visitor.visitIf (*this);
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

class Return  : public Stmt {
public:
    Token keyword;
    std::unique_ptr<Expr> value;

    Return (Token keyword, std::unique_ptr<Expr> value)
        : keyword(keyword), value(std::move(value)) {}

    void accept(StmtVisitor& visitor) const override {
        return visitor.visitReturn (*this);
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

class While  : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;

    While (std::unique_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(std::move(condition)), body(body) {}

    void accept(StmtVisitor& visitor) const override {
        return visitor.visitWhile (*this);
    }
};

#endif
