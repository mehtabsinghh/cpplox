#include "Environment.hpp"

void Environment::define(const std::string& name, const std::pair<std::shared_ptr<void>, TokenType>& value) {
    values[name] = value;
}

std::pair<std::shared_ptr<void>, TokenType> Environment::get(const Token& name) {
    if (values.find(name.getLexeme()) != values.end()) {
        return values[name.getLexeme()];
    }

    throw std::runtime_error("Undefined variable '" + name.getLexeme() + "'.");
}

std::pair<std::shared_ptr<void>, TokenType> Environment::get(const std::string& name) {
    if (values.find(name) != values.end()) {
        return values[name];
    }

    throw std::runtime_error("Undefined variable '" + name + "'.");
}

void Environment::assign(const Token& name, const std::pair<std::shared_ptr<void>, TokenType>& value) {
    if (values.find(name.getLexeme()) != values.end()) {
        values[name.getLexeme()] = value;
        return;
    }

    throw std::runtime_error("Undefined variable '" + name.getLexeme() + "'.");
}