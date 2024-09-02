#ifndef LOX_FUNCTION_HPP
#define LOX_FUNCTION_HPP
#include "LoxCallable.hpp"

/**
 * @class LoxFunction
 * @brief Class for Lox functions
 * 
 * The LoxFunction class is a subclass of LoxCallable that represents a Lox
 * function. It contains a pointer to the function declaration and a pointer
 * to the closure environment.
 */
class LoxFunction : public LoxCallable {
    const std::unique_ptr<Function> declaration; // The function declaration
    const std::shared_ptr<Environment> closure; // The closure environment
public:
    /**
     * @brief Constructs a new LoxFunction object
     * 
     * @param declaration The function declaration
     * @param closure The closure environment
     */
    LoxFunction(std::unique_ptr<Function> declaration, std::shared_ptr<Environment> closure)
        : declaration(std::move(declaration)), closure(closure) {}

    /**
     * @brief Gets the arity of the function
     * 
     * @return int The arity of the function
     */
    int arity() override;

    /**
     * @brief Calls the function with the given arguments
     * 
     * @param interpreter The interpreter object
     * @param arguments The arguments to pass to the function
     * @return The return value and type of the function
     */
    std::pair<std::shared_ptr<void>, TokenType> call(Interpreter& interpreter, std::vector<std::pair<std::shared_ptr<void>, TokenType>> arguments) override;
    
    /**
     * @brief Converts the LoxFunction object to a string
     * 
     * @return std::string 
     */
    std::string toString() const override;
};

#endif