#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "Token.hpp"
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>
#include <iostream>

class Environment {
    std::unordered_map<std::string, std::pair<std::shared_ptr<void>, TokenType>> values;
public:
    std::shared_ptr<Environment> enclosing;
    Environment() : enclosing(nullptr) {}
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}
    void define(const std::string& name, const std::pair<std::shared_ptr<void>, TokenType>& value);
    std::pair<std::shared_ptr<void>, TokenType> get(const Token& name);
    std::pair<std::shared_ptr<void>, TokenType> get(const std::string& name);
    void assign(const Token& name, const std::pair<std::shared_ptr<void>, TokenType>& value);
};


#endif