#ifndef LOX_FUNCTION_HPP
#define LOX_FUNCTION_HPP
#include "LoxCallable.hpp"

class LoxFunction : public LoxCallable {
    const std::unique_ptr<Function> declaration;
    const std::shared_ptr<Environment> closure;
public:
    LoxFunction(std::unique_ptr<Function> declaration, std::shared_ptr<Environment> closure)
        : declaration(std::move(declaration)), closure(closure) {}
    int arity() override;
    std::pair<std::shared_ptr<void>, TokenType> call(Interpreter& interpreter, std::vector<std::pair<std::shared_ptr<void>, TokenType>> arguments) override;
    std::string toString() const override;
};

#endif