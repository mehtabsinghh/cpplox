#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Expr.hpp"

template<typename T>
T* get_if(const std::shared_ptr<void>& ptr) {
    return std::dynamic_pointer_cast<T>(ptr).get();
}

class Interpreter : public ExprVisitor {
public:
    void visitBinary (const Binary& expr) override;
    void visitGrouping (const Grouping& expr) override;
    void visitLiteral (const Literal& expr) override;
    void visitUnary (const Unary& expr) override;
private:
    std::shared_ptr<void> result;
    std::shared_ptr<void> evaluate(const Expr& expr);
    bool isTruthy(const std::shared_ptr<void>& object);
    bool isEqual(const std::shared_ptr<void>& a, const std::shared_ptr<void>& b);
};






#endif