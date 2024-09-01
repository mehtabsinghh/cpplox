#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "TokenInfo.hpp"
#include <string>
#include <memory>

/**
 * @class Token
 * @brief Represents a token in the source code
 * 
 * The Token class represents a token in the source code. Each token has a type,
 * lexeme, literal value, and line number
 * 
 */
class Token{
    const TokenType type;
    const std::string lexeme;
    std::shared_ptr<void> literal;
    const int line;
public:
    
    /**
     * @brief Construct a new Token object
     * 
     * @param type Type of the token
     * @param lexeme Lexeme (text) of the token
     * @param literal Literal value of the token if any
     * @param line Line number where the token is located
     */
    Token(TokenType type, const std::string& lexeme, std::shared_ptr<void> literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}
    
    /**
     * @brief Converts the token to a string representation
     * 
     * @return std::string A string representation of the token
     */
    std::string toString() const;

    /**
     * @brief Gets the lexeme of the token
     * 
     * @return std::string The lexeme (text) of the token
     */
    std::string getLexeme() const;

    /**
     * @brief Gets the type of the token
     * 
     * @return TokenType The type of the token
     */
    TokenType getType() const;

    /**
     * @brief Gets the literal value of the token
     * 
     * @return std::shared_ptr<void> The literal value of the token
     */
    std::shared_ptr<void> getLiteral() const;

    /**
     * @brief Gets the line number where the token is located
     * 
     * @return int The line number where the token is located
     */
    int getLine() const;
};

#endif // TOKEN_HPP