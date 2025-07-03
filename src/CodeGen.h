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
#include <functional>

class CodeGenerator {
private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::DIBuilder> debugBuilder;
    
    // Builtin system
    std::unordered_map<std::string, std::function<llvm::Value*(const std::vector<llvm::Value*>&)>> builtinFunctions;
    std::unordered_map<std::string, std::unordered_map<std::string, std::function<llvm::Value*(llvm::Value*, const std::vector<llvm::Value*>&)>>> builtinMethods;
    
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
    
    // Module system
    std::unordered_map<std::string, std::shared_ptr<ProgramAST>> moduleCache;
    std::unordered_map<std::string, std::filesystem::path> moduleCachePaths; // Track cache paths for each module
    std::unordered_map<std::string, std::string> moduleObjectFiles; // Track object files for each module
    std::filesystem::path currentSourceDir;
    std::vector<std::string> missingModules; // Track missing modules for reporting
    
    // Built-in functions
    llvm::Function* printlnFunc;
    llvm::Function* mallocFunc;
    llvm::Function* freeFunc;
    
    void setupProjectStructure(const std::string& sourceFile);
    void createBuiltinFunctions();
    void setupDebugInfo(const std::string& sourceFile);
    llvm::Type* getFlastType(const std::string& typeName);
    
    // Builtin system methods
    void registerBuiltinFunctions();
    void registerBuiltinMethods();
    llvm::Value* callBuiltinFunction(const std::string& name, const std::vector<llvm::Value*>& args);
    llvm::Value* callBuiltinMethod(const std::string& type, const std::string& method, llvm::Value* object, const std::vector<llvm::Value*>& args);
    std::string getTypeName(llvm::Value* value);
    
    // Code generation methods
    llvm::Value* codegen(ExprAST* expr);
    llvm::Value* codegen(StmtAST* stmt);
    llvm::Function* codegen(FunctionDeclAST* func);
    llvm::StructType* codegen(ClassDeclAST* cls);
    void codegen(ImportDeclAST* importDecl);
    
    // Module loading
    std::string resolveModulePath(const std::string& importPath, const std::string& currentDir);
    std::shared_ptr<ProgramAST> loadModule(const std::string& modulePath);
    void processImportedFunctions(std::shared_ptr<ProgramAST> module, const std::vector<std::string>& specificImports, bool isWildcard);
    
    llvm::Value* codegenNumber(NumberExprAST* expr);
    llvm::Value* codegenScientific(ScientificExprAST* expr);
    llvm::Value* codegenString(StringExprAST* expr);
    llvm::Value* codegenVariable(VariableExprAST* expr);
    llvm::Value* codegenBinary(BinaryExprAST* expr);
    llvm::Value* codegenCall(CallExprAST* expr);
    llvm::Value* codegenMemberAccess(MemberAccessExprAST* expr);
    llvm::Value* codegenMethodCall(MethodCallExprAST* expr);
    llvm::Value* codegenNew(NewExprAST* expr);
    
    llvm::Value* codegenVarDecl(VarDeclStmtAST* stmt);
    llvm::Value* codegenAssign(AssignStmtAST* stmt);
    llvm::Value* codegenExprStmt(ExprStmtAST* stmt);
    llvm::Value* codegenReturn(ReturnStmtAST* returnStmt);
    llvm::Value* codegenWhile(WhileStmtAST* whileStmt);
    llvm::Value* codegenFor(ForStmtAST* forStmt);
    llvm::Value* codegenForIn(ForInStmtAST* forInStmt);
    llvm::Value* codegenBlock(BlockStmtAST* blockStmt);
    
    // Module-specific caching
    std::string getModuleCacheFileName(const std::string& modulePath);
    std::filesystem::path getModuleCacheDir(const std::string& modulePath);
    void createModuleCacheStructure(const std::string& modulePath);
    bool isModuleCacheValid(const std::string& modulePath);
    void saveModuleCache(const std::string& modulePath, std::shared_ptr<ProgramAST> moduleAst);
    std::shared_ptr<ProgramAST> loadModuleFromCache(const std::string& modulePath);
    
    // Module object file generation
    std::string getModuleObjectFileName(const std::string& modulePath);
    void generateModuleObjectFile(const std::string& modulePath, std::shared_ptr<ProgramAST> moduleAst);
    bool isModuleObjectValid(const std::string& modulePath);
    std::vector<std::string> collectModuleObjectFiles();
    
    // Missing modules reporting
    void reportMissingModules();
    
public:
    CodeGenerator();
    void generateCode(ProgramAST* program, const std::string& sourceFile);
    void printIR();
    void writeObjectFile(const std::string& filename);
    std::string writeExecutable(const std::string& sourceFile, bool debugMode = true, bool optimized = false);
    void cleanupCache();
}; 