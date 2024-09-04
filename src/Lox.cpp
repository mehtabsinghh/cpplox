#include "Lox.hpp"
#include "Stmt.hpp"
#include <vector>

bool Lox::hadError = false;
bool Lox::hadRuntimeError = false;

void Lox::runFile(const std::string& path) {
    // Reads file from path
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        // File doesn't exist
        std::cerr << "Could not open file " << path << std::endl;
        exit(1);
    }

    // Stores bytes into vector and runs it
    std::istreambuf_iterator<char> begin(file);
    std::istreambuf_iterator<char> end;
    std::vector<char> bytes(begin, end);
    file.close();
    run(std::string(bytes.begin(), bytes.end()));

    // Indicate an error in the exit code
    if (hadError) std::exit(65);
    if (hadRuntimeError) std::exit(70);
}

void Lox::runPrompt() {
    std::string line;
    while (true) {
        // Puts input into line then runs it
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line.empty()) break;
        run(line);
        hadError = false;
    }
}

void Lox::run(const std::string& source) {
    // Scans the source code
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    // Parses the tokens
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    if (hadError) return; // Stop if there was a syntax error

    // Runs the interpreter
    Interpreter interpreter;
    interpreter.interpret(statements);
}

void Lox::error(int line, const std::string& message) {
    report(line, "", message);
}

void Lox::error(Token token, const std::string& message) {
    if (token.getType() == TokenType::END_OF_FILE) {
        report(token.getLine(), " at end", message);
    } else {
        report(token.getLine(), " at '" + token.getLexeme() + "'", message);
    }
}

void Lox::report(int line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}

void Lox::runtimeError(RuntimeError error) {
    std::cerr << error.what() << "\n[line" << error.token.getLine() << "]" << std::endl;
    hadError = true;
}