#pragma once
#include "AST.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <unordered_map>
#include <memory>
#include <filesystem>

class CodeGenerator {
private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::DIBuilder> debugBuilder;
    
    // Project structure
    std::filesystem::path projectRoot;
    std::filesystem::path buildDir;
    std::filesystem::path binDir;
    std::filesystem::path cacheDir;
    
    // Debug info
    llvm::DICompileUnit* debugCompileUnit;
    llvm::DIFile* debugFile;
    
    // Symbol tables
    std::unordered_map<std::string, llvm::Value*> namedValues;
    std::unordered_map<std::string, llvm::Function*> functions;
    std::unordered_map<std::string, llvm::StructType*> classes;
    
    // Built-in functions
    llvm::Function* printlnFunc;
    llvm::Function* mallocFunc;
    llvm::Function* freeFunc;
    
    void setupProjectStructure(const std::string& sourceFile);
    void createBuiltinFunctions();
    void setupDebugInfo(const std::string& sourceFile);
    llvm::Type* getFlastType(const std::string& typeName);
    
    // Code generation methods
    llvm::Value* codegen(ExprAST* expr);
    llvm::Value* codegen(StmtAST* stmt);
    llvm::Function* codegen(FunctionAST* func);
    llvm::StructType* codegen(ClassAST* cls);
    
    llvm::Value* codegenNumber(NumberExprAST* expr);
    llvm::Value* codegenString(StringExprAST* expr);
    llvm::Value* codegenVariable(VariableExprAST* expr);
    llvm::Value* codegenBinary(BinaryExprAST* expr);
    llvm::Value* codegenCall(CallExprAST* expr);
    llvm::Value* codegenMemberAccess(MemberAccessExprAST* expr);
    llvm::Value* codegenNew(NewExprAST* expr);
    
    llvm::Value* codegenVarDecl(VarDeclStmtAST* stmt);
    llvm::Value* codegenAssign(AssignStmtAST* stmt);
    llvm::Value* codegenExprStmt(ExprStmtAST* stmt);
    llvm::Value* codegenReturn(ReturnStmtAST* stmt);
    
public:
    CodeGenerator();
    void generateCode(ProgramAST* program, const std::string& sourceFile);
    void printIR();
    void writeObjectFile(const std::string& filename);
    std::string writeExecutable(const std::string& sourceFile, bool debugMode = true, bool optimized = false);
    void cleanupCache();
}; 