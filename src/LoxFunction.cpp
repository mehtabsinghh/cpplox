#include "LoxFunction.hpp"
#include "ReturnException.hpp"

int LoxFunction::arity() {
    return declaration->params.size();
}

std::pair<std::shared_ptr<void>, TokenType> LoxFunction::call(Interpreter& interpreter, std::vector<std::pair<std::shared_ptr<void>, TokenType>> arguments) {
    auto environment = std::make_shared<Environment>(closure);
    for (int i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params[i].getLexeme(), arguments[i]);
    }

    try {
        interpreter.executeBlock(declaration->body, environment);
    } catch (const ReturnException& e) {
        return {e.value, e.type};
    }
    return {nullptr, TokenType::NIL};
}

std::string LoxFunction::toString() const {
    return "<fn " + declaration->name.getLexeme() + ">";
}