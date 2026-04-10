#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <string>
#include <vector>

class CodeGenerator {
public:
    void generate(ProgramNode* program, const std::string& outputFile);

private:
    std::vector<std::string> tac_instructions;
    int temp_count = 1;
    int label_count = 1;

    std::string newTemp();
    std::string newLabel();
    void emit(const std::string& instr);

    void genBlock(BlockNode* node);
    void genStatement(ASTNode* node);
    void genAssign(AssignNode* node);
    void genIf(IfNode* node);
    void genWhile(WhileNode* node);
    void genRead(ReadNode* node);
    void genWrite(WriteNode* node);
    
    std::string genExpr(ASTNode* node);
};

#endif // CODEGEN_H
