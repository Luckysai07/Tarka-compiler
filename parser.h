#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <vector>
#include <memory>
#include <stdexcept>

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ProgramNode> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    Token peek(size_t offset = 0) const;
    Token advance();
    bool match(TokenType type);
    void expect(TokenType type, const std::string& err_msg);
    void reportError(const std::string& msg);

    std::unique_ptr<ProgramNode> parseProgram();
    std::vector<std::unique_ptr<VarDeclNode>> parseVarDecls();
    std::unique_ptr<BlockNode> parseBlock();
    std::vector<std::unique_ptr<ASTNode>> parseStmtList();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<AssignNode> parseAssign(const Token& ident);
    std::unique_ptr<IfNode> parseIf();
    std::unique_ptr<WhileNode> parseWhile();
    std::unique_ptr<ReadNode> parseRead();
    std::unique_ptr<WriteNode> parseWrite();

    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseSimpleExpr();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
};

#endif // PARSER_H
