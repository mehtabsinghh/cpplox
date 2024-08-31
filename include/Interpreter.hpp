#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Environment.hpp"

class Interpreter : public ExprVisitor, StmtVisitor {
public:
    const std::shared_ptr<Environment> globals = std::make_shared<Environment>();
    Interpreter();
    void visitBinary (const Binary& expr) override;
    void visitGrouping (const Grouping& expr) override;
    void visitLiteral (const Literal& expr) override;
    void visitUnary (const Unary& expr) override;
    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);
    void visitExpression(const Expression& stmt) override;
    void visitFunction(const Function& stmt) override;
    void visitPrint(const Print& stmt) override;
    void visitVar(const Var& stmt) override;
    void visitVariable(const Variable& expr) override;
    void visitAssign(const Assign& stmt) override;
    void visitBlock(const Block& stmt) override;
    void visitIf(const If& stmt) override;
    void visitLogical(const Logical& expr) override;
    void visitWhile(const While& stmt) override;
    void visitCall(const Call& expr) override;
    void visitReturn(const Return& stmt) override;
    void executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> environment);
    std::shared_ptr<void>& getResult();
    TokenType getType();
private:
    // Global environment for the interpreter
    
    std::shared_ptr<Environment> environment = globals;
    std::shared_ptr<void> result;
    TokenType type;
    std::shared_ptr<void> evaluate(const Expr& expr);
    bool isTruthy(const std::shared_ptr<void>& object, TokenType type);
    bool isEqual(const std::shared_ptr<void>& a, const std::shared_ptr<void>& b, TokenType aType, TokenType bType);
    void checkNumberOperand(const Token op, const TokenType type);
    void checkNumberOperands(const Token op, const TokenType leftType, const TokenType rightType);
    std::string stringify(const std::shared_ptr<void>& object, TokenType type);
    void execute(const Stmt& stmt);

};






#endif