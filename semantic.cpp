#include "semantic.h"
#include <iostream>

void SemanticAnalyzer::reportError(int line, const std::string& msg) {
    throw SemanticError("Semantic Error at line " + std::to_string(line) + ": " + msg);
}

void SemanticAnalyzer::printSymbolTable() const {
    std::cout << "========== SEMANTIC OUTPUT ==========\n";
    std::cout << "Symbol Table:\n";
    for (const auto& pair : symbol_table) {
        std::cout << "  " << pair.first << " : " << pair.second << "\n";
    }
    std::cout << "Semantic analysis passed.\n";
}

void SemanticAnalyzer::analyze(ProgramNode* program) {
    if (!program) return;

    // Process variable declarations
    for (const auto& decl : program->var_decls) {
        for (const auto& name : decl->names) {
            if (symbol_table.count(name)) {
                reportError(0, "Duplicate variable declaration for '" + name + "'");
            }
            symbol_table[name] = decl->type;
        }
    }

    if (program->block) {
        analyzeBlock(program->block.get());
    }
}

void SemanticAnalyzer::analyzeBlock(BlockNode* node) {
    for (const auto& stmt : node->statements) {
        if (stmt) analyzeStatement(stmt.get());
    }
}

void SemanticAnalyzer::analyzeStatement(ASTNode* node) {
    if (auto n = dynamic_cast<AssignNode*>(node)) {
        analyzeAssign(n);
    } else if (auto n = dynamic_cast<IfNode*>(node)) {
        analyzeIf(n);
    } else if (auto n = dynamic_cast<WhileNode*>(node)) {
        analyzeWhile(n);
    } else if (auto n = dynamic_cast<ReadNode*>(node)) {
        analyzeRead(n);
    } else if (auto n = dynamic_cast<WriteNode*>(node)) {
        analyzeWrite(n);
    } else if (auto n = dynamic_cast<BlockNode*>(node)) {
        analyzeBlock(n);
    } else {
        reportError(0, "Unknown statement type in semantic analysis");
    }
}

void SemanticAnalyzer::analyzeAssign(AssignNode* node) {
    if (!symbol_table.count(node->ident)) {
        reportError(node->line_no, "Undeclared variable '" + node->ident + "'");
    }
    
    std::string lhs_type = symbol_table[node->ident];
    std::string rhs_type = getExprType(node->expr.get());

    if (lhs_type != rhs_type) {
        reportError(node->line_no, "Type mismatch in assignment to '" + node->ident + "' (expected " + lhs_type + ", got " + rhs_type + ")");
    }
}

void SemanticAnalyzer::analyzeIf(IfNode* node) {
    std::string cond_type = getExprType(node->condition.get());
    if (cond_type != "boolean") {
        // Find line number roughly by looking at the condition node
        reportError(0, "If condition must be boolean"); 
    }
    if (node->then_branch) analyzeStatement(node->then_branch.get());
    if (node->else_branch) analyzeStatement(node->else_branch.get());
}

void SemanticAnalyzer::analyzeWhile(WhileNode* node) {
    std::string cond_type = getExprType(node->condition.get());
    if (cond_type != "boolean") {
        reportError(0, "While condition must be boolean");
    }
    if (node->body) analyzeStatement(node->body.get());
}

void SemanticAnalyzer::analyzeRead(ReadNode* node) {
    for (const auto& ident : node->idents) {
        if (!symbol_table.count(ident)) {
            reportError(node->line_no, "Undeclared variable '" + ident + "' in read()");
        }
    }
}

void SemanticAnalyzer::analyzeWrite(WriteNode* node) {
    for (const auto& expr : node->exprs) {
       getExprType(expr.get()); // ensure it is valid
    }
}

std::string SemanticAnalyzer::getExprType(ASTNode* node) {
    if (auto n = dynamic_cast<NumberNode*>(node)) {
        return "integer";
    }
    if (auto n = dynamic_cast<BoolNode*>(node)) {
        return "boolean";
    }
    if (auto n = dynamic_cast<IdentNode*>(node)) {
        if (!symbol_table.count(n->name)) {
            reportError(n->line_no, "Undeclared variable '" + n->name + "'");
        }
        return symbol_table[n->name];
    }
    if (auto n = dynamic_cast<BinOpNode*>(node)) {
        std::string left_type = getExprType(n->left.get());
        std::string right_type = getExprType(n->right.get());
        
        if (n->op == "+" || n->op == "-" || n->op == "*" || n->op == "div" || n->op == "mod") {
            if (left_type != "integer" || right_type != "integer") {
                reportError(n->line_no, "Type mismatch in arithmetic operator " + n->op);
            }
            return "integer";
        }
        if (n->op == "<" || n->op == "<=" || n->op == ">" || n->op == ">=" || n->op == "=" || n->op == "<>") {
            if (left_type != right_type) {
                reportError(n->line_no, "Type mismatch in relational operator " + n->op);
            }
            return "boolean";
        }
        if (n->op == "and" || n->op == "or") {
            if (left_type != "boolean" || right_type != "boolean") {
                reportError(n->line_no, "Type mismatch in logical operator " + n->op);
            }
            return "boolean";
        }
    }
    if (auto n = dynamic_cast<UnaryOpNode*>(node)) {
        std::string type = getExprType(n->operand.get());
        if (n->op == "-") {
            if (type != "integer") reportError(n->line_no, "Type mismatch for unary operator -");
            return "integer";
        }
        if (n->op == "not") {
            if (type != "boolean") reportError(n->line_no, "Type mismatch for unary operator not");
            return "boolean";
        }
    }
    reportError(0, "Unknown expression type");
    return "unknown";
}
