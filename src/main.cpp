#include "Lox.hpp"
#include "AstPrinter.hpp"

// Helper function to create a sample expression
std::unique_ptr<Expr> createSampleExpression() {
    // Creating shared pointers for literals
    auto intLiteral = std::make_shared<int>(123);
    auto doubleLiteral = std::make_shared<double>(45.67);

    // Creating Tokens with shared_ptr<void>
    Token minusToken(TokenType::MINUS, "-", nullptr, 1);
    Token starToken(TokenType::STAR, "*", nullptr, 1);

    // Creating expressions
    return std::make_unique<Binary>(
        std::make_unique<Unary>(
            minusToken,
            std::make_unique<Literal>(intLiteral)),
        starToken,
        std::make_unique<Grouping>(
            std::make_unique<Literal>(doubleLiteral))
    );
}

void testExpression() {
    // Create the sample expression
    auto expression = createSampleExpression();

    // Create the AstPrinter instance
    AstPrinter printer;

    // Print the expression
    printer.print(*expression);
    
    // Output the result
    std::cout << printer.get() << std::endl;
}

int main(int argc, char* argv[]) {
    /*
    Lox lox;

    if (argc > 2) {
        std::cerr << "Usage: cpplox [script]" << std::endl;
        return 1;
    } else if (argc == 2) {
        // Run file passed as argument
        lox.runFile(argv[1]);
    } else {
        // Run interactive prompt
        lox.runPrompt();
    } */
   testExpression();
    return 0;
}