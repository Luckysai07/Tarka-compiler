#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

class Visitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

class ProgramNode;
class VarDeclNode;
class BlockNode;
class AssignNode;
class IfNode;
class WhileNode;
class ReadNode;
class WriteNode;
class BinOpNode;
class UnaryOpNode;
class NumberNode;
class BoolNode;
class IdentNode;

// Helper function for printing indentation
inline void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
}

class ProgramNode : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<VarDeclNode>> var_decls;
    std::unique_ptr<BlockNode> block;

    ProgramNode(const std::string& n) : name(n) {}
    void print(int indent = 0) const override;
};

class VarDeclNode : public ASTNode {
public:
    std::vector<std::string> names;
    std::string type;

    VarDeclNode(const std::string& t) : type(t) {}
    void print(int indent = 0) const override;
};

class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void print(int indent = 0) const override;
};

class AssignNode : public ASTNode {
public:
    std::string ident;
    std::unique_ptr<ASTNode> expr;
    int line_no;

    AssignNode(const std::string& id, std::unique_ptr<ASTNode> e, int ln)
        : ident(id), expr(std::move(e)), line_no(ln) {}
    void print(int indent = 0) const override;
};

class IfNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> then_branch;
    std::unique_ptr<ASTNode> else_branch; // can be nullptr

    IfNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> tb, std::unique_ptr<ASTNode> eb)
        : condition(std::move(cond)), then_branch(std::move(tb)), else_branch(std::move(eb)) {}
    void print(int indent = 0) const override;
};

class WhileNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    
    WhileNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    void print(int indent = 0) const override;
};

class ReadNode : public ASTNode {
public:
    std::vector<std::string> idents;
    int line_no;

    ReadNode(int ln) : line_no(ln) {}
    void print(int indent = 0) const override;
};

class WriteNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> exprs;
    bool is_writeln;
    int line_no;

    WriteNode(bool ln, int lno) : is_writeln(ln), line_no(lno) {}
    void print(int indent = 0) const override;
};

class BinOpNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::string op;
    std::unique_ptr<ASTNode> right;
    int line_no;

    BinOpNode(std::unique_ptr<ASTNode> l, std::string o, std::unique_ptr<ASTNode> r, int ln)
        : left(std::move(l)), op(o), right(std::move(r)), line_no(ln) {}
    void print(int indent = 0) const override;
};

class UnaryOpNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> operand;
    int line_no;

    UnaryOpNode(std::string o, std::unique_ptr<ASTNode> opnd, int ln)
        : op(o), operand(std::move(opnd)), line_no(ln) {}
    void print(int indent = 0) const override;
};

class NumberNode : public ASTNode {
public:
    int value;
    int line_no;

    NumberNode(int v, int ln) : value(v), line_no(ln) {}
    void print(int indent = 0) const override;
};

class BoolNode : public ASTNode {
public:
    bool value;
    int line_no;

    BoolNode(bool v, int ln) : value(v), line_no(ln) {}
    void print(int indent = 0) const override;
};

class IdentNode : public ASTNode {
public:
    std::string name;
    int line_no;

    IdentNode(const std::string& n, int ln) : name(n), line_no(ln) {}
    void print(int indent = 0) const override;
};

// Inline implementations for print methods

inline void ProgramNode::print(int indent) const {
    printIndent(indent); std::cout << "Program(" << name << ")\n";
    for (const auto& decl : var_decls) decl->print(indent + 1);
    if (block) block->print(indent + 1);
}

inline void VarDeclNode::print(int indent) const {
    printIndent(indent); 
    std::cout << "VarDecl(Type: " << type << ")[";
    for (size_t i = 0; i < names.size(); ++i) {
        std::cout << names[i] << (i + 1 < names.size() ? ", " : "");
    }
    std::cout << "]\n";
}

inline void BlockNode::print(int indent) const {
    printIndent(indent); std::cout << "Block\n";
    for (const auto& stmt : statements) {
        if (stmt) stmt->print(indent + 1);
    }
}

inline void AssignNode::print(int indent) const {
    printIndent(indent); std::cout << "Assign(" << ident << ")\n";
    if (expr) expr->print(indent + 1);
}

inline void IfNode::print(int indent) const {
    printIndent(indent); std::cout << "If\n";
    printIndent(indent + 1); std::cout << "Condition:\n";
    if (condition) condition->print(indent + 2);
    printIndent(indent + 1); std::cout << "Then:\n";
    if (then_branch) then_branch->print(indent + 2);
    if (else_branch) {
        printIndent(indent + 1); std::cout << "Else:\n";
        else_branch->print(indent + 2);
    }
}

inline void WhileNode::print(int indent) const {
    printIndent(indent); std::cout << "While\n";
    printIndent(indent + 1); std::cout << "Condition:\n";
    if (condition) condition->print(indent + 2);
    printIndent(indent + 1); std::cout << "Body:\n";
    if (body) body->print(indent + 2);
}

inline void ReadNode::print(int indent) const {
    printIndent(indent); std::cout << "Read(";
    for (size_t i = 0; i < idents.size(); ++i) {
        std::cout << idents[i] << (i + 1 < idents.size() ? ", " : "");
    }
    std::cout << ")\n";
}

inline void WriteNode::print(int indent) const {
    printIndent(indent); std::cout << (is_writeln ? "Writeln" : "Write") << "\n";
    for (const auto& expr : exprs) {
        if (expr) expr->print(indent + 1);
    }
}

inline void BinOpNode::print(int indent) const {
    printIndent(indent); std::cout << "BinOp(" << op << ")\n";
    if (left) left->print(indent + 1);
    if (right) right->print(indent + 1);
}

inline void UnaryOpNode::print(int indent) const {
    printIndent(indent); std::cout << "UnaryOp(" << op << ")\n";
    if (operand) operand->print(indent + 1);
}

inline void NumberNode::print(int indent) const {
    printIndent(indent); std::cout << "Number(" << value << ")\n";
}

inline void BoolNode::print(int indent) const {
    printIndent(indent); std::cout << "Boolean(" << (value ? "true" : "false") << ")\n";
}

inline void IdentNode::print(int indent) const {
    printIndent(indent); std::cout << "Ident(" << name << ")\n";
}

#endif // AST_H
