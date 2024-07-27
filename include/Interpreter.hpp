#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Environment.hpp"

template<typename T>
T* get_if(const std::shared_ptr<void>& ptr) {
    if (ptr && typeid(T) == typeid(*static_cast<T*>(ptr.get()))) {
        return static_cast<T*>(ptr.get());
    }
    return nullptr;
}

class Interpreter : public ExprVisitor, StmtVisitor {
    std::shared_ptr<Environment> environment = std::make_shared<Environment>();
    std::shared_ptr<void> result;
public:
    void visitBinary (const Binary& expr) override;
    void visitGrouping (const Grouping& expr) override;
    void visitLiteral (const Literal& expr) override;
    void visitUnary (const Unary& expr) override;
    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);
    void visitExpression(const Expression& stmt) override;
    void visitPrint(const Print& stmt) override;
    void visitVar(const Var& stmt) override;
    void visitVariable(const Variable& expr) override;
    void visitAssign(const Assign& stmt) override;
    void visitBlock(const Block& stmt) override;
    void executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> environment);
    std::shared_ptr<void>& getResult();
    TokenType getType();
private:
    // Global environment for the interpreter
    TokenType type;
    std::shared_ptr<void> evaluate(const Expr& expr);
    bool isTruthy(const std::shared_ptr<void>& object, TokenType type);
    bool isEqual(const std::shared_ptr<void>& a, const std::shared_ptr<void>& b, TokenType aType, TokenType bType);
    void checkNumberOperand(const Token op, const std::shared_ptr<void>& operand);
    void checkNumberOperands(const Token op, const std::shared_ptr<void>& left, const std::shared_ptr<void>& right);
    std::string stringify(const std::shared_ptr<void>& object, TokenType type);
    void execute(const Stmt& stmt);

};






#endif