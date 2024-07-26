#include "Lox.hpp"
#include "AstPrinter.hpp"

// Helper function to create a sample expression
auto createSampleExpression() {
    // Creares the expression (* (- 123.00)(group 45.67))
    auto expression = std::make_unique<Binary>(
        std::make_unique<Unary>(
            Token(TokenType::MINUS, "-", nullptr, 1),
            std::make_unique<Literal>(TokenType::STRING, std::make_shared<double>(123))
        ),
        Token(TokenType::STAR, "*", nullptr, 1),
        std::make_unique<Grouping>(
            std::make_unique<Literal>(TokenType::NUMBER, std::make_shared<double>(45.67))
        )
    );
    return expression;
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
    } 
    return 0;
}