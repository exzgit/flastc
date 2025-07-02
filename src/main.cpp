#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    return content;
}

void printUsage(const char* programName) {
    std::cout << "FLAST PROFESSIONAL COMPILER v1.0\n";
    std::cout << "Usage: " << programName << " <input.fls> [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o <output>    Set output name (default: auto-generated)\n";
    std::cout << "  --release      Release build (optimized, no debug)\n";
    std::cout << "  --debug        Debug build (default, with debug info)\n";
    std::cout << "  --clean        Clean cache only (build dir auto-cleaned)\n";
    std::cout << "  --ir           Print LLVM IR instead of compiling\n";
    std::cout << "  --tokens       Print tokens instead of compiling\n";
    std::cout << "  --ast          Print AST instead of compiling\n";
    std::cout << "  -h, --help     Show this help message\n\n";
    std::cout << "Output Structure:\n";
    std::cout << "  .build/bin/    - Executable files (platform-specific extension)\n";
    std::cout << "  .build/cache/  - Temporary files (.o, etc)\n";
    std::cout << "  Note: .build/ directory is automatically cleaned on each compilation\n\n";
    std::cout << "Platform Support:\n";
    std::cout << "  Linux:   No extension (e.g., 'program')\n";
    std::cout << "  Windows: .exe extension (e.g., 'program.exe')\n";
    std::cout << "  macOS:   No extension (e.g., 'program')\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " program.fls                 # Debug build\n";
    std::cout << "  " << programName << " program.fls --release       # Optimized build\n";
    std::cout << "  " << programName << " program.fls -o myapp        # Custom name\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Check for help first
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }
    
    std::string inputFile = argv[1];
    std::string outputName = "";
    bool debugMode = true;
    bool optimized = false;
    bool cleanCache = false;
    bool printIR = false;
    bool printTokens = false;
    bool printAST = false;
    
    // Parse command line arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-o" && i + 1 < argc) {
            outputName = argv[++i];
        } else if (arg == "--release") {
            debugMode = false;
            optimized = true;
        } else if (arg == "--debug") {
            debugMode = true;
            optimized = false;
        } else if (arg == "--clean") {
            cleanCache = true;
        } else if (arg == "--ir") {
            printIR = true;
        } else if (arg == "--tokens") {
            printTokens = true;
        } else if (arg == "--ast") {
            printAST = true;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    try {
        // Validate input file
        if (!std::filesystem::exists(inputFile)) {
            throw std::runtime_error("Input file does not exist: " + inputFile);
        }
        
        // Read source file
        std::string source = readFile(inputFile);
        
        // Lexical analysis
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        if (printTokens) {
            std::cout << "=== TOKENS ===" << std::endl;
            for (const auto& token : tokens) {
                std::cout << tokenTypeToString(token.type) << ": " << token.value 
                         << " (line " << token.line << ", col " << token.column << ")" << std::endl;
            }
            return 0;
        }
        
        // Syntax analysis
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        if (printAST) {
            std::cout << "=== AST ===" << std::endl;
            std::cout << ast->toString() << std::endl;
            return 0;
        }
        
        // Code generation
        CodeGenerator codegen;
        
        // Clean cache if requested
        if (cleanCache) {
            codegen.cleanupCache();
        }
        
        // Generate code with source file path
        codegen.generateCode(ast.get(), inputFile);
        
        if (printIR) {
            std::cout << "=== LLVM IR ===" << std::endl;
            codegen.printIR();
            return 0;
        }
        
        // Compile to executable with professional output
        std::string exePath = codegen.writeExecutable(inputFile, debugMode, optimized);
        
        // Show final result
        std::cout << "\n=== BUILD COMPLETE ===" << std::endl;
        std::cout << "✓ Executable: " << exePath << std::endl;
        std::cout << "✓ Build type: " << (debugMode ? "Debug" : "Release") << std::endl;
        
        if (debugMode) {
            std::cout << "✓ Debug symbols: Enabled" << std::endl;
        }
        if (optimized) {
            std::cout << "✓ Optimization: Enabled" << std::endl;
        }
        
        std::cout << "\nRun with: " << exePath << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Compilation failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 