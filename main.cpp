#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include "optimizer.h"
#include "interpreter.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file.pas>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // 1. Lexer Phase
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();
        
        std::cout << "========== LEXER OUTPUT ==========\n";
        for (const auto& t : tokens) {
            std::cout << "Token: " << Lexer::tokenTypeToString(t.type) 
                      << " \t| Lexeme: '" << t.lexeme 
                      << "' \t| Line: " << t.line << "\n";
        }

        // 2. Parser Phase
        Parser parser(tokens);
        auto ast = parser.parse();
        
        std::cout << "\n========== PARSER OUTPUT ==========\n";
        if (ast) {
            ast->print();
        }

        // 2.5 Optimization Phase
        Optimizer optimizer;
        optimizer.optimize(ast.get());
        
        std::cout << "\n========== OPTIMIZED AST ==========\n";
        if (ast) {
            ast->print();
        }

        // 3. Semantic Analysis Phase
        SemanticAnalyzer semanticAnalyzer;
        semanticAnalyzer.analyze(ast.get());
        std::cout << "\n";
        semanticAnalyzer.printSymbolTable();

        // 4. Code Generation Phase
        std::cout << "\n";
        CodeGenerator codeGen;
        codeGen.generate(ast.get(), "output.tac");

        // 5. Execution Phase
        std::cout << "\n========== PROGRAM OUTPUT ==========\n";
        Interpreter interpreter;
        if (argc >= 3) {
            interpreter.setInputFile(argv[2]);
        }
        interpreter.execute(ast.get());
        std::cout << "\n";

        std::cout << "\nCompilation Successful!\n";

    } catch (const std::exception& e) {
        std::cerr << "\n========== COMPILATION ERROR ==========\n";
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}
