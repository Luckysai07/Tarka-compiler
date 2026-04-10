#include "codegen.h"
#include <iostream>
#include <fstream>

std::string CodeGenerator::newTemp() {
    return "t" + std::to_string(temp_count++);
}

std::string CodeGenerator::newLabel() {
    return "L" + std::to_string(label_count++);
}

void CodeGenerator::emit(const std::string& instr) {
    tac_instructions.push_back(instr);
}

void CodeGenerator::generate(ProgramNode* program, const std::string& outputFile) {
    if (program && program->block) {
        genBlock(program->block.get());
    }

    std::cout << "========== CODEGEN OUTPUT ==========\n";
    for (const auto& instr : tac_instructions) {
        std::cout << instr << "\n";
    }

    std::ofstream out(outputFile);
    if (out.is_open()) {
        for (const auto& instr : tac_instructions) {
            out << instr << "\n";
        }
        out.close();
    } else {
        std::cerr << "Warning: Could not open output file " << outputFile << "\n";
    }
}

void CodeGenerator::genBlock(BlockNode* node) {
    for (const auto& stmt : node->statements) {
        if (stmt) genStatement(stmt.get());
    }
}

void CodeGenerator::genStatement(ASTNode* node) {
    if (auto n = dynamic_cast<AssignNode*>(node)) {
        genAssign(n);
    } else if (auto n = dynamic_cast<IfNode*>(node)) {
        genIf(n);
    } else if (auto n = dynamic_cast<WhileNode*>(node)) {
        genWhile(n);
    } else if (auto n = dynamic_cast<ReadNode*>(node)) {
        genRead(n);
    } else if (auto n = dynamic_cast<WriteNode*>(node)) {
        genWrite(n);
    } else if (auto n = dynamic_cast<BlockNode*>(node)) {
        genBlock(n);
    }
}

void CodeGenerator::genAssign(AssignNode* node) {
    std::string rhs = genExpr(node->expr.get());
    emit(node->ident + " = " + rhs);
}

void CodeGenerator::genIf(IfNode* node) {
    std::string cond = genExpr(node->condition.get());
    std::string l_then = newLabel();
    std::string l_end = newLabel();
    std::string l_else = node->else_branch ? newLabel() : l_end;

    emit("if " + cond + " goto " + l_then);
    emit("goto " + l_else);
    emit(l_then + ":");
    
    if (node->then_branch) genStatement(node->then_branch.get());
    
    if (node->else_branch) {
        emit("goto " + l_end);
        emit(l_else + ":");
        genStatement(node->else_branch.get());
    }
    
    emit(l_end + ":");
}

void CodeGenerator::genWhile(WhileNode* node) {
    std::string l_start = newLabel();
    std::string l_body = newLabel();
    std::string l_end = newLabel();

    emit(l_start + ":");
    std::string cond = genExpr(node->condition.get());
    
    emit("if " + cond + " goto " + l_body);
    emit("goto " + l_end);
    emit(l_body + ":");

    if (node->body) genStatement(node->body.get());

    emit("goto " + l_start);
    emit(l_end + ":");
}

void CodeGenerator::genRead(ReadNode* node) {
    for (const auto& ident : node->idents) {
        emit("read " + ident);
    }
}

void CodeGenerator::genWrite(WriteNode* node) {
    for (const auto& expr : node->exprs) {
        std::string val = genExpr(expr.get());
        if (node->is_writeln && &expr == &node->exprs.back()) {
            emit("write_nl " + val); // denote newline somehow or assume write does not newline? Specification says: Write/Writeln
        } else {
            emit("write " + val);
        }
    }
    if (node->is_writeln && node->exprs.empty()) {
        emit("write_nl");
    }
}

std::string CodeGenerator::genExpr(ASTNode* node) {
    if (auto n = dynamic_cast<NumberNode*>(node)) {
        return std::to_string(n->value);
    }
    if (auto n = dynamic_cast<BoolNode*>(node)) {
        return n->value ? "1" : "0"; // or true/false
    }
    if (auto n = dynamic_cast<IdentNode*>(node)) {
        return n->name;
    }
    if (auto n = dynamic_cast<BinOpNode*>(node)) {
        std::string left = genExpr(n->left.get());
        std::string right = genExpr(n->right.get());
        std::string t = newTemp();
        emit(t + " = " + left + " " + n->op + " " + right);
        return t;
    }
    if (auto n = dynamic_cast<UnaryOpNode*>(node)) {
        std::string operand = genExpr(n->operand.get());
        std::string t = newTemp();
        emit(t + " = " + n->op + " " + operand);
        return t;
    }
    return "unknown";
}
