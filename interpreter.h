#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

class Interpreter {
public:
    Interpreter();
    void setInputFile(const std::string& filename);
    void execute(ASTNode* node);

private:
    std::unordered_map<std::string, int> memory;
    std::vector<int> inputs;
    size_t inputIndex;
    
    int evaluate(ASTNode* node);
    void interpretStatement(ASTNode* node);
};

#endif
