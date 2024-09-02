#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Environment.hpp"

/**
 * @class Interpreter
 * @brief Executes the statements and expressions parsed from source code.
 * 
 * The Interpreter class is responsible for evaluating expressions and executing 
 * statements. It implements the visitor pattern to handle different types of 
 * expressions and statements, and maintains an environment for variable storage 
 * and function definitions.
 */
class Interpreter : public ExprVisitor, StmtVisitor {
public:
    const std::shared_ptr<Environment> globals = std::make_shared<Environment>(); // Global environment for storing variables and functions

    /**
     * @brief Construct a new Interpreter object and defines clock function
     */
    Interpreter();

    /**
     * @brief Interprets and executes a list of statements.
     * 
     * @param statements The statements to execute.
     */
    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);

    /**
     * @brief Methods to visit and evaluate different types of expressions.
     * 
     * These methods handle the evaluation of various expression types such as 
     * binary operations, grouping expressions, literals, unary operations, 
     * variable access, logical operations, and function calls
     */
    void visitBinary (const Binary& expr) override;
    void visitGrouping (const Grouping& expr) override;
    void visitLiteral (const Literal& expr) override;
    void visitUnary (const Unary& expr) override;
    void visitVariable(const Variable& expr) override;
    void visitLogical(const Logical& expr) override;
    void visitCall(const Call& expr) override;

    /**
     * @brief Methods to visit and execute different types of statements.
     * 
     * These methods handle the execution of various statement types such as 
     * expression statements, function declarations, print statements, variable 
     * declarations, assignment, block statements, conditional statements, loops, 
     * and return statements
     */
    void visitExpression(const Expression& stmt) override;
    void visitFunction(const Function& stmt) override;
    void visitPrint(const Print& stmt) override;
    void visitVar(const Var& stmt) override;
    void visitAssign(const Assign& stmt) override;
    void visitBlock(const Block& stmt) override;
    void visitIf(const If& stmt) override;
    void visitWhile(const While& stmt) override;
    void visitReturn(const Return& stmt) override;

    /**
     * @brief Executes a block of statements within a given environment.
     * 
     * @param statements The statements to execute
     * @param environment  The environment in which to execute the statements
     */
    void executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> environment);
    
    /**
     * @brief Gets the result of the last executed statement or expression
     * 
     * @return A reference of the last executede statement or expression
     */
    std::shared_ptr<void>& getResult();

    /**
     * @brief Gets the type of the result of the last executed expression 
     * 
     * @return The type of the last exectued statement or expression 
     */
    TokenType getType();

private:
    std::shared_ptr<Environment> environment = globals; // Current environment for variable storage and function definitions
    std::shared_ptr<void> result; // Result of the last executed statement or expression
    TokenType type; // Type of the last executed statement or expression

    /**
     * @brief Evaluates an expression and returns the result
     * 
     * @param expr The expression ot evaluate
     * @return A shared pointer to the result of the expression
     */
    std::shared_ptr<void> evaluate(const Expr& expr);

    /**
     * @brief Determines if an object is truthy or falsy for conditional checks
     * 
     * @param object The object to check
     * @param type The type of the object
     * @return True if the object is truthy, false otherwise
     */
    bool isTruthy(const std::shared_ptr<void>& object, TokenType type);

    /**
     * @brief Determines if two objects are equal
     * 
     * @param left The first object
     * @param right The second object
     * @param leftType The type of the first object
     * @param rightType The type of the second object
     * @return True if the objects are equal, false otherwise
     */
    bool isEqual(const std::shared_ptr<void>& left, const std::shared_ptr<void>& right, TokenType leftType, TokenType rightType);

    /**
     * @brief Checks if an operand is a number for unary and binary operations
     * 
     * @param op The operator token
     * @param type The type of the operand
     */
    void checkNumberOperand(const Token op, const TokenType type);

    /**
     * @brief Check if both operands are numbers for binary operations
     * 
     * @param op The operator token
     * @param leftType The type of left operand
     * @param rightType The type of right operand
     */
    void checkNumberOperands(const Token op, const TokenType leftType, const TokenType rightType);

    /**
     * @brief Converts an object to a string representation
     * 
     * @param object The object to convert
     * @param type The type of the object
     * @return A string representation of the object
     */
    std::string stringify(const std::shared_ptr<void>& object, TokenType type);

    /**
     * @brief Executes a statement
     * 
     * @param stmt The statement to execute
     */
    void execute(const Stmt& stmt);
};

#endif // INTERPRETER_HPP