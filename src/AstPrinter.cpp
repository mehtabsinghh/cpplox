#include "AstPrinter.hpp"

void AstPrinter::print (const Expr& Expr) {
    Expr.accept(*this);
}

void AstPrinter::visitBinary (const Binary& Expr) {
    result += "(" + Expr.op.getLexeme() + " ";
    Expr.left->accept(*this);
    Expr.right->accept(*this);
    result += ")";
}

void AstPrinter::visitGrouping (const Grouping& Expr) {
    result += "(group ";
    Expr.expression->accept(*this);
    result += ")";
}

/* TODO:
    Does not work, add cases for different types of literals (int, double, string, etc.)
*/
void AstPrinter::visitLiteral (const Literal& Expr) {
    if (Expr.type == TokenType::NIL || Expr.value == nullptr) {
        result += "nil";
    }
    if (Expr.type == TokenType::NUMBER) {
        result += std::to_string(*(std::static_pointer_cast<double>(Expr.value)));
    } else if (Expr.type == TokenType::STRING) {
        result += *(std::static_pointer_cast<std::string>(Expr.value));
    } else {
        result += "nil";
    }
}

void AstPrinter::visitUnary (const Unary& Expr) {
    result += "(" + Expr.op.getLexeme() + " ";
    Expr.right->accept(*this);
    result += ")";
}

const std::string& AstPrinter::get() const {
    return result;
}