#include "parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tks) : tokens(tks), pos(0) {}

Token Parser::peek(size_t offset) const {
    if (pos + offset >= tokens.size()) {
        return tokens.empty() ? Token{TokenType::EOF_TOKEN, "", 0} : tokens.back();
    }
    return tokens[pos + offset];
}

Token Parser::advance() {
    if (pos >= tokens.size()) return tokens.back();
    return tokens[pos++];
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& err_msg) {
    if (!match(type)) {
        reportError(err_msg);
    }
}

void Parser::reportError(const std::string& msg) {
    Token t = peek();
    throw ParseError("Parse error at line " + std::to_string(t.line) + ": " + msg + " (got '" + t.lexeme + "')");
}

std::unique_ptr<ProgramNode> Parser::parse() {
    return parseProgram();
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>("Main");

    program->var_decls = parseVarDecls();

    auto block = std::make_unique<BlockNode>();
    while (peek().type != TokenType::EOF_TOKEN) {
        block->statements.push_back(parseStatement());
        if (match(TokenType::SEMICOLON)) {
            // handle trailing semicolons or empty statements gracefully
        }
    }
    program->block = std::move(block);

    return program;
}

std::vector<std::unique_ptr<VarDeclNode>> Parser::parseVarDecls() {
    std::vector<std::unique_ptr<VarDeclNode>> decls;
    while (match(TokenType::VAR)) {
        std::vector<std::string> names;
        names.push_back(peek().lexeme);
        expect(TokenType::IDENT, "Expected identifier");

        while (match(TokenType::COMMA)) {
            names.push_back(peek().lexeme);
            expect(TokenType::IDENT, "Expected identifier after ','");
        }

        expect(TokenType::COLON, "Expected ':'");

        std::string type;
        if (peek().type == TokenType::INTEGER || peek().type == TokenType::BOOLEAN) {
            type = advance().lexeme;
        } else {
            reportError("Expected type 'integer' or 'boolean'");
        }

        auto decl = std::make_unique<VarDeclNode>(type);
        decl->names = names;
        decls.push_back(std::move(decl));

        expect(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    }
    return decls;
}

std::unique_ptr<BlockNode> Parser::parseBlock() {
    expect(TokenType::LBRACE, "Expected '{'");
    auto block = std::make_unique<BlockNode>();
    block->statements = parseStmtList();
    expect(TokenType::RBRACE, "Expected '}'");
    return block;
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseStmtList() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    if (peek().type == TokenType::RBRACE || peek().type == TokenType::EOF_TOKEN) return statements;

    statements.push_back(parseStatement());

    while (match(TokenType::SEMICOLON)) {
        if (peek().type == TokenType::RBRACE || peek().type == TokenType::EOF_TOKEN) break;
        statements.push_back(parseStatement());
    }
    return statements;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    Token t = peek();
    if (t.type == TokenType::IDENT) {
        advance();
        return parseAssign(t);
    } else if (match(TokenType::IF)) {
        return parseIf();
    } else if (match(TokenType::WHILE)) {
        return parseWhile();
    } else if (t.type == TokenType::READ) {
        return parseRead();
    } else if (t.type == TokenType::WRITE || t.type == TokenType::WRITELN) {
        return parseWrite();
    } else if (t.type == TokenType::LBRACE) {
        return parseBlock();
    }
    
    reportError("Unexpected statement");
    return nullptr;
}

std::unique_ptr<AssignNode> Parser::parseAssign(const Token& ident) {
    expect(TokenType::ASSIGN, "Expected '=' in assignment");
    auto expr = parseExpression();
    return std::make_unique<AssignNode>(ident.lexeme, std::move(expr), ident.line);
}

std::unique_ptr<IfNode> Parser::parseIf() {
    auto cond = parseExpression();
    auto then_branch = parseStatement();
    std::unique_ptr<ASTNode> else_branch = nullptr;

    if (match(TokenType::ELSE)) {
        else_branch = parseStatement();
    }
    return std::make_unique<IfNode>(std::move(cond), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<WhileNode> Parser::parseWhile() {
    auto cond = parseExpression();
    auto body = parseStatement();
    return std::make_unique<WhileNode>(std::move(cond), std::move(body));
}

std::unique_ptr<ReadNode> Parser::parseRead() {
    Token readTok = advance(); // read
    expect(TokenType::LPAREN, "Expected '('");
    auto node = std::make_unique<ReadNode>(readTok.line);
    
    node->idents.push_back(peek().lexeme);
    expect(TokenType::IDENT, "Expected identifier");

    while (match(TokenType::COMMA)) {
        node->idents.push_back(peek().lexeme);
        expect(TokenType::IDENT, "Expected identifier");
    }

    expect(TokenType::RPAREN, "Expected ')'");
    return node;
}

std::unique_ptr<WriteNode> Parser::parseWrite() {
    Token writeTok = advance();
    bool is_writeln = (writeTok.type == TokenType::WRITELN);
    auto node = std::make_unique<WriteNode>(is_writeln, writeTok.line);

    expect(TokenType::LPAREN, "Expected '('");
    
    node->exprs.push_back(parseExpression());

    while (match(TokenType::COMMA)) {
        node->exprs.push_back(parseExpression());
    }

    expect(TokenType::RPAREN, "Expected ')'");
    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto left = parseSimpleExpr();

    Token t = peek();
    if (t.type == TokenType::EQUAL || t.type == TokenType::NOTEQUAL || 
        t.type == TokenType::LT || t.type == TokenType::GT || 
        t.type == TokenType::LTE || t.type == TokenType::GTE) {
        advance();
        auto right = parseSimpleExpr();
        return std::make_unique<BinOpNode>(std::move(left), t.lexeme, std::move(right), t.line);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseSimpleExpr() {
    Token t = peek();
    int line = t.line;
    bool has_unary = false;
    std::string unary_op;

    if (t.type == TokenType::PLUS || t.type == TokenType::MINUS) {
        advance();
        has_unary = true;
        unary_op = t.lexeme;
    }

    auto term = parseTerm();
    
    std::unique_ptr<ASTNode> node;
    if (has_unary) {
        node = std::make_unique<UnaryOpNode>(unary_op, std::move(term), line);
    } else {
        node = std::move(term);
    }

    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS || peek().type == TokenType::OR) {
        Token opTok = advance();
        auto right = parseTerm();
        node = std::make_unique<BinOpNode>(std::move(node), opTok.lexeme, std::move(right), opTok.line);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto node = parseFactor();

    while (peek().type == TokenType::MULTIPLY || peek().type == TokenType::DIV || 
           peek().type == TokenType::MOD || peek().type == TokenType::AND) {
        Token opTok = advance();
        auto right = parseFactor();
        node = std::make_unique<BinOpNode>(std::move(node), opTok.lexeme, std::move(right), opTok.line);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    Token t = peek();

    if (t.type == TokenType::NUMBER) {
        advance();
        return std::make_unique<NumberNode>(std::stoi(t.lexeme), t.line);
    } else if (t.type == TokenType::IDENT) {
        advance();
        return std::make_unique<IdentNode>(t.lexeme, t.line);
    } else if (t.type == TokenType::TRUE_TOKEN) {
        advance();
        return std::make_unique<BoolNode>(true, t.line);
    } else if (t.type == TokenType::FALSE_TOKEN) {
        advance();
        return std::make_unique<BoolNode>(false, t.line);
    } else if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')'");
        return expr;
    } else if (t.type == TokenType::NOT) {
        advance();
        auto factor = parseFactor();
        return std::make_unique<UnaryOpNode>("not", std::move(factor), t.line);
    } else if (t.type == TokenType::MINUS) {
        advance();
        auto factor = parseFactor();
        return std::make_unique<UnaryOpNode>("-", std::move(factor), t.line);
    }

    reportError("Unexpected token in expression");
    return nullptr;
}
