#include "Interpreter.hpp"
#include "RuntimeError.hpp"
#include "Lox.hpp"
#include "LoxFunction.hpp"
#include "Clock.hpp"
#include <iostream>
#include "ReturnException.hpp"

Interpreter::Interpreter() {
    globals->define("clock", std::make_pair(std::make_shared<Clock>(), TokenType::FUN)); // Add the clock function to the global environment
}

void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements) {
    try {
        // Execute each statement
        for (const auto& statement : statements) {
            execute(*statement);
        }
    } catch (const RuntimeError& error) {
        // Catch any runtime errors and print them
        Lox::runtimeError(error);
    }
}

void Interpreter::visitBinary(const Binary& expr) {
    // Evaluate the left and right expressions
    std::shared_ptr<void> left = evaluate(*expr.left);
    const TokenType leftType = getType();
    std::shared_ptr<void> right = evaluate(*expr.right);
    const TokenType rightType = getType();

    // Perform the operation based on the operator type
    switch (expr.op.getType()) {
        // Equality and comparison operations
        case TokenType::GREATER:
            checkNumberOperands(expr.op, leftType, rightType);
            result = std::make_shared<bool>(*std::static_pointer_cast<double>(left) > *std::static_pointer_cast<double>(right));
            type = *std::static_pointer_cast<bool>(result) ? TokenType::TRUE : TokenType::FALSE;
            break;
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr.op, leftType, rightType);
            result = std::make_shared<bool>(*std::static_pointer_cast<double>(left) >= *std::static_pointer_cast<double>(right));
            type = *std::static_pointer_cast<bool>(result) ? TokenType::TRUE : TokenType::FALSE;
            break;
        case TokenType::LESS:
            checkNumberOperands(expr.op, leftType, rightType);
            result = std::make_shared<bool>(*std::static_pointer_cast<double>(left) < *std::static_pointer_cast<double>(right));
            type = *std::static_pointer_cast<bool>(result) ? TokenType::TRUE : TokenType::FALSE;
            break;
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr.op, leftType, rightType);
            result = std::make_shared<bool>(*std::static_pointer_cast<double>(left) <= *std::static_pointer_cast<double>(right));
            type = *std::static_pointer_cast<bool>(result) ? TokenType::TRUE : TokenType::FALSE;
            break;
        case TokenType::BANG_EQUAL:
            result = std::make_shared<bool>(!isEqual(left, right, leftType, rightType));
            type = *std::static_pointer_cast<bool>(result) ? TokenType::TRUE : TokenType::FALSE;
            break;
        case TokenType::EQUAL_EQUAL:
            result = std::make_shared<bool>(isEqual(left, right, leftType, rightType));
            type = *std::static_pointer_cast<bool>(result) ? TokenType::TRUE : TokenType::FALSE;
            break;
        
        // Arithmetic operations
        case TokenType::MINUS:
            checkNumberOperands(expr.op, leftType, rightType);
            result = std::make_shared<double>(*std::static_pointer_cast<double>(left) - *std::static_pointer_cast<double>(right));
            type = TokenType::NUMBER;
            break;
        case TokenType::PLUS:
            if (leftType == TokenType::NUMBER && rightType == TokenType::NUMBER) {
                // If both are numbers, add them
                result = std::make_shared<double>(*std::static_pointer_cast<double>(left) + *std::static_pointer_cast<double>(right));
                type = TokenType::NUMBER;
            } else if (leftType == TokenType::STRING && rightType == TokenType::STRING) {
                // If both are strings, concatenate them
                result = std::make_shared<std::string>(*std::static_pointer_cast<std::string>(left) + *std::static_pointer_cast<std::string>(right));
                type = TokenType::STRING;
            } else {
                // Otherwise, throw an error
                throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
            }
            break;
        case TokenType::SLASH:
            checkNumberOperands(expr.op, leftType, rightType);
            result = std::make_shared<double>(*std::static_pointer_cast<double>(left) / *std::static_pointer_cast<double>(right));
            type = TokenType::NUMBER;
            break;
        case TokenType::STAR:
            checkNumberOperands(expr.op, leftType, rightType);
            result = std::make_shared<double>(*std::static_pointer_cast<double>(left) * *std::static_pointer_cast<double>(right));
            type = TokenType::NUMBER;
            break;
        default:
            // Unreachable
            break;
    }
}

void Interpreter::visitGrouping(const Grouping& expr) {
    // Evaluate the expression within the grouping
    result = evaluate(*expr.expression);
}

void Interpreter::visitLiteral(const Literal& expr) {
    // Set the result and type to the value and type of the literal
    result = expr.value;
    type = expr.type;
}

void Interpreter::visitUnary(const Unary& expr) {
    // Evaluate the right expression
    std::shared_ptr<void> right = evaluate(*expr.right);
    TokenType rightType = getType();

    // Perform the operation based on the operator type
    switch (expr.op.getType()) {
        case TokenType::MINUS:
            // Negate the number
            checkNumberOperand(expr.op, rightType);
            result = std::make_shared<double>(-*std::static_pointer_cast<double>(right));
            break;
        case TokenType::BANG:
            // Negate the boolean
            result = std::make_shared<bool>(!isTruthy(right, rightType));
            type = !isTruthy(right, rightType) ? TokenType::TRUE : TokenType::FALSE;
            break;
        default:
            break;
    }
}

void Interpreter::visitVariable(const Variable& expr) {
    // Look up the variable in the environment
    std::pair<std::shared_ptr<void>, TokenType> value = environment->get(expr.name);

    // Set the result and type to the value and type of the variable
    result = value.first;
    type = value.second;
}

void Interpreter::visitLogical(const Logical& expr) {
    // Evaluate the left expression
    std::shared_ptr<void> left = evaluate(*expr.left);
    TokenType leftType = getType();

    // Perform the operation based on the operator type
    if (expr.op.getType() == TokenType::OR) {
        // If the left expression is truthy, return it
        if (isTruthy(left, leftType)) {
            result = left;
            type = leftType;
            return;
        }
    } else {
        // If the left expression is falsy, return it
        if (!isTruthy(left, leftType)) {
            result = left;
            type = leftType;
            return;
        }
    }

    // Evaluate the right expression
    result = evaluate(*expr.right);
    type = getType();
}

void Interpreter::visitCall(const Call& expr) {
    // Evaluate the callee
    std::shared_ptr<void> callee = evaluate(*expr.callee);
    TokenType calleeType = getType();
    std::vector<std::pair<std::shared_ptr<void>, TokenType>> arguments;

    // Check if the callee is a function or class
    if (calleeType != TokenType::FUN && calleeType != TokenType::CLASS) {
       throw RuntimeError(expr.paren, "Can only call functions and classes.");
    }

    // Evaluate the arguments
    for (const auto& argument : expr.arguments) {
        evaluate(*argument);
        arguments.push_back(std::make_pair(getResult(), getType()));
    }

    // Call the function or class
    std::shared_ptr<LoxFunction> function = std::static_pointer_cast<LoxFunction>(callee);
    if (arguments.size() != function->arity()) {
        throw RuntimeError(expr.paren, "Expected " + std::to_string(function->arity()) + " arguments but got " + std::to_string(arguments.size()) + ".");
    }

    // Get the return value and type of the function
    auto [returnValue, returnType] = function->call(*this, arguments);
    result = returnValue;
    type = returnType;
}

void Interpreter::visitExpression(const Expression& stmt) {
    // Evaluate the expression
    evaluate(*stmt.expression);
}

void Interpreter::visitFunction(const Function& stmt) {
    // Create a new function and define it in the current environment
    std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(std::make_unique<Function>(stmt), environment);
    environment->define(stmt.name.getLexeme(), std::make_pair(function, TokenType::FUN));
}

void Interpreter::visitPrint(const Print& stmt) {
    // Evaluate the expression and print the result
    std::shared_ptr<void> value = evaluate(*stmt.expression);
    std::cout << stringify(value, type) << std::endl;
}

void Interpreter::visitVar(const Var& stmt) {
    std::shared_ptr<void> value = nullptr;
    
    // Evaluate the initialiser if present
    if (stmt.initializer != nullptr) {
        value = evaluate(*stmt.initializer);
    }
    
    // Define the variable in the current environment
    environment->define(stmt.name.getLexeme(), std::make_pair(value, type));
}

void Interpreter::visitAssign(const Assign& stmt) {
    // Evaluate the value and assign it to the variable
    std::shared_ptr<void> value = evaluate(*stmt.value);
    TokenType valueType = getType();
    environment->assign(stmt.name, std::make_pair(value, valueType));
}

std::shared_ptr<void> Interpreter::evaluate(const Expr& expr) {
    // Visit the expression and return the result
    expr.accept(*this);
    return result;
}

void Interpreter::visitBlock(const Block& stmt) {
    // Execute the block of statements within a new environment
    executeBlock(stmt.statements, std::make_shared<Environment>(environment));
}

void Interpreter::visitIf(const If& stmt) {
    // Evaluate the condition and execute the appropriate branch
    if (isTruthy(evaluate(*stmt.condition), getType())) {
        execute(*stmt.thenBranch);
    } else if (stmt.elseBranch != nullptr) {
        execute(*stmt.elseBranch);
    }
}

void Interpreter::visitWhile(const While& stmt) {
    // Execute the loop while the condition is truthy
    while (isTruthy(evaluate(*stmt.condition), type)) {
        execute(*stmt.body);
    }
}

void Interpreter::visitReturn(const Return& stmt) {
    // Evaluate the return value
    std::shared_ptr<void> value = nullptr;
    if (stmt.value != nullptr) {
        value = evaluate(*stmt.value);
    }

    // Throw a return exception to extit the function
    throw ReturnException(value, type);
}

void Interpreter::executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> environment) {
    std::shared_ptr<Environment> previous = this->environment;
    try {
        // Execute the block of statements within the given environment
        this->environment = environment;
        for (const auto& statement : statements) {
            execute(*statement);
        }
    } catch (const RuntimeError& error) {
        // Catch any runtime errors and print them
        Lox::runtimeError(error);
    }

    // Restore the previous environment
    this->environment = previous;
}

std::shared_ptr<void>& Interpreter::getResult() {
    return result;
}

TokenType Interpreter::getType() {
    return type;
}

bool Interpreter::isTruthy(const std::shared_ptr<void>& object, TokenType type) {
    if (type == TokenType::FALSE) {
        return false;
    } else if (type == TokenType::NIL) {
        return false;
    } 
    return true;
}

bool Interpreter::isEqual(const std::shared_ptr<void>& left, const std::shared_ptr<void>& right, TokenType leftType, TokenType rightType) {
    // Check if the types are the same
    if (leftType != rightType) 
        return false;

    // Check if boolean values are the same type
    if (leftType == TokenType::TRUE || leftType == TokenType::FALSE) 
        return leftType == rightType;
    if (left == nullptr && right == nullptr) {
        std::cout << "Both are null" << std::endl;
        return true;
    }
    if (left == nullptr) 
        return false;
    
    // Check if the values are the same
    if (leftType == TokenType::NUMBER) 
        return *std::static_pointer_cast<double>(left) == *std::static_pointer_cast<double>(right);
    if (leftType == TokenType::STRING) 
        return *std::static_pointer_cast<std::string>(left) == *std::static_pointer_cast<std::string>(right);
    if (leftType == TokenType::TRUE || leftType == TokenType::FALSE) 
        return *std::static_pointer_cast<bool>(left) == *std::static_pointer_cast<bool>(right);
    
    // Otherwise return false
    return false;
}

void Interpreter::checkNumberOperand(const Token op, const TokenType type) {
    if (type == TokenType::NUMBER) {
        return;
    }
    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token op, const TokenType leftType, const TokenType rightType) {
    if (leftType == TokenType::NUMBER && rightType == TokenType::NUMBER) {
        return;
    }
    throw RuntimeError(op, "Operands must be numbers.");
}

std::string Interpreter::stringify(const std::shared_ptr<void>& object, TokenType type) {
    if (type == TokenType::NIL) {
        return "nil";
    }
    if (type == TokenType::FUN) {
        return std::static_pointer_cast<LoxFunction>(object)->toString();
    }
    if (type == TokenType::NUMBER) {
        auto num = *std::static_pointer_cast<double>(object);
        std::string text = std::to_string(num);
        // Remove trailing ".0" if present
        if (text.find(".0") != std::string::npos) {
            text = text.substr(0, text.find(".0"));
        }
        return text;
    } else if (type == TokenType::STRING) {
        return *std::static_pointer_cast<std::string>(object);
    } else if (type == TokenType::TRUE) {
        return "true";
    } else if (type == TokenType::FALSE) {
        return "false";
    } else if (object == nullptr) {
        return "nil";
    }

    return "nil";
}

void Interpreter::execute(const Stmt& stmt) {
    stmt.accept(*this);
}


