#include "interpreter.h"
#include <stdexcept>

Interpreter::Interpreter() : inputIndex(0) {}

void Interpreter::setInputFile(const std::string& filename) {
    if (filename.empty()) return;
    std::ifstream file(filename);
    int val;
    while (file >> val) {
        inputs.push_back(val);
    }
}

int Interpreter::evaluate(ASTNode* node) {
    if (auto num = dynamic_cast<NumberNode*>(node)) {
        return num->value;
    } else if (auto boolean = dynamic_cast<BoolNode*>(node)) {
        return boolean->value ? 1 : 0;
    } else if (auto ident = dynamic_cast<IdentNode*>(node)) {
        if (memory.find(ident->name) == memory.end()) {
            return 0; // Default initialized to 0
        }
        return memory[ident->name];
    } else if (auto binop = dynamic_cast<BinOpNode*>(node)) {
        int left = evaluate(binop->left.get());
        int right = evaluate(binop->right.get());
        
        if (binop->op == "+") return left + right;
        if (binop->op == "-") return left - right;
        if (binop->op == "*") return left * right;
        if (binop->op == "/") {
            if (right == 0) throw std::runtime_error("Division by zero");
            return left / right;
        }
        if (binop->op == "%") {
            if (right == 0) throw std::runtime_error("Modulo by zero");
            return left % right;
        }
        if (binop->op == "==") return left == right;
        if (binop->op == "!=") return left != right;
        if (binop->op == "<") return left < right;
        if (binop->op == ">") return left > right;
        if (binop->op == "<=") return left <= right;
        if (binop->op == ">=") return left >= right;
        if (binop->op == "and") return (left != 0 && right != 0) ? 1 : 0;
        if (binop->op == "or") return (left != 0 || right != 0) ? 1 : 0;

    } else if (auto unary = dynamic_cast<UnaryOpNode*>(node)) {
        int val = evaluate(unary->operand.get());
        if (unary->op == "-") return -val;
        if (unary->op == "not") return (val == 0) ? 1 : 0;
    }

    throw std::runtime_error("Invalid expression node in interpreter");
}

void Interpreter::interpretStatement(ASTNode* node) {
    if (!node) return;

    if (auto program = dynamic_cast<ProgramNode*>(node)) {
        interpretStatement(program->block.get());
    } else if (auto block = dynamic_cast<BlockNode*>(node)) {
        for (auto& stmt : block->statements) {
            interpretStatement(stmt.get());
        }
    } else if (auto assign = dynamic_cast<AssignNode*>(node)) {
        memory[assign->ident] = evaluate(assign->expr.get());
    } else if (auto ifStmt = dynamic_cast<IfNode*>(node)) {
        if (evaluate(ifStmt->condition.get()) != 0) {
            interpretStatement(ifStmt->then_branch.get());
        } else if (ifStmt->else_branch) {
            interpretStatement(ifStmt->else_branch.get());
        }
    } else if (auto whileStmt = dynamic_cast<WhileNode*>(node)) {
        while (evaluate(whileStmt->condition.get()) != 0) {
            interpretStatement(whileStmt->body.get());
        }
    } else if (auto readStmt = dynamic_cast<ReadNode*>(node)) {
        for (const auto& id : readStmt->idents) {
            int val = 0;
            if (inputIndex < inputs.size()) {
                val = inputs[inputIndex++];
            } else {
                std::cin >> val;
            }
            memory[id] = val;
        }
    } else if (auto writeStmt = dynamic_cast<WriteNode*>(node)) {
        for (size_t i = 0; i < writeStmt->exprs.size(); ++i) {
            std::cout << evaluate(writeStmt->exprs[i].get());
            if (i < writeStmt->exprs.size() - 1) std::cout << " ";
        }
        if (writeStmt->is_writeln) {
            std::cout << "\n";
        }
    }
}

void Interpreter::execute(ASTNode* node) {
    interpretStatement(node);
}
