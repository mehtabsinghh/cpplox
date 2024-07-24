#include "Interpreter.hpp"
#include "RuntimeError.hpp"
#include <iostream>

void Interpreter::visitLiteral(const Literal& expr) {
    result = expr.value;
}

void Interpreter::visitGrouping(const Grouping& expr) {
    result = evaluate(*expr.expression);
}

std::shared_ptr<void> Interpreter::evaluate(const Expr& expr) {
    expr.accept(*this);
    return result;
}

void Interpreter::visitUnary(const Unary& expr) {
    std::shared_ptr<void> right = evaluate(*expr.right);

    switch (expr.op.getType()) {
        case TokenType::MINUS:
            checkNumberOperand(expr.op, right);
            result = std::make_shared<double>(-*get_if<double>(right));
            break;
        case TokenType::BANG:
            result = std::make_shared<bool>(!isTruthy(right));
            break;
        default:
            break;
    }
}

bool Interpreter::isTruthy(const std::shared_ptr<void>& object) {
    if (object == nullptr) {
        return false;
    }
    if (auto boolean = get_if<bool>(object)) {
        return *boolean;
    }
    return true;
}

void Interpreter::visitBinary(const Binary& expr) {
    std::shared_ptr<void> left = evaluate(*expr.left);
    std::shared_ptr<void> right = evaluate(*expr.right);

    switch (expr.op.getType()) {
        case TokenType::GREATER:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<bool>(*get_if<double>(left) > *get_if<double>(right));
            break;
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<bool>(*get_if<double>(left) >= *get_if<double>(right));
            break;
        case TokenType::LESS:
            checkNumberOperands(expr.op, left, right);
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<bool>(*get_if<double>(left) < *get_if<double>(right));
            break;
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<bool>(*get_if<double>(left) <= *get_if<double>(right));
            break;
        case TokenType::BANG_EQUAL:
            result = std::make_shared<bool>(!isEqual(left, right));
            break;
        case TokenType::EQUAL_EQUAL:
            result = std::make_shared<bool>(isEqual(left, right));
            break;
        case TokenType::MINUS:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<double>(*get_if<double>(left) - *get_if<double>(right));
            break;
        case TokenType::PLUS:
            if (auto l = get_if<double>(left)) {
                if (auto r = get_if<double>(right)) {
                    result = std::make_shared<double>(*l + *r);
                } else if (auto r = get_if<std::string>(right)) {
                    result = std::make_shared<std::string>(std::to_string(*l) + *r);
                }
            } else if (auto l = get_if<std::string>(left)) {
                if (auto r = get_if<double>(right)) {
                    result = std::make_shared<std::string>(*l + std::to_string(*r));
                } else if (auto r = get_if<std::string>(right)) {
                    result = std::make_shared<std::string>(*l + *r);
                }
            } else {
                throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
            }
            break;
        case TokenType::SLASH:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<double>(*get_if<double>(left) / *get_if<double>(right));
            break;
        case TokenType::STAR:
            checkNumberOperands(expr.op, left, right);
            result = std::make_shared<double>(*get_if<double>(left) * *get_if<double>(right));
            break;
        default:
            break;
    }
}

bool Interpreter::isEqual(const std::shared_ptr<void>& a, const std::shared_ptr<void>& b) {
    if (a == nullptr && b == nullptr) {
        return true;
    } else if (a == nullptr) {
        return false;
    }
    if (auto a_num = get_if<double>(a), b_num = get_if<double>(b); a_num && b_num) {
            return *a_num == *b_num;
    } else if (auto a_str = get_if<std::string>(a), b_str = get_if<std::string>(b); a_str && b_str) {
            return *a_str == *b_str;
    } else if (auto a_bool = get_if<bool>(a), b_bool = get_if<bool>(b); a_bool && b_bool) {
            return *a_bool == *b_bool;
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

void Interpreter::interpret(std::unique_ptr<Expr>& expr) {
    try {
        std::shared_ptr<void> value = evaluate(*expr);
        std::cout << stringify(value) << std::endl;
    } catch (RuntimeError& error) {
        std::cerr << error.what() << std::endl;
    }
}

std::string Interpreter::stringify(const std::shared_ptr<void>& object) {
    if (object == nullptr) {
        return "nil";
    }
    if (auto num = get_if<double>(object)) {
        std::string text = std::to_string(*num);
        // Remove trailing ".0" if present
        if (text.find(".0") != std::string::npos) {
            text = text.substr(0, text.find(".0"));
        }
        return text;
    }
    // For other types, assume their toString method (if any)
    // This will require specific handling if other types are used
    // As a placeholder, just return a generic string conversion
    // This is a simplistic conversion; adjust as necessary
    return "unsupported type";
}