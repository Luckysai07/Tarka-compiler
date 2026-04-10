#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ast.h"
#include <memory>

class Optimizer {
public:
    void optimize(ProgramNode* program);

private:
    void optimizeBlock(BlockNode* node);
    void optimizeStatement(std::unique_ptr<ASTNode>& node);
    std::unique_ptr<ASTNode> optimizeExpr(std::unique_ptr<ASTNode> node);
};

#endif // OPTIMIZER_H
