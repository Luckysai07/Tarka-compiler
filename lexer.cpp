#include "lexer.h"
#include <cctype>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

Lexer::Lexer(const std::string& source) : src(source), pos(0), line(1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token tok;
    do {
        tok = nextToken();
        tokens.push_back(tok);
    } while (tok.type != TokenType::EOF_TOKEN);
    return tokens;
}

char Lexer::peek(size_t offset) const {
    if (pos + offset >= src.length()) return '\0';
    return src[pos + offset];
}

char Lexer::advance() {
    if (pos >= src.length()) return '\0';
    char c = src[pos++];
    if (c == '\n') line++;
    return c;
}

void Lexer::skipWhitespaceAndComments() {
    while (pos < src.length()) {
        char c = peek();
        if (std::isspace(c)) {
            advance();
        } else if (c == '/' && peek(1) == '/') {
            // single line comment
            advance(); advance();
            while (pos < src.length() && peek() != '\n') {
                advance();
            }
        } else if (c == '/' && peek(1) == '*') {
            // block comment
            advance(); advance();
            while (pos < src.length() && !(peek() == '*' && peek(1) == '/')) {
                advance();
            }
            if (pos < src.length()) {
                advance(); advance(); // Consume */
            }
        } else {
            break;
        }
    }
}

Token Lexer::nextToken() {
    skipWhitespaceAndComments();

    if (pos >= src.length()) {
        return {TokenType::EOF_TOKEN, "", line};
    }

    char c = peek();

    // Identify identifiers and keywords
    if (std::isalpha(c) || c == '_') {
        std::string lexeme;
        while (pos < src.length() && (std::isalnum(peek()) || peek() == '_')) {
            lexeme += advance();
        }

        static const std::unordered_map<std::string, TokenType> keywords = {
            {"var", TokenType::VAR},
            {"if", TokenType::IF},
            {"else", TokenType::ELSE},
            {"while", TokenType::WHILE},
            {"read", TokenType::READ},
            {"write", TokenType::WRITE},
            {"writeln", TokenType::WRITELN},
            {"integer", TokenType::INTEGER},
            {"boolean", TokenType::BOOLEAN},
            {"true", TokenType::TRUE_TOKEN},
            {"false", TokenType::FALSE_TOKEN},
            {"and", TokenType::AND},
            {"or", TokenType::OR},
            {"not", TokenType::NOT}
        };

        // lowercase comparison
        std::string lower_lexeme;
        for (char ch : lexeme) lower_lexeme += std::tolower(ch);

        if (keywords.count(lower_lexeme)) {
            return {keywords.at(lower_lexeme), lexeme, line};
        }
        return {TokenType::IDENT, lexeme, line};
    }

    // Number literals
    if (std::isdigit(c)) {
        std::string lexeme;
        while (pos < src.length() && std::isdigit(peek())) {
            lexeme += advance();
        }
        return {TokenType::NUMBER, lexeme, line};
    }

    // Operators and basic tokens
    int current_line = line;
    char char1 = advance();
    char char2 = peek();

    if (char1 == '=' && char2 == '=') {
        advance(); return {TokenType::EQUAL, "==", current_line};
    } else if (char1 == '!' && char2 == '=') {
        advance(); return {TokenType::NOTEQUAL, "!=", current_line};
    } else if (char1 == '<' && char2 == '=') {
        advance(); return {TokenType::LTE, "<=", current_line};
    } else if (char1 == '>' && char2 == '=') {
        advance(); return {TokenType::GTE, ">=", current_line};
    }

    switch (char1) {
        case '+': return {TokenType::PLUS, "+", current_line};
        case '-': return {TokenType::MINUS, "-", current_line};
        case '*': return {TokenType::MULTIPLY, "*", current_line};
        case '/': return {TokenType::DIV, "/", current_line};
        case '%': return {TokenType::MOD, "%", current_line};
        case '=': return {TokenType::ASSIGN, "=", current_line};
        case '<': return {TokenType::LT, "<", current_line};
        case '>': return {TokenType::GT, ">", current_line};
        case '(': return {TokenType::LPAREN, "(", current_line};
        case ')': return {TokenType::RPAREN, ")", current_line};
        case '{': return {TokenType::LBRACE, "{", current_line};
        case '}': return {TokenType::RBRACE, "}", current_line};
        case ';': return {TokenType::SEMICOLON, ";", current_line};
        case ':': return {TokenType::COLON, ":", current_line};
        case ',': return {TokenType::COMMA, ",", current_line};
    }

    return {TokenType::UNKNOWN, std::string(1, char1), current_line};
}

std::string Lexer::tokenTypeToString(TokenType type) {
    switch(type) {
        case TokenType::VAR: return "VAR";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::READ: return "READ";
        case TokenType::WRITE: return "WRITE";
        case TokenType::WRITELN: return "WRITELN";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::BOOLEAN: return "BOOLEAN";
        case TokenType::TRUE_TOKEN: return "TRUE";
        case TokenType::FALSE_TOKEN: return "FALSE";
        case TokenType::IDENT: return "IDENT";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIV: return "DIV";
        case TokenType::MOD: return "MOD";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOTEQUAL: return "NOTEQUAL";
        case TokenType::LT: return "LT";
        case TokenType::GT: return "GT";
        case TokenType::LTE: return "LTE";
        case TokenType::GTE: return "GTE";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        default: return "UNKNOWN";
    }
}
