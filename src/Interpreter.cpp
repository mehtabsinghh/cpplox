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
            break;
    }
}

void Interpreter::visitGrouping(const Grouping& expr) {
    result = evaluate(*expr.expression);
}

void Interpreter::visitLiteral(const Literal& expr) {
    result = expr.value;
    type = expr.type;
}

void Interpreter::visitUnary(const Unary& expr) {
    std::shared_ptr<void> right = evaluate(*expr.right);
    TokenType rightType = getType();
    switch (expr.op.getType()) {
        case TokenType::MINUS:
            checkNumberOperand(expr.op, rightType);
            result = std::make_shared<double>(-*std::static_pointer_cast<double>(right));
            break;
        case TokenType::BANG:
            result = std::make_shared<bool>(!isTruthy(right, rightType));
            type = !isTruthy(right, rightType) ? TokenType::TRUE : TokenType::FALSE;
            break;
        default:
            break;
    }
}

void Interpreter::visitVariable(const Variable& expr) {
    std::pair<std::shared_ptr<void>, TokenType> value = environment->get(expr.name);
    result = value.first;
    type = value.second;
}

void Interpreter::visitLogical(const Logical& expr) {
    std::shared_ptr<void> left = evaluate(*expr.left);
    TokenType leftType = getType();

    if (expr.op.getType() == TokenType::OR) {
        if (isTruthy(left, leftType)) {
            result = left;
            type = leftType;
            return;
        }
    } else {
        if (!isTruthy(left, leftType)) {
            result = left;
            type = leftType;
            return;
        }
    }

    result = evaluate(*expr.right);
    type = getType();
}

void Interpreter::visitCall(const Call& expr) {
    std::shared_ptr<void> callee = evaluate(*expr.callee);
    TokenType calleeType = getType();
    std::vector<std::pair<std::shared_ptr<void>, TokenType>> arguments;

    if (calleeType != TokenType::FUN && calleeType != TokenType::CLASS) {
       throw RuntimeError(expr.paren, "Can only call functions and classes.");
    }

    for (const auto& argument : expr.arguments) {
        evaluate(*argument);
        arguments.push_back(std::make_pair(getResult(), getType()));
    }

    std::shared_ptr<LoxFunction> function = std::static_pointer_cast<LoxFunction>(callee);
    if (arguments.size() != function->arity()) {
        throw RuntimeError(expr.paren, "Expected " + std::to_string(function->arity()) + " arguments but got " + std::to_string(arguments.size()) + ".");
    }
    auto [returnValue, returnType] = function->call(*this, arguments);
    result = returnValue;
    type = returnType;
}

void Interpreter::visitExpression(const Expression& stmt) {
    evaluate(*stmt.expression);
}

void Interpreter::visitFunction(const Function& stmt) {
    std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(std::make_unique<Function>(stmt), environment);
    environment->define(stmt.name.getLexeme(), std::make_pair(function, TokenType::FUN));
}

void Interpreter::visitPrint(const Print& stmt) {
    std::shared_ptr<void> value = evaluate(*stmt.expression);
    std::cout << stringify(value, type) << std::endl;
}

void Interpreter::visitVar(const Var& stmt) {
    std::shared_ptr<void> value = nullptr;
    if (stmt.initializer != nullptr) {
        value = evaluate(*stmt.initializer);
    }
    
    environment->define(stmt.name.getLexeme(), std::make_pair(value, type));
}

void Interpreter::visitAssign(const Assign& stmt) {
    std::shared_ptr<void> value = evaluate(*stmt.value);
    TokenType valueType = getType();
    environment->assign(stmt.name, std::make_pair(value, valueType));
}

std::shared_ptr<void> Interpreter::evaluate(const Expr& expr) {
    expr.accept(*this);
    return result;
}

void Interpreter::visitBlock(const Block& stmt) {
    executeBlock(stmt.statements, std::make_shared<Environment>(environment));
}

void Interpreter::visitIf(const If& stmt) {
    if (isTruthy(evaluate(*stmt.condition), getType())) {
        execute(*stmt.thenBranch);
    } else if (stmt.elseBranch != nullptr) {
        execute(*stmt.elseBranch);
    }
}

void Interpreter::visitWhile(const While& stmt) {
    while (isTruthy(evaluate(*stmt.condition), type)) {
        execute(*stmt.body);
    }
}

void Interpreter::visitReturn(const Return& stmt) {
    std::shared_ptr<void> value = nullptr;
    if (stmt.value != nullptr) {
        value = evaluate(*stmt.value);
    }
    throw ReturnException(value, type);
}

void Interpreter::executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> environment) {
    std::shared_ptr<Environment> previous = this->environment;
    try {
        this->environment = environment;
        for (const auto& statement : statements) {
            execute(*statement);
        }
    } catch (const RuntimeError& error) {
        Lox::runtimeError(error);
    }
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
    if (leftType != rightType) 
        return false;
    if (leftType == TokenType::TRUE || leftType == TokenType::FALSE) 
        return leftType == rightType;
    if (left == nullptr && right == nullptr) {
        std::cout << "Both are null" << std::endl;
        return true;
    }
    if (left == nullptr) 
        return false;
    
    if (leftType == TokenType::NUMBER) 
        return *std::static_pointer_cast<double>(left) == *std::static_pointer_cast<double>(right);
    if (leftType == TokenType::STRING) 
        return *std::static_pointer_cast<std::string>(left) == *std::static_pointer_cast<std::string>(right);
    if (leftType == TokenType::TRUE || leftType == TokenType::FALSE) 
        return *std::static_pointer_cast<bool>(left) == *std::static_pointer_cast<bool>(right);
    

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


