#pragma once
#include "AST.h"
#include "Token.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "ErrorHandler.h"

// Enhanced error types for better diagnostics
// (enum class ErrorLevel dan ErrorCode dihapus, gunakan dari ErrorHandler.h)

struct DiagnosticMessage {
    ErrorLevel level;
    ErrorCode code;
    std::string message;
    std::string file;
    int line;
    int column;
    int endLine;
    int endColumn;
    std::string snippet; // Code snippet showing the error
    std::vector<std::string> suggestions; // Possible fixes
    
    DiagnosticMessage(ErrorLevel level, ErrorCode code, const std::string& message,
                     const std::string& file, int line, int column, int endLine = -1, int endColumn = -1)
        : level(level), code(code), message(message), file(file), line(line), column(column),
          endLine(endLine == -1 ? line : endLine), endColumn(endColumn == -1 ? column : endColumn) {}
    
    std::string toString() const {
        std::ostringstream oss;
        
        // Color codes for different error levels
        std::string levelColor;
        std::string levelName;
        switch (level) {
            case ErrorLevel::INFO:    levelColor = "\033[36m"; levelName = "info"; break;
            case ErrorLevel::WARNING: levelColor = "\033[33m"; levelName = "warning"; break;
            case ErrorLevel::ERROR:   levelColor = "\033[31m"; levelName = "error"; break;
            case ErrorLevel::FATAL:   levelColor = "\033[35m"; levelName = "fatal"; break;
        }
        
        oss << levelColor << levelName << "\033[0m";
        oss << "[E" << static_cast<int>(code) << "]: " << message << "\n";
        oss << "  --> " << file << ":" << line << ":" << column << "\n";
        
        if (!snippet.empty()) {
            oss << "   |\n";
            oss << std::setw(3) << line << " | " << snippet << "\n";
            oss << "   | " << std::string(column - 1, ' ') << "\033[31m^\033[0m\n";
        }
        
        if (!suggestions.empty()) {
            oss << "   |\n";
            oss << "   = help: ";
            for (size_t i = 0; i < suggestions.size(); ++i) {
                if (i > 0) oss << "\n          ";
                oss << suggestions[i];
            }
            oss << "\n";
        }
        
        return oss.str();
    }
};

// Symbol table for scope management
struct Symbol {
    std::string name;
    std::shared_ptr<TypeInfo> type;
    bool isMutable;
    bool isInitialized;
    int declarationLine;
    int declarationColumn;
    
    Symbol() = default;
    
    Symbol(const std::string& name, std::shared_ptr<TypeInfo> type, bool isMutable = false,
           bool isInitialized = true, int line = 0, int column = 0)
        : name(name), type(type), isMutable(isMutable), isInitialized(isInitialized),
          declarationLine(line), declarationColumn(column) {}
};

class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    
public:
    void pushScope() {
        scopes.emplace_back();
    }
    
    void popScope() {
        if (!scopes.empty()) {
            scopes.pop_back();
        }
    }
    
    bool declare(const Symbol& symbol) {
        if (scopes.empty()) return false;
        
        auto& currentScope = scopes.back();
        if (currentScope.find(symbol.name) != currentScope.end()) {
            return false; // Already declared in current scope
        }
        
        currentScope[symbol.name] = symbol;
        return true;
    }
    
    Symbol* lookup(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return &found->second;
            }
        }
        return nullptr;
    }
    
    Symbol* lookupCurrentScope(const std::string& name) {
        if (scopes.empty()) return nullptr;
        
        auto& currentScope = scopes.back();
        auto found = currentScope.find(name);
        return found != currentScope.end() ? &found->second : nullptr;
    }
    
    bool updateSymbol(const std::string& name, const Symbol& newSymbol) {
        Symbol* existing = lookup(name);
        if (existing) {
            *existing = newSymbol;
            return true;
        }
        return false;
    }
    
    size_t scopeDepth() const { return scopes.size(); }
};

// Type registry for user-defined types
class TypeRegistry {
private:
    std::unordered_map<std::string, std::shared_ptr<StructDeclAST>> structs;
    std::unordered_map<std::string, std::shared_ptr<EnumDeclAST>> enums;
    std::unordered_map<std::string, std::shared_ptr<TraitDeclAST>> traits;
    
public:
    void registerStruct(const std::string& name, std::shared_ptr<StructDeclAST> structDecl) {
        structs[name] = structDecl;
    }
    
    void registerEnum(const std::string& name, std::shared_ptr<EnumDeclAST> enumDecl) {
        enums[name] = enumDecl;
    }
    
    void registerTrait(const std::string& name, std::shared_ptr<TraitDeclAST> traitDecl) {
        traits[name] = traitDecl;
    }
    
    std::shared_ptr<StructDeclAST> getStruct(const std::string& name) {
        auto it = structs.find(name);
        return it != structs.end() ? it->second : nullptr;
    }
    
    std::shared_ptr<EnumDeclAST> getEnum(const std::string& name) {
        auto it = enums.find(name);
        return it != enums.end() ? it->second : nullptr;
    }
    
    std::shared_ptr<TraitDeclAST> getTrait(const std::string& name) {
        auto it = traits.find(name);
        return it != traits.end() ? it->second : nullptr;
    }
    

    
    bool isUserDefinedType(const std::string& name) {
        return structs.count(name) || enums.count(name) || 
               traits.count(name);
    }
};

// Main semantic analyzer class
class SemanticAnalyzer {
private:
    std::vector<DiagnosticMessage> diagnostics;
    SymbolTable symbolTable;
    TypeRegistry typeRegistry;
    std::string currentFile;
    std::vector<std::string> sourceLines;
    
    // Function analysis state
    std::shared_ptr<TypeInfo> currentFunctionReturnType;
    bool hasReturnStatement;
    int loopDepth;
    
    // Module analysis state
    std::unordered_set<std::string> importedModules;
    std::unordered_map<std::string, std::string> moduleAliases;
    
    // Error reporting helpers
    void reportError(ErrorCode code, const std::string& message, int line, int column,
                    int endLine = -1, int endColumn = -1);
    void reportWarning(ErrorCode code, const std::string& message, int line, int column,
                      int endLine = -1, int endColumn = -1);
    void reportInfo(const std::string& message, int line, int column);
    
    std::string getCodeSnippet(int line) const;
    void addSuggestion(const std::string& suggestion);
    
    // Type checking
    bool isAssignable(const TypeInfo& from, const TypeInfo& to);
    std::shared_ptr<TypeInfo> inferType(std::shared_ptr<ExprAST> expr);
    bool checkTypeCompatibility(const TypeInfo& expected, const TypeInfo& actual);
    
    // Built-in type checking
    bool isNumericType(FlastType type);
    bool isIntegerType(FlastType type);
    bool isFloatType(FlastType type);
    bool isBuiltinType(FlastType type);
    
    // Pattern matching validation
    bool isExhaustiveMatch(std::shared_ptr<MatchStmtAST> matchStmt);
    bool isReachablePattern(std::shared_ptr<ExprAST> pattern,
                           const std::vector<std::shared_ptr<ExprAST>>& previousPatterns);
    
    // Control flow analysis
    void analyzeControlFlow(std::shared_ptr<StmtAST> stmt);
    bool hasUnreachableCode(const std::vector<std::shared_ptr<StmtAST>>& statements);
    
    // Ownership and borrowing analysis (Rust-like)
    void analyzeOwnership(std::shared_ptr<ExprAST> expr);
    bool isMovedValue(const std::string& varName);
    void markAsMoved(const std::string& varName);
    
public:
    SemanticAnalyzer(const std::string& file, const std::vector<std::string>& sourceLines);
    
    // Main analysis entry points
    bool analyze(std::shared_ptr<ProgramAST> program);
    bool analyzeDeclaration(std::shared_ptr<DeclAST> decl);
    bool analyzeStatement(std::shared_ptr<StmtAST> stmt);
    bool analyzeExpression(std::shared_ptr<ExprAST> expr);
    
    // Specific analyzers
    bool analyzeFunctionDecl(std::shared_ptr<FunctionDeclAST> funcDecl);
    bool analyzeStructDecl(std::shared_ptr<StructDeclAST> structDecl);
    bool analyzeEnumDecl(std::shared_ptr<EnumDeclAST> enumDecl);
    bool analyzeTraitDecl(std::shared_ptr<TraitDeclAST> traitDecl);
    bool analyzeImplDecl(std::shared_ptr<ImplDeclAST> implDecl);

    bool analyzeImportDecl(std::shared_ptr<ImportDeclAST> importDecl);
    
    bool analyzeVarDecl(std::shared_ptr<VarDeclStmtAST> varDecl);
    bool analyzeAssignment(std::shared_ptr<AssignStmtAST> assignment);
    bool analyzeIfStmt(std::shared_ptr<IfStmtAST> ifStmt);
    bool analyzeWhileStmt(std::shared_ptr<WhileStmtAST> whileStmt);
    bool analyzeForStmt(std::shared_ptr<ForStmtAST> forStmt);
    bool analyzeMatchStmt(std::shared_ptr<MatchStmtAST> matchStmt);
    bool analyzeReturnStmt(std::shared_ptr<ReturnStmtAST> returnStmt);
    
    bool analyzeBinaryExpr(std::shared_ptr<BinaryExprAST> binaryExpr);
    bool analyzeUnaryExpr(std::shared_ptr<UnaryExprAST> unaryExpr);
    bool analyzeCallExpr(std::shared_ptr<CallExprAST> callExpr);
    bool analyzeMemberAccess(std::shared_ptr<MemberAccessExprAST> memberAccess);
    bool analyzeVariableExpr(std::shared_ptr<VariableExprAST> varExpr);
    
    // Utility methods
    bool hasErrors() const;
    bool hasWarnings() const;
    size_t getErrorCount() const;
    size_t getWarningCount() const;
    
    void printDiagnostics() const;
    std::vector<DiagnosticMessage> getDiagnostics() const { return diagnostics; }
    
    // Configuration
    void setStrictMode(bool strict) { strictMode = strict; }
    void setWarningsAsErrors(bool warningsAsErrors) { this->warningsAsErrors = warningsAsErrors; }
    
private:
    bool strictMode = false;
    bool warningsAsErrors = false;
    std::unordered_set<std::string> movedValues;
}; 