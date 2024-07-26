#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "Token.hpp"
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>

class Environment {
    std::unordered_map<std::string, std::pair<std::shared_ptr<void>, TokenType>> values;
public:
    void define(const std::string& name, const std::pair<std::shared_ptr<void>, TokenType>& value) {
        values[name] = value;
    }

    std::pair<std::shared_ptr<void>, TokenType> get(const Token& name) {
        if (values.find(name.getLexeme()) != values.end()) {
            return values[name.getLexeme()];
        }

        throw std::runtime_error("Undefined variable '" + name.getLexeme() + "'.");
    }
};


#endif