#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

enum class TokenType {
    VAR, IF, ELSE, WHILE,
    READ, WRITE, WRITELN, INTEGER, BOOLEAN, TRUE_TOKEN, FALSE_TOKEN,
    IDENT, NUMBER, ASSIGN, PLUS, MINUS, MULTIPLY, DIV, MOD,
    EQUAL, NOTEQUAL, LT, GT, LTE, GTE, AND, OR, NOT,
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COLON, COMMA, EOF_TOKEN,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    static std::string tokenTypeToString(TokenType type);

private:
    std::string src;
    size_t pos;
    int line;

    char peek(size_t offset = 0) const;
    char advance();
    void skipWhitespaceAndComments();
    Token nextToken();
};

#endif // LEXER_H
