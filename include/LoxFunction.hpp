#include "LoxCallable.hpp"


class LoxFunction : public LoxCallable {
    const Function declaration;
public:
    LoxFunction(const Function& declaration) : declaration(declaration) {}
    int arity() override;
    std::pair<std::shared_ptr<void>, TokenType> call(Interpreter& interpreter, std::vector<std::pair<std::shared_ptr<void>, TokenType>> arguments) override;
    std::string toString() const override;
};