#include "Environment.hpp"

void Environment::define(const std::string& name, const std::pair<std::shared_ptr<void>, TokenType>& value) {
    // Define the variable in the environment
    values[name] = value;
}

std::pair<std::shared_ptr<void>, TokenType> Environment::get(const Token& name) {
    // Look up the variable in the environment
    if (values.find(name.getLexeme()) != values.end()) {
        return values[name.getLexeme()];
    }

    // If the variable is not found, check the enclosing environment
    if (enclosing != nullptr) {
        return (*enclosing).get(name);
    }

    // If the variable is not found in the enclosing environment, throw an error
    throw std::runtime_error("Undefined variable '" + name.getLexeme() + "'.");
}


void Environment::assign(const Token& name, const std::pair<std::shared_ptr<void>, TokenType>& value) {
    // Assign a new value to an existing variable in the environment
    if (values.find(name.getLexeme()) != values.end()) {
        values[name.getLexeme()] = value;
        return;
    }

    // If the variable is not found, check the enclosing environment
    if (enclosing != nullptr) {
        (*enclosing).assign(name, value);
        return;
    }

    // If the variable is not found in the enclosing environment, throw an error
    throw std::runtime_error("Undefined variable '" + name.getLexeme() + "'.");
}