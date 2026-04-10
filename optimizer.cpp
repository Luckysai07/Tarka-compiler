#include "optimizer.h"

void Optimizer::optimize(ProgramNode* program) {
    if (program && program->block) {
        optimizeBlock(program->block.get());
    }
}

void Optimizer::optimizeBlock(BlockNode* node) {
    if (!node) return;
    for (auto& stmt : node->statements) {
        optimizeStatement(stmt);
    }
}

void Optimizer::optimizeStatement(std::unique_ptr<ASTNode>& node) {
    if (!node) return;

    if (auto assignNode = dynamic_cast<AssignNode*>(node.get())) {
        assignNode->expr = optimizeExpr(std::move(assignNode->expr));
    } else if (auto ifNode = dynamic_cast<IfNode*>(node.get())) {
        ifNode->condition = optimizeExpr(std::move(ifNode->condition));
        if (ifNode->then_branch) {
            if (auto block = dynamic_cast<BlockNode*>(ifNode->then_branch.get())) {
                optimizeBlock(block);
            } else {
                optimizeStatement(ifNode->then_branch);
            }
        }
        if (ifNode->else_branch) {
            if (auto block = dynamic_cast<BlockNode*>(ifNode->else_branch.get())) {
                optimizeBlock(block);
            } else {
                optimizeStatement(ifNode->else_branch);
            }
        }
    } else if (auto whileNode = dynamic_cast<WhileNode*>(node.get())) {
        whileNode->condition = optimizeExpr(std::move(whileNode->condition));
        if (whileNode->body) {
            if (auto block = dynamic_cast<BlockNode*>(whileNode->body.get())) {
                optimizeBlock(block);
            } else {
                optimizeStatement(whileNode->body);
            }
        }
    } else if (auto writeNode = dynamic_cast<WriteNode*>(node.get())) {
        for (auto& expr : writeNode->exprs) {
            expr = optimizeExpr(std::move(expr));
        }
    }
}

std::unique_ptr<ASTNode> Optimizer::optimizeExpr(std::unique_ptr<ASTNode> node) {
    if (!node) return nullptr;

    if (auto binOp = dynamic_cast<BinOpNode*>(node.get())) {
        // Optimize children first (bottom-up traversal)
        binOp->left = optimizeExpr(std::move(binOp->left));
        binOp->right = optimizeExpr(std::move(binOp->right));

        // Constant folding
        if (auto leftNum = dynamic_cast<NumberNode*>(binOp->left.get())) {
            if (auto rightNum = dynamic_cast<NumberNode*>(binOp->right.get())) {
                int res = 0;
                if (binOp->op == "+") res = leftNum->value + rightNum->value;
                else if (binOp->op == "-") res = leftNum->value - rightNum->value;
                else if (binOp->op == "*") res = leftNum->value * rightNum->value;
                else if (binOp->op == "/") {
                    if (rightNum->value != 0) res = leftNum->value / rightNum->value;
                    else return node; // avoid div by zero at compile time
                }
                else if (binOp->op == ">") return std::make_unique<BoolNode>(leftNum->value > rightNum->value, binOp->line_no);
                else if (binOp->op == "<") return std::make_unique<BoolNode>(leftNum->value < rightNum->value, binOp->line_no);
                else if (binOp->op == ">=") return std::make_unique<BoolNode>(leftNum->value >= rightNum->value, binOp->line_no);
                else if (binOp->op == "<=") return std::make_unique<BoolNode>(leftNum->value <= rightNum->value, binOp->line_no);
                else if (binOp->op == "=") return std::make_unique<BoolNode>(leftNum->value == rightNum->value, binOp->line_no);
                else if (binOp->op == "<>") return std::make_unique<BoolNode>(leftNum->value != rightNum->value, binOp->line_no);

                return std::make_unique<NumberNode>(res, binOp->line_no);
            }
        }
    } else if (auto unaryOp = dynamic_cast<UnaryOpNode*>(node.get())) {
        unaryOp->operand = optimizeExpr(std::move(unaryOp->operand));
        
        if (auto num = dynamic_cast<NumberNode*>(unaryOp->operand.get())) {
            if (unaryOp->op == "-") return std::make_unique<NumberNode>(-num->value, unaryOp->line_no);
            else if (unaryOp->op == "+") return std::make_unique<NumberNode>(num->value, unaryOp->line_no);
        }
    }
    
    return node;
}
