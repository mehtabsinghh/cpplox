#include "AstPrinter.hpp"

// Prints the expression by delegating to the appropriate visitor method
void AstPrinter::print(const Expr& expr) {
    expr.accept(*this);
}

// Formats binary expressions with appropriate spaces and parentheses
void AstPrinter::visitBinary(const Binary& expr) {
    result += "(";
    result += expr.op.getLexeme(); // Add the operator
    result += " ";
    expr.left->accept(*this); // Print the left side of the expression
    result += " "; // Add space between the left and right expression
    expr.right->accept(*this); // Print the right side of the expression
    result += ")";
}

// Formats grouping expressions with appropriate parentheses
void AstPrinter::visitGrouping(const Grouping& expr) {
    result += "(group ";
    expr.expression->accept(*this);
    result += ")";
}

// Formats literal values based on their type
void AstPrinter::visitLiteral(const Literal& expr) {
    if (expr.type == TokenType::NIL || expr.value == nullptr) {
        result += "nil";
    } else if (expr.type == TokenType::NUMBER) {
        result += std::to_string(*(std::static_pointer_cast<double>(expr.value)));
    } else if (expr.type == TokenType::STRING) {
        result += "\"" + *(std::static_pointer_cast<std::string>(expr.value)) + "\""; // Add quotes around strings
    } else {
        result += "unknown"; // Handle unexpected types
    }
}

// Formats unary expressions with appropriate parentheses
void AstPrinter::visitUnary(const Unary& expr) {
    result += "(";
    result += expr.op.getLexeme(); // Add the operator
    result += " ";
    expr.right->accept(*this); // Print the right side of the expression
    result += ")";
}

// Returns the formatted result string
const std::string& AstPrinter::get() const {
    return result;
}
