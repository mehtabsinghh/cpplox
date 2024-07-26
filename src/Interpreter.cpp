#include "Interpreter.hpp"
#include "RuntimeError.hpp"
#include "Lox.hpp"
#include <iostream>

void Interpreter::visitLiteral(const Literal& expr) {
    result = expr.value;
    type = expr.type;
}

void Interpreter::visitGrouping(const Grouping& expr) {
    result = evaluate(*expr.expression);
}

std::shared_ptr<void> Interpreter::evaluate(const Expr& expr) {
    expr.accept(*this);
    return result;
}

void Interpreter::visitExpression(const Expression& stmt) {
    evaluate(*stmt.expression);
}

void Interpreter::visitPrint(const Print& stmt) {
    std::shared_ptr<void> value = evaluate(*stmt.expression);
    std::cout << stringify(value, type) << std::endl;
}

void Interpreter::execute(const Stmt& stmt) {
    stmt.accept(*this);
}

void Interpreter::visitUnary(const Unary& expr) {
    std::shared_ptr<void> right = evaluate(*expr.right);
    TokenType rightType = getType();
    switch (expr.op.getType()) {
        case TokenType::MINUS:
            checkNumberOperand(expr.op, right);
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

bool Interpreter::isTruthy(const std::shared_ptr<void>& object, TokenType type) {
    if (type == TokenType::FALSE) {
        return false;
    } else if (type == TokenType::NIL) {
        return false;
    } 
    return true;
}

void Interpreter::visitBinary(const Binary& expr) {
    std::shared_ptr<void> left = evaluate(*expr.left);
    TokenType leftType = getType();
    std::shared_ptr<void> right = evaluate(*expr.right);
    TokenType rightType = getType();

    switch (expr.op.getType()) {
        case TokenType::GREATER:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<bool>(*std::static_pointer_cast<double>(left) > *std::static_pointer_cast<double>(right));
            break;
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<bool>(*std::static_pointer_cast<double>(left) >= *std::static_pointer_cast<double>(right));
            break;
        case TokenType::LESS:
            checkNumberOperands(expr.op, left, right);
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<bool>(*std::static_pointer_cast<double>(left) < *std::static_pointer_cast<double>(right));
            break;
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<bool>(*std::static_pointer_cast<double>(left) <= *std::static_pointer_cast<double>(right));
            break;
        case TokenType::BANG_EQUAL:
            result = std::make_shared<bool>(!isEqual(left, right, leftType, rightType));
            break;
        case TokenType::EQUAL_EQUAL:
            result = std::make_shared<bool>(isEqual(left, right, leftType, rightType));
            break;
        case TokenType::MINUS:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<double>(*std::static_pointer_cast<double>(left) - *std::static_pointer_cast<double>(right));
            type = TokenType::NUMBER;
            break;
        case TokenType::PLUS:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<double>(*std::static_pointer_cast<double>(left) + *std::static_pointer_cast<double>(right));
            type = TokenType::NUMBER;
            break;
        case TokenType::SLASH:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<double>(*std::static_pointer_cast<double>(left) / *std::static_pointer_cast<double>(right));
            type = TokenType::NUMBER;
            break;
        case TokenType::STAR:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<double>(*std::static_pointer_cast<double>(left) * *std::static_pointer_cast<double>(right));
            type = TokenType::NUMBER;
            break;
        default:
            break;
    }
}

bool Interpreter::isEqual(const std::shared_ptr<void>& a, const std::shared_ptr<void>& b, TokenType aType, TokenType bType) {
    if (a == nullptr && b == nullptr) {
        return true;
    }
    if (a == nullptr) {
        return false;
    }

    if (aType != bType) {
        return false;
    }

    if (aType == TokenType::NUMBER) {
        return *std::static_pointer_cast<double>(a) == *std::static_pointer_cast<double>(b);
    } else if (aType == TokenType::STRING) {
        return *std::static_pointer_cast<std::string>(a) == *std::static_pointer_cast<std::string>(b);
    }

    return false;
}

void Interpreter::checkNumberOperand(const Token op, const std::shared_ptr<void>& operand) {
    if (auto oper = get_if<double>(operand)) {
        return;
    }
    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token op, const std::shared_ptr<void>& left, const std::shared_ptr<void>& right) {
    if (auto l = get_if<double>(left), r = get_if<double>(right); l && r) {
        return;
    }
    throw RuntimeError(op, "Operands must be numbers.");
}

void Interpreter::interpret(const std::vector<std::unique_ptr<Stmt>>& statements) {
    try {
        for (const auto& statement : statements) {
            execute(*statement);
        }
    } catch (const RuntimeError& error) {
        Lox::runtimeError(error);
    }
}

std::string Interpreter::stringify(const std::shared_ptr<void>& object, TokenType type) {
    if (type == TokenType::NIL) {
        return "nil";
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

std::shared_ptr<void>& Interpreter::getResult() {
    return result;
}

TokenType Interpreter::getType() {
    return type;
}