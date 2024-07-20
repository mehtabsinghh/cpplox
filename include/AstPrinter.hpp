#ifndef ASTPRINTER_HPP
#define ASTPRINTER_HPP

#include "Expr.hpp"
#include <string>

class AstPrinter : public ExprVisitor {
    std::string result = "";
public:
    void print (const Expr& Expr);
    void visitBinary (const Binary& Expr) override;
    void visitGrouping (const Grouping& Expr) override;
    void visitLiteral (const Literal& Expr) override;
    void visitUnary (const Unary& Expr) override;
    const std::string& get() const;
};

#endif
