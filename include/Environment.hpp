#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "Token.hpp"
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>
#include <iostream>

/**
 * @class Environment
 * @brief Represents a collection of variables and their values
 * 
 * The Environment class is used to store variables and their values. It is 
 * implemented as a hash map that maps variable names to their values. Each 
 * environment can have an optional enclosing environment, which allows for 
 * variable scoping and resolution.
 */
class Environment {
    std::unordered_map<std::string, std::pair<std::shared_ptr<void>, TokenType>> values; // Hash map of variable names to values
public:
    std::shared_ptr<Environment> enclosing; // Enclosing environment for variable scoping

    /**
     * @brief Construct a new Environment object
     */
    Environment() : enclosing(nullptr) {}

    /**
     * @brief Construct a new Environment object with an enclosing environment
     * 
     * @param enclosing The enclosing environment
     */
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

    /**
     * @brief Defines a new variable in the environment
     * 
     * @param name The name of the variable
     * @param value The value of the variable
     */
    void define(const std::string& name, const std::pair<std::shared_ptr<void>, TokenType>& value);

    /**
     * @brief Gets the value of a variable in the environment
     * 
     * @param name The name of the variable
     * @return The value of the variable
     */
    std::pair<std::shared_ptr<void>, TokenType> get(const Token& name);

    /**
     * @brief Gets the value of a variable in the environment
     * 
     * @param name The name of the variable
     * @return The value of the variable
     */
    std::pair<std::shared_ptr<void>, TokenType> get(const std::string& name);

    /**
     * @brief Assigns a new value to an existing variable in the environment
     * 
     * @param name The name of the variable
     * @param value The new value of the variable
     */
    void assign(const Token& name, const std::pair<std::shared_ptr<void>, TokenType>& value);
};

#endif // ENVIRONMENT_HPP