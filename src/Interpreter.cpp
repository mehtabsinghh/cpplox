#include "Interpreter.hpp"


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
            result = std::make_shared<double>(-*get_if<double>(right));
            break;
        case TokenType::BANG:
            result = std::make_shared<bool>(!isTruthy(right));
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
            result = std::make_shared<bool>(*get_if<double>(left) > *get_if<double>(right));
            break;
        case TokenType::GREATER_EQUAL:
            result = std::make_shared<bool>(*get_if<double>(left) >= *get_if<double>(right));
            break;
        case TokenType::LESS:
            result = std::make_shared<bool>(*get_if<double>(left) < *get_if<double>(right));
            break;
        case TokenType::LESS_EQUAL:
            result = std::make_shared<bool>(*get_if<double>(left) <= *get_if<double>(right));
            break;
        case TokenType::BANG_EQUAL:
            result = std::make_shared<bool>(!isEqual(left, right));
            break;
        case TokenType::EQUAL_EQUAL:
            result = std::make_shared<bool>(isEqual(left, right));
            break;
        case TokenType::MINUS:
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
            }
        case TokenType::SLASH:
            result = std::make_shared<double>(*get_if<double>(left) / *get_if<double>(right));
            break;
        case TokenType::STAR:
            result = std::make_shared<double>(*get_if<double>(left) * *get_if<double>(right));
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