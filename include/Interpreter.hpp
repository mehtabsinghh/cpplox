#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Expr.hpp"

template<typename T>
T* get_if(const std::shared_ptr<void>& ptr) {
    if (ptr && typeid(T) == typeid(*static_cast<T*>(ptr.get()))) {
        return static_cast<T*>(ptr.get());
    }
    return nullptr;
}

class Interpreter : public ExprVisitor {
public:
    void visitBinary (const Binary& expr) override;
    void visitGrouping (const Grouping& expr) override;
    void visitLiteral (const Literal& expr) override;
    void visitUnary (const Unary& expr) override;
    void interpret(std::unique_ptr<Expr>& expr);
private:
    std::shared_ptr<void> result;
    std::shared_ptr<void> evaluate(const Expr& expr);
    bool isTruthy(const std::shared_ptr<void>& object);
    bool isEqual(const std::shared_ptr<void>& a, const std::shared_ptr<void>& b);
    void checkNumberOperand(const Token op, const std::shared_ptr<void>& operand);
    void checkNumberOperands(const Token op, const std::shared_ptr<void>& left, const std::shared_ptr<void>& right);
    std::string stringify(const std::shared_ptr<void>& object);
};






#endif