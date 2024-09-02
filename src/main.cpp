#include "Lox.hpp"

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