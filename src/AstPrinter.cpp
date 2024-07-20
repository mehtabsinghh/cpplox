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

void AstPrinter::visitLiteral (const Literal& Expr) {
    if (Expr.value == nullptr) {
        result += "nil";
    } else {
        result += *std::static_pointer_cast<std::string>(Expr.value);
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