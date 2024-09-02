#include "LoxFunction.hpp"
#include "ReturnException.hpp"

int LoxFunction::arity() {
    return declaration->params.size();
}

std::pair<std::shared_ptr<void>, TokenType> LoxFunction::call(Interpreter& interpreter, std::vector<std::pair<std::shared_ptr<void>, TokenType>> arguments) {
    // Create a new environment for the function call
    auto environment = std::make_shared<Environment>(closure);

    // Define the function parameters in the new environment
    for (int i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params[i].getLexeme(), arguments[i]);
    }

    try {
        // Execute the function body in the new environment
        interpreter.executeBlock(declaration->body, environment);
    } catch (const ReturnException& e) {
        // Return the value from the return statement
        return {e.value, e.type};
    }

    // If there was no return statement, return nil
    return {nullptr, TokenType::NIL};
}

std::string LoxFunction::toString() const {
    return "<fn " + declaration->name.getLexeme() + ">";
}