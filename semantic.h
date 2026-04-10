#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include <map>
#include <string>
#include <stdexcept>

class SemanticError : public std::runtime_error {
public:
    SemanticError(const std::string& msg) : std::runtime_error(msg) {}
};

class SemanticAnalyzer {
public:
    void analyze(ProgramNode* program);
    void printSymbolTable() const;

private:
    std::map<std::string, std::string> symbol_table;

    void analyzeBlock(BlockNode* node);
    void analyzeStatement(ASTNode* node);
    void analyzeAssign(AssignNode* node);
    void analyzeIf(IfNode* node);
    void analyzeWhile(WhileNode* node);
    void analyzeRead(ReadNode* node);
    void analyzeWrite(WriteNode* node);

    std::string getExprType(ASTNode* node);
    void reportError(int line, const std::string& msg);
};

#endif // SEMANTIC_H
