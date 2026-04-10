# Tarka Compiler & Web IDE

**Tarka** is a custom, statically-typed instructional programming language featuring a modern, C/JavaScript-style syntax. It is built entirely from scratch in C++ and powered by a highly interactive Next.js Web IDE. The project demonstrates a full compiler pipeline, from tokenization to code execution, coupled with a professional, CodeChef-inspired developer interface.

## 🚀 Key Features

### The Language & Compiler (C++)
*   **Modern Syntax**: Designed to be intuitive and clean (e.g., using `{}` for blocks, `var` for declarations), departing from rigid boilerplate legacy syntaxes.
*   **Complete Pipeline**: 
    - **Lexer**: Efficient string-to-token parsing mapping custom keywords, operators, and constants.
    - **Parser**: Hand-tailored Recursive Descent parsing to build an Abstract Syntax Tree (AST).
    - **Semantic Analyzer**: Type checks and verifies variable declarations dynamically.
    - **Optimizer**: Evaluates constant folding at compile-time to streamline execution.
    - **Interpreter**: A custom tree-walking engine capable of directly executing AST logic, including recursive math and conditional branching.
    - **Code Generator**: Safely transpiles AST structures down to custom Three-Address Code (TAC) bytecode.
*   **Non-Blocking I/O**: Safely handles standard input piping directly via `input.txt` streams.

### The Developer Environment (Next.js & React)
*   **CodeChef Layout**: A beautiful, industry-standard split-pane view that allows programmers to edit code efficiently without excessive scrolling.
*   **Dynamic Resizing**: Built using `react-resizable-panels`, ensuring developers can horizontally and vertically drag all splitters to maximize their workspace intuitively.
*   **Deep Transparency**: Dedicated inspection tabs for every layer of the compiler pipeline! Dive under the hood in real-time to view:
    *   `RAW` Console Execution Outputs
    *   `LEXER` Token Maps
    *   `PARSER` Abstract Syntax Trees
    *   `OPTIMIZED` Post-folded Code Trees
    *   `TAC Bytes` Three-Address Code Generations

## 🛠️ Architecture Setup 

**Prerequisites:** 
- `Node.js` (for the React Frontend)
- `MSYS2 UCRT64` GCC Toolchain (for the C++ Compiler backend)
- `mingw32-make` must be enabled in your Windows PATH.

### 1. Build the Backend C++ Engine
```bash
# In the root repository directory
mingw32-make clean
mingw32-make
```

### 2. Start the Frontend Next.js IDE
```bash
# Enter the web workspace
cd web/
npm install
npm run dev
```

Navigate to `http://localhost:3000` to begin coding in Tarka!

## 📝 About the Name
*Tarka* represents clarity and logic. The engine forces programmers to think structurally while the Next.js shell handles the heavy-lifting of compilation mapping, ensuring that students and developers alike can comfortably learn compilation theory step-by-step.
