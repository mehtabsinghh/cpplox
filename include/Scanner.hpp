#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "Token.hpp"
#include <string>
#include <vector>
#include <unordered_map>

/**
 * @class Scanner
 * @brief Scans the source code and returns a vector of tokens
 * 
 * The Scanner class goes through the raw source code and creates a vector of tokens
 * to be used by the parser
 * 
 */
class Scanner {
private:
    const std::string source; // Source code to scan
    std::vector<Token> tokens; // List of tokens
    int start = 0; // Start position of current lexeme being scanned
    int current = 0; // Current position of the scanner
    int line = 1; // Current line number being scanned
    static const std::unordered_map<std::string, TokenType> keywords; // Map of reserved keywords and their token types
    
public:
    /**
     * @brief Constructs a new Scanner object with the given source code
     * 
     * @param source The source code to scan 
     */
    Scanner(const std::string& source) : source(source) {}

    /**
     * @brief Scans the source code and returns a vector of tokens
     * 
     * @return std::vector<Token> 
     */
    std::vector<Token> scanTokens();

    /**
     * @brief Checks if the scanner is at the end of the source code
     * 
     * @return True is the scanner is at the end, false otherwise
     */
    bool isAtEnd() const;

    /**
     * @brief Scans a single token from the source code
     */
    void scanToken();

    /**
     * @brief Advances to the next character in the source code
     * 
     * @return The current character before advancing
     */
    char advance();

    /**
     * @brief Adds a token of the given type to the list of tokens
     * 
     * @param type The type of token to add
     */
    void addToken(const TokenType type);

    /**
     * @brief Adds a token of the given type and literal value to the list of tokens
     * 
     * @param type The type of token to add
     * @param literal The literal value of the token
     */
    void addToken(const TokenType type, std::shared_ptr<void> literal);

    /**
     * @brief Checks if the current character matches the expected character
     * 
     * @param expected The character to match
     * @return True if the current character matches the expected character, false otherwise
     */
    bool match(const char expected);

    /**
     * @brief Returns the current character without consuming it
     * 
     * @return The current character
     */
    char peek() const;

    /**
     * @brief Scans a string literal from the source code
     */
    void string();

    /**
     * @brief Checks if the character is a digit
     * 
     * @param c The character to check
     * @return True if the character is a digit, false otherwise
     */
    bool isDigit(const char c) const;

    /**
     * @brief Scans a number literal from the source code
     */
    void number();

    /**
     * @brief Returns the next character without consuming it
     * 
     * @return The next character
     */
    char peekNext() const;

    /**
     * @brief Scans an identifier from the source code
     */
    void identifier();

    /**
     * @brief Checks if the character is an alphabetic character
     * 
     * @param c The character to check
     * @return True if the character is an alphabetic character, false otherwise
     */
    bool isAlpha(const char c) const;

    /**
     * @brief Checks if the character is an alphanumeric character
     * 
     * @param c The character to check
     * @return True if the character is an alphanumeric character, false otherwise
     */
    bool isAlphaNumeric(const char c) const;
    
};

#endif // SCANNER_HPP