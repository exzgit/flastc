#include "CodeGen.h"
#include "Lexer.h"
#include "Parser.h"
#include <llvm/IR/Verifier.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <ctime>

CodeGenerator::CodeGenerator() : debugCompileUnit(nullptr), debugFile(nullptr) {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("flast", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    debugBuilder = std::make_unique<llvm::DIBuilder>(*module);
    
    createBuiltinFunctions();
    registerBuiltinFunctions();
    registerBuiltinMethods();
}

void CodeGenerator::setupProjectStructure(const std::string& sourceFile) {
    // Get source file directory as project root
    projectRoot = std::filesystem::absolute(std::filesystem::path(sourceFile)).parent_path();
    currentSourceDir = projectRoot; // Set current source directory for module resolution
    
    // Create build structure
    buildDir = projectRoot / ".build";
    binDir = buildDir / "bin";
    cacheDir = buildDir / "cache";
    
    // Only clean main cache directory, preserve module caches
    if (std::filesystem::exists(cacheDir)) {
        std::cout << "🧹 Cleaning main cache directory..." << std::endl;
        try {
            // Only remove main cache files, not module subdirectories
            for (const auto& entry : std::filesystem::directory_iterator(cacheDir)) {
                if (entry.is_regular_file()) {
                    std::filesystem::remove(entry);
                }
            }
            std::cout << "✓ Removed old main cache files (preserving module caches)" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "⚠️  Warning: Could not fully clean cache directory: " << e.what() << std::endl;
        }
    }
    
    // Create directories (will not overwrite existing binaries)
    std::filesystem::create_directories(binDir);
    std::filesystem::create_directories(cacheDir);
    
    std::cout << "📁 Project structure:" << std::endl;
    std::cout << "  Root: " << projectRoot << std::endl;
    std::cout << "  Build: " << buildDir << std::endl;
    std::cout << "  Binaries: " << binDir << " (preserved)" << std::endl;
    std::cout << "  Cache: " << cacheDir << " (module-specific)" << std::endl;
}

void CodeGenerator::setupDebugInfo(const std::string& sourceFile) {
    auto filename = std::filesystem::path(sourceFile).filename().string();
    auto directory = std::filesystem::absolute(std::filesystem::path(sourceFile)).parent_path().string();
    
    // Create debug file
    debugFile = debugBuilder->createFile(filename, directory);
    
    // Create compile unit
    debugCompileUnit = debugBuilder->createCompileUnit(
        llvm::dwarf::DW_LANG_C,  // Language
        debugFile,               // File
        "Flast Compiler v1.0",   // Producer
        false,                   // Optimized
        "",                      // Flags
        0                        // Runtime version
    );
    
    module->addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);
    module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 4);
}

void CodeGenerator::createBuiltinFunctions() {
    // Create printf function
    std::vector<llvm::Type*> printfArgs;
    printfArgs.push_back(llvm::Type::getInt8PtrTy(*context));
    
    llvm::FunctionType* printfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context), printfArgs, true);
    
    llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, 
                          "printf", module.get());
    
    // Create println function (wrapper for printf)
    std::vector<llvm::Type*> printlnArgs;
    printlnArgs.push_back(llvm::Type::getInt8PtrTy(*context));
    
    llvm::FunctionType* printlnType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*context), printlnArgs, true);
    
    printlnFunc = llvm::Function::Create(printlnType, llvm::Function::ExternalLinkage,
                                        "println", module.get());
    
    // Create malloc function
    std::vector<llvm::Type*> mallocArgs;
    mallocArgs.push_back(llvm::Type::getInt64Ty(*context));
    
    llvm::FunctionType* mallocType = llvm::FunctionType::get(
        llvm::Type::getInt8PtrTy(*context), mallocArgs, false);
    
    mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage,
                                       "malloc", module.get());
    
    // Create free function
    std::vector<llvm::Type*> freeArgs;
    freeArgs.push_back(llvm::Type::getInt8PtrTy(*context));
    
    llvm::FunctionType* freeType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*context), freeArgs, false);
    
    freeFunc = llvm::Function::Create(freeType, llvm::Function::ExternalLinkage,
                                     "free", module.get());
}

llvm::Type* CodeGenerator::getFlastType(const std::string& typeName) {
    // Integer types
    if (typeName == "i8") {
        return llvm::Type::getInt8Ty(*context);
    } else if (typeName == "i16") {
        return llvm::Type::getInt16Ty(*context);
    } else if (typeName == "int" || typeName == "i32") {
        return llvm::Type::getInt32Ty(*context);
    } else if (typeName == "i64") {
        return llvm::Type::getInt64Ty(*context);
    }
    // Unsigned integer types (treat same as signed for now)
    else if (typeName == "u8") {
        return llvm::Type::getInt8Ty(*context);
    } else if (typeName == "u16") {
        return llvm::Type::getInt16Ty(*context);
    } else if (typeName == "u32") {
        return llvm::Type::getInt32Ty(*context);
    } else if (typeName == "u64") {
        return llvm::Type::getInt64Ty(*context);
    }
    // Floating-point types
    else if (typeName == "f32") {
        return llvm::Type::getFloatTy(*context);
    } else if (typeName == "double" || typeName == "f64") {
        return llvm::Type::getDoubleTy(*context);
    }
    // Other types
    else if (typeName == "bool") {
        return llvm::Type::getInt1Ty(*context);
    } else if (typeName == "char") {
        return llvm::Type::getInt8Ty(*context);
    } else if (typeName == "string" || typeName == "str") {
        return llvm::Type::getInt8PtrTy(*context);
    } else if (typeName == "void") {
        return llvm::Type::getVoidTy(*context);
    } else {
        // Check if it's a class type
        auto it = structs.find(typeName);
        if (it != structs.end()) {
            return llvm::PointerType::get(it->second, 0);
        }
        // Default to i32
        return llvm::Type::getInt32Ty(*context);
    }
}

void CodeGenerator::generateCode(ProgramAST* program, const std::string& sourceFile) {
    setupProjectStructure(sourceFile);
    // Temporarily disable debug info to fix compilation issues
    // setupDebugInfo(sourceFile);
    
    std::cout << "🔍 Total declarations found: " << program->declarations.size() << std::endl;
    
    // First pass: Generate all struct types (forward declarations)
    std::cout << "🔍 First pass: Processing struct types..." << std::endl;
    for (auto& decl : program->declarations) {
        if (auto structDecl = dynamic_cast<StructDeclAST*>(decl.get())) {
            std::cout << "🔍 Processing struct: " << structDecl->name << std::endl;
            // Create struct type first
            std::vector<llvm::Type*> memberTypes;
            for (auto& field : structDecl->fields) {
                memberTypes.push_back(getFlastType(field.second->toString()));
            }
            
            llvm::StructType* structType = llvm::StructType::create(*context, structDecl->name);
            structType->setBody(memberTypes);
            structs[structDecl->name] = structType;
        }
    }
    
    // Process imports first
    std::cout << "🔍 Second pass: Processing imports..." << std::endl;
    for (auto& decl : program->declarations) {
        if (auto importDecl = dynamic_cast<ImportDeclAST*>(decl.get())) {
            std::cout << "🔍 Processing import: " << importDecl->moduleName << std::endl;
            try {
                codegen(importDecl);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Import failed: " << e.what() << std::endl;
            }
        }
    }
    
    // Report missing modules after all imports are processed
    reportMissingModules();
    
    // Second pass: Generate method implementations
    std::cout << "🔍 Third pass: Processing functions..." << std::endl;
    for (auto& decl : program->declarations) {
        std::cout << "🔍 Checking declaration type..." << std::endl;
        if (auto structDecl = dynamic_cast<StructDeclAST*>(decl.get())) {
            std::cout << "🔍 Processing struct methods for: " << structDecl->name << std::endl;
            // Generate methods (if any)
            // Structs don't have methods by default, they use impl blocks
        } else if (auto funcDecl = dynamic_cast<FunctionDeclAST*>(decl.get())) {
            std::cout << "🔍 Processing function: " << funcDecl->name << std::endl;
            codegen(funcDecl);
        } else {
            std::cout << "🔍 Unknown declaration type (skipped)" << std::endl;
        }
    }
    
    std::cout << "🔍 Code generation completed!" << std::endl;
    
    // Finalize debug info only if enabled
    // debugBuilder->finalize();
}



llvm::Function* CodeGenerator::codegen(FunctionDeclAST* func) {
    // Create function signature
    std::vector<llvm::Type*> argTypes;
    for (auto& param : func->parameters) {
        argTypes.push_back(getFlastType(param.type->toString()));
    
    }
    
    // Handle special case for constructor (return type "self")
    llvm::Type* returnType;
    if (func->returnType->toString() == "self") {
        // Constructor returns a pointer to the object (generic pointer for now)
        returnType = llvm::Type::getInt8PtrTy(*context);
    } else {
        returnType = getFlastType(func->returnType->toString());
    }
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, argTypes, false);
    
    llvm::Function* function = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, func->name, module.get());
    
    functions[func->name] = function;
    
    // Create entry block
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(entry);
    
    // Create allocas for parameters
    auto argIt = function->arg_begin();
    for (size_t i = 0; i < func->parameters.size(); ++i) {
        llvm::AllocaInst* alloca = builder->CreateAlloca(
            getFlastType(func->parameters[i].type->toString()), nullptr, func->parameters[i].name);
        
        builder->CreateStore(&(*argIt), alloca);
        namedValues[func->parameters[i].name] = alloca;
        ++argIt;
    }
    
    // Check if this is a constructor (return type "self")
    bool isConstructor = func->returnType->toString() == "self";
    
    // For class methods, always provide 'self' parameter
    // TODO: In a proper implementation, this should be passed as first parameter
    // For now, create a placeholder 'self' for all class methods
    llvm::Type* objectType = llvm::Type::getInt8PtrTy(*context); // Generic pointer for now
    llvm::AllocaInst* selfAlloca = builder->CreateAlloca(objectType, nullptr, "self");
    
    // Initialize self with a null pointer initially
    llvm::Value* nullPtr = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(*context));
    builder->CreateStore(nullPtr, selfAlloca);
    
    namedValues["self"] = selfAlloca;
    
    // Generate function body
    llvm::Value* retVal = nullptr;
    if (func->body) {
        for (auto& stmt : func->body->statements) {
            retVal = codegen(stmt.get());
        }
    } else {
        // Abstract method - no implementation
        // For now, just create a basic return
    }
    
    // Add return if not present
    if (!retVal || !llvm::isa<llvm::ReturnInst>(retVal)) {
        if (func->returnType->toString() == "void") {
            builder->CreateRetVoid();
        } else if (func->returnType->toString() == "self") {
            // Constructor: return the self pointer
            llvm::Value* selfVar = namedValues["self"];
            if (selfVar) {
                llvm::Value* selfVal = builder->CreateLoad(selfVar->getType()->getPointerElementType(), selfVar, "selfret");
                builder->CreateRet(selfVal);
            } else {
                // Return null pointer as fallback
                builder->CreateRet(llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(*context)));
            }
        } else {
            builder->CreateRet(llvm::ConstantInt::get(*context, llvm::APInt(32, 0)));
        }
    }
    
    // Clean up named values for this function scope
    // Remove function parameters and local variables from global scope
    for (auto& param : func->parameters) {
        namedValues.erase(param.name);
    }
    // Always clean up 'self' since all functions have it now
    namedValues.erase("self");
    
    // Verify function
    if (llvm::verifyFunction(*function, &llvm::errs())) {
        function->eraseFromParent();
        throw std::runtime_error("Function verification failed");
    }
    
    return function;
}

llvm::Value* CodeGenerator::codegen(ExprAST* expr) {
    if (auto numberExpr = dynamic_cast<NumberExprAST*>(expr)) {
        return codegenNumber(numberExpr);
    } else if (auto scientificExpr = dynamic_cast<ScientificExprAST*>(expr)) {
        return codegenScientific(scientificExpr);
    } else if (auto stringExpr = dynamic_cast<StringExprAST*>(expr)) {
        return codegenString(stringExpr);
    } else if (auto varExpr = dynamic_cast<VariableExprAST*>(expr)) {
        return codegenVariable(varExpr);
    } else if (auto binaryExpr = dynamic_cast<BinaryExprAST*>(expr)) {
        return codegenBinary(binaryExpr);
    } else if (auto callExpr = dynamic_cast<CallExprAST*>(expr)) {
        return codegenCall(callExpr);
    } else if (auto memberExpr = dynamic_cast<MemberAccessExprAST*>(expr)) {
        return codegenMemberAccess(memberExpr);
    } else if (auto methodCallExpr = dynamic_cast<MethodCallExprAST*>(expr)) {
        return codegenMethodCall(methodCallExpr);
    } else if (auto newExpr = dynamic_cast<NewExprAST*>(expr)) {
        return codegenNew(newExpr);
    }
    
    throw std::runtime_error("Unknown expression type");
}

llvm::Value* CodeGenerator::codegen(StmtAST* stmt) {
    if (auto varDeclStmt = dynamic_cast<VarDeclStmtAST*>(stmt)) {
        return codegenVarDecl(varDeclStmt);
    } else if (auto assignStmt = dynamic_cast<AssignStmtAST*>(stmt)) {
        return codegenAssign(assignStmt);
    } else if (auto exprStmt = dynamic_cast<ExprStmtAST*>(stmt)) {
        return codegenExprStmt(exprStmt);
    } else if (auto returnStmt = dynamic_cast<ReturnStmtAST*>(stmt)) {
        return codegenReturn(returnStmt);
    } else if (auto whileStmt = dynamic_cast<WhileStmtAST*>(stmt)) {
        return codegenWhile(whileStmt);
    } else if (auto forStmt = dynamic_cast<ForStmtAST*>(stmt)) {
        return codegenFor(forStmt);
    } else if (auto forInStmt = dynamic_cast<ForInStmtAST*>(stmt)) {
        return codegenForIn(forInStmt);
    } else if (auto blockStmt = dynamic_cast<BlockStmtAST*>(stmt)) {
        return codegenBlock(blockStmt);
    }
    
    throw std::runtime_error("Unknown statement type");
}

llvm::Value* CodeGenerator::codegenNumber(NumberExprAST* expr) {
    // Check if the number can fit in different integer types
    double value = expr->value;
    
    // Check if it's a whole number
    if (value == std::floor(value)) {
        // It's an integer - choose the appropriate size
        long long intVal = static_cast<long long>(value);
        
        // Check ranges for different integer types
        if (intVal >= INT32_MIN && intVal <= INT32_MAX) {
            // Fits in i32
            return llvm::ConstantInt::get(*context, llvm::APInt(32, intVal));
        } else if (intVal >= LLONG_MIN && intVal <= LLONG_MAX) {
            // Needs i64
            return llvm::ConstantInt::get(*context, llvm::APInt(64, intVal));
        } else {
            // Too big for i64, use double
            return llvm::ConstantFP::get(*context, llvm::APFloat(value));
        }
    } else {
        // It's a floating-point number
        return llvm::ConstantFP::get(*context, llvm::APFloat(value));
    }
}

llvm::Value* CodeGenerator::codegenScientific(ScientificExprAST* expr) {
    // Scientific notation is always treated as floating-point
    // Convert the scientific notation string to double value
    double value = expr->value;  // ScientificExprAST should store the parsed double value
    return llvm::ConstantFP::get(*context, llvm::APFloat(value));
}

llvm::Value* CodeGenerator::codegenString(StringExprAST* expr) {
    return builder->CreateGlobalStringPtr(expr->value);
}

llvm::Value* CodeGenerator::codegenVariable(VariableExprAST* expr) {
    if (expr->name == "EXIT_SUCCESS") {
        return llvm::ConstantInt::get(*context, llvm::APInt(32, 0));
    }
    
    if (expr->name == "self") {
        // Handle 'self' reference in class methods
        llvm::Value* selfVar = namedValues["self"];
        if (!selfVar) {
            throw std::runtime_error("'self' is only available in class method context");
        }
        return builder->CreateLoad(selfVar->getType()->getPointerElementType(), selfVar, "selfval");
    }
    
    llvm::Value* var = namedValues[expr->name];
    if (!var) {
        throw std::runtime_error("Unknown variable: " + expr->name);
    }
    
    return builder->CreateLoad(var->getType()->getPointerElementType(), var, expr->name);
}

llvm::Value* CodeGenerator::codegenBinary(BinaryExprAST* expr) {
    // Handle assignment operator specially
    if (expr->op == "=") {
        // Assignment: left side must be a variable
        if (auto varExpr = dynamic_cast<VariableExprAST*>(expr->left.get())) {
            llvm::Value* rhs = codegen(expr->right.get());
            if (!rhs) return nullptr;
            
            llvm::Value* var = namedValues[varExpr->name];
            if (!var) {
                throw std::runtime_error("Unknown variable: " + varExpr->name);
            }
            
            builder->CreateStore(rhs, var);
            return rhs;  // Return the assigned value
        } else {
            throw std::runtime_error("Invalid assignment target");
        }
    }
    
    // For other operators, evaluate both sides normally
    llvm::Value* lhs = codegen(expr->left.get());
    llvm::Value* rhs = codegen(expr->right.get());
    
    if (!lhs || !rhs) {
        return nullptr;
    }
    
    // Handle type conversion for arithmetic operations
    if (lhs->getType() != rhs->getType()) {
        if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy()) {
            // Both are integers but different sizes
            if (lhs->getType()->getIntegerBitWidth() < rhs->getType()->getIntegerBitWidth()) {
                // Extend lhs to match rhs
                lhs = builder->CreateSExt(lhs, rhs->getType(), "sext");
            } else {
                // Extend rhs to match lhs
                rhs = builder->CreateSExt(rhs, lhs->getType(), "sext");
            }
        } else if (lhs->getType()->isFloatingPointTy() && rhs->getType()->isIntegerTy()) {
            // Convert integer to float
            rhs = builder->CreateSIToFP(rhs, lhs->getType(), "i2f");
        } else if (lhs->getType()->isIntegerTy() && rhs->getType()->isFloatingPointTy()) {
            // Convert integer to float
            lhs = builder->CreateSIToFP(lhs, rhs->getType(), "i2f");
        }
    }
    
    if (expr->op == "+") {
        if (lhs->getType()->isIntegerTy()) {
            return builder->CreateAdd(lhs, rhs, "addtmp");
        } else {
            return builder->CreateFAdd(lhs, rhs, "addtmp");
        }
    } else if (expr->op == "-") {
        if (lhs->getType()->isIntegerTy()) {
            return builder->CreateSub(lhs, rhs, "subtmp");
        } else {
            return builder->CreateFSub(lhs, rhs, "subtmp");
        }
    } else if (expr->op == "*") {
        if (lhs->getType()->isIntegerTy()) {
            return builder->CreateMul(lhs, rhs, "multmp");
        } else {
            return builder->CreateFMul(lhs, rhs, "multmp");
        }
    } else if (expr->op == "/") {
        if (lhs->getType()->isIntegerTy()) {
            return builder->CreateSDiv(lhs, rhs, "divtmp");
        } else {
            return builder->CreateFDiv(lhs, rhs, "divtmp");
        }
    } else if (expr->op == "<") {
        if (lhs->getType()->isIntegerTy()) {
            return builder->CreateICmpSLT(lhs, rhs, "cmptmp");
        } else {
            return builder->CreateFCmpOLT(lhs, rhs, "cmptmp");
        }
    } else if (expr->op == ">") {
        if (lhs->getType()->isIntegerTy()) {
            return builder->CreateICmpSGT(lhs, rhs, "cmptmp");
        } else {
            return builder->CreateFCmpOGT(lhs, rhs, "cmptmp");
        }
    } else if (expr->op == "==") {
        if (lhs->getType()->isIntegerTy()) {
            return builder->CreateICmpEQ(lhs, rhs, "cmptmp");
        } else {
            return builder->CreateFCmpOEQ(lhs, rhs, "cmptmp");
        }
    } else if (expr->op == "!=") {
        if (lhs->getType()->isIntegerTy()) {
            return builder->CreateICmpNE(lhs, rhs, "cmptmp");
        } else {
            return builder->CreateFCmpONE(lhs, rhs, "cmptmp");
        }
    }
    
    throw std::runtime_error("Unknown binary operator: " + expr->op);
}

llvm::Value* CodeGenerator::codegenCall(CallExprAST* expr) {
    // Check if it's a builtin function first
    auto builtinIt = builtinFunctions.find(expr->callee);
    if (builtinIt != builtinFunctions.end()) {
        std::vector<llvm::Value*> args;
        for (auto& arg : expr->args) {
            args.push_back(codegen(arg.get()));
        }
        return callBuiltinFunction(expr->callee, args);
    }
    
    if (expr->callee == "println") {
        // Handle println specially
        if (expr->args.empty()) {
            // Empty println - just print newline
            llvm::Value* formatStr = builder->CreateGlobalStringPtr("\n");
            llvm::Function* printf = module->getFunction("printf");
            return builder->CreateCall(printf, {formatStr});
        }
        
        std::vector<llvm::Value*> printfArgs;
        
        // For each argument, create appropriate printf call
        for (size_t i = 0; i < expr->args.size(); ++i) {
            llvm::Value* argVal = codegen(expr->args[i].get());
            
            if (argVal->getType()->isIntegerTy()) {
                std::string formatStr;
                if (argVal->getType()->isIntegerTy(64)) {
                    formatStr = "%lld";  // long long for i64
                } else if (argVal->getType()->isIntegerTy(32)) {
                    formatStr = "%d";    // int for i32
                } else if (argVal->getType()->isIntegerTy(16)) {
                    formatStr = "%hd";   // short for i16
                } else if (argVal->getType()->isIntegerTy(8)) {
                    formatStr = "%hhd";  // char for i8
                } else if (argVal->getType()->isIntegerTy(1)) {
                    formatStr = "%d";    // bool as int
                } else {
                    formatStr = "%d";    // default to int
                }
                
                llvm::Value* fmt = builder->CreateGlobalStringPtr(formatStr);
                printfArgs = {fmt, argVal};
            } else if (argVal->getType()->isDoubleTy() || argVal->getType()->isFloatTy()) {
                llvm::Value* formatStr = builder->CreateGlobalStringPtr("%f");
                printfArgs = {formatStr, argVal};
            } else if (argVal->getType()->isPointerTy()) {
                // For string literals, print directly
                llvm::Value* formatStr = builder->CreateGlobalStringPtr("%s");
                printfArgs = {formatStr, argVal};
            } else {
                // Fallback for unknown types
                llvm::Value* formatStr = builder->CreateGlobalStringPtr("(unknown)");
                printfArgs = {formatStr};
            }
            
            // Print this argument
            llvm::Function* printf = module->getFunction("printf");
            builder->CreateCall(printf, printfArgs);
            
            // Add space between arguments (except for last)
            if (i < expr->args.size() - 1) {
                llvm::Value* spaceStr = builder->CreateGlobalStringPtr(" ");
                builder->CreateCall(printf, {spaceStr});
            }
        }
        
        // Add newline at the end
        llvm::Value* newlineStr = builder->CreateGlobalStringPtr("\n");
        llvm::Function* printf = module->getFunction("printf");
        return builder->CreateCall(printf, {newlineStr});
    }
    
    // Regular function call
    llvm::Function* callee = functions[expr->callee];
    if (!callee) {
        throw std::runtime_error("Unknown function: " + expr->callee);
    }
    
    std::vector<llvm::Value*> args;
    for (auto& arg : expr->args) {
        args.push_back(codegen(arg.get()));
    }
    
    return builder->CreateCall(callee, args);
}

llvm::Value* CodeGenerator::codegenMemberAccess(MemberAccessExprAST* expr) {
    llvm::Value* object = codegen(expr->object.get());
    return object;
}

llvm::Value* CodeGenerator::codegenMethodCall(MethodCallExprAST* expr) {
    // Get the object
    llvm::Value* object = codegen(expr->object.get());
    
    // Determine object type for builtin method lookup
    std::string objectType = getTypeName(object);
    
    // Check if it's a builtin method
    auto typeIt = builtinMethods.find(objectType);
    if (typeIt != builtinMethods.end()) {
        auto methodIt = typeIt->second.find(expr->method);
        if (methodIt != typeIt->second.end()) {
            std::vector<llvm::Value*> args;
            for (auto& arg : expr->args) {
                args.push_back(codegen(arg.get()));
            }
            return callBuiltinMethod(objectType, expr->method, object, args);
        }
    }
    
    // Look for the method as a regular function
    llvm::Function* method = functions[expr->method];
    if (!method) {
        throw std::runtime_error("Unknown method: " + expr->method);
    }
    
    // Generate arguments (don't pass self for now since methods don't expect it)
    std::vector<llvm::Value*> args;
    
    // Add method arguments (not passing object/self for now)
    for (auto& arg : expr->args) {
        args.push_back(codegen(arg.get()));
    }
    
    return builder->CreateCall(method, args);
}

llvm::Value* CodeGenerator::codegenNew(NewExprAST* expr) {
    auto it = structs.find(expr->className);
    if (it == structs.end()) {
        throw std::runtime_error("Unknown class: " + expr->className);
    }
    
    llvm::StructType* structType = it->second;
    llvm::Value* size = llvm::ConstantInt::get(*context, 
        llvm::APInt(64, module->getDataLayout().getTypeAllocSize(structType)));
    
    llvm::Value* ptr = builder->CreateCall(mallocFunc, {size});
    return builder->CreateBitCast(ptr, llvm::PointerType::get(structType, 0));
}

llvm::Value* CodeGenerator::codegenVarDecl(VarDeclStmtAST* stmt) {
    llvm::Type* type = getFlastType(stmt->type->toString());
    llvm::AllocaInst* alloca = builder->CreateAlloca(type, nullptr, stmt->name);
    
    if (stmt->initializer) {
        llvm::Value* initVal = codegen(stmt->initializer.get());
        
        // Handle type conversion if needed
        if (initVal->getType() != type) {
            if (type->isIntegerTy() && initVal->getType()->isIntegerTy()) {
                // Integer to integer conversion
                if (type->getIntegerBitWidth() != initVal->getType()->getIntegerBitWidth()) {
                    if (type->getIntegerBitWidth() > initVal->getType()->getIntegerBitWidth()) {
                        // Sign extend for larger type
                        initVal = builder->CreateSExt(initVal, type, "sext");
                    } else {
                        // Truncate for smaller type
                        initVal = builder->CreateTrunc(initVal, type, "trunc");
                    }
                }
            } else if (type->isIntegerTy() && initVal->getType()->isFloatingPointTy()) {
                // Float to integer conversion
                initVal = builder->CreateFPToSI(initVal, type, "f2i");
            } else if (type->isFloatingPointTy() && initVal->getType()->isIntegerTy()) {
                // Integer to float conversion
                initVal = builder->CreateSIToFP(initVal, type, "i2f");
            } else if (type->isFloatingPointTy() && initVal->getType()->isFloatingPointTy()) {
                // Float to float conversion
                if (type->isFloatTy() && initVal->getType()->isDoubleTy()) {
                    initVal = builder->CreateFPTrunc(initVal, type, "fptrunc");
                } else if (type->isDoubleTy() && initVal->getType()->isFloatTy()) {
                    initVal = builder->CreateFPExt(initVal, type, "fpext");
                }
            }
        }
        
        builder->CreateStore(initVal, alloca);
    }
    
    namedValues[stmt->name] = alloca;
    return alloca;
}

llvm::Value* CodeGenerator::codegenAssign(AssignStmtAST* stmt) {
    llvm::Value* rhs = codegen(stmt->value.get());
    
    if (auto varExpr = dynamic_cast<VariableExprAST*>(stmt->target.get())) {
        llvm::Value* var = namedValues[varExpr->name];
        if (!var) {
            throw std::runtime_error("Unknown variable: " + varExpr->name);
        }
        return builder->CreateStore(rhs, var);
    } else if (auto memberExpr = dynamic_cast<MemberAccessExprAST*>(stmt->target.get())) {
        // Handle member assignment like self.member = value
        // For now, treat it as a simple variable assignment
        // TODO: Implement proper member access and assignment
        std::string memberName = memberExpr->member;
        llvm::Value* var = namedValues[memberName];
        if (!var) {
            // Create a placeholder for the member
            llvm::Type* memberType = rhs->getType();
            llvm::AllocaInst* alloca = builder->CreateAlloca(memberType, nullptr, memberName);
            namedValues[memberName] = alloca;
            var = alloca;
        }
        return builder->CreateStore(rhs, var);
    }
    
    throw std::runtime_error("Invalid assignment target");
}

llvm::Value* CodeGenerator::codegenExprStmt(ExprStmtAST* stmt) {
    return codegen(stmt->expression.get());
}

llvm::Value* CodeGenerator::codegenReturn(ReturnStmtAST* stmt) {
    if (stmt->value) {
        llvm::Value* retVal = codegen(stmt->value.get());
        return builder->CreateRet(retVal);
    } else {
        return builder->CreateRetVoid();
    }
}

void CodeGenerator::printIR() {
    module->print(llvm::outs(), nullptr);
}

void CodeGenerator::writeObjectFile(const std::string& filename) {
    // Initialize target
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
    
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(targetTriple);
    
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    
    if (!target) {
        throw std::runtime_error("Target lookup failed: " + error);
    }
    
    auto cpu = "generic";
    auto features = "";
    
    llvm::TargetOptions opt;
    auto relocModel = llvm::Optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(
        targetTriple, cpu, features, opt, relocModel);
    
    module->setDataLayout(targetMachine->createDataLayout());
    
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    
    if (ec) {
        throw std::runtime_error("Could not open file: " + ec.message());
    }
    
    llvm::legacy::PassManager pass;
    auto fileType = llvm::CGFT_ObjectFile;
    
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        throw std::runtime_error("TargetMachine can't emit a file of this type");
    }
    
    pass.run(*module);
    dest.flush();
}

std::string CodeGenerator::writeExecutable(const std::string& sourceFile, bool debugMode, bool optimized) {
    auto sourcePath = std::filesystem::path(sourceFile);
    auto baseName = sourcePath.stem().string();
    
    // Detect OS and set appropriate executable extension
    std::string exeExtension;
    std::string osName;
    
#ifdef _WIN32
    exeExtension = ".exe";
    osName = "Windows";
#elif __APPLE__
    exeExtension = "";  // macOS doesn't use extensions for executables
    osName = "macOS";
#elif __linux__
    exeExtension = "";  // Linux doesn't use extensions for executables
    osName = "Linux";
#else
    exeExtension = ".out";  // Generic Unix
    osName = "Unix";
#endif
    
    // Output paths
    auto objFile = cacheDir / (baseName + ".o");
    auto exeFile = binDir / (baseName + exeExtension);
    
    std::cout << "\n=== FLAST PROFESSIONAL COMPILER ===" << std::endl;
    std::cout << "Platform: " << osName << std::endl;
    std::cout << "Compiling: " << sourceFile << std::endl;
    std::cout << "Target: " << exeFile << std::endl;
    
    // Generate main object file
    writeObjectFile(objFile.string());
    std::cout << "✓ Main object file: " << objFile << std::endl;
    
    // Collect all module object files
    std::vector<std::string> moduleObjFiles = collectModuleObjectFiles();
    if (!moduleObjFiles.empty()) {
        std::cout << "✓ Module object files: " << moduleObjFiles.size() << " files" << std::endl;
        for (const auto& moduleObj : moduleObjFiles) {
            std::cout << "  - " << std::filesystem::path(moduleObj).filename() << std::endl;
        }
    }
    
    // Build link command with all object files
    std::string linkCmd = "gcc -no-pie";
    
    if (debugMode) {
        linkCmd += " -g";  // Add debug symbols
        std::cout << "✓ Debug mode enabled" << std::endl;
    }
    
    if (optimized) {
        linkCmd += " -O2";  // Optimization
        std::cout << "✓ Optimization enabled" << std::endl;
    }
    
    // Add main object file
    linkCmd += " \"" + objFile.string() + "\"";
    
    // Add all module object files
    for (const auto& moduleObj : moduleObjFiles) {
        linkCmd += " \"" + moduleObj + "\"";
    }
    
    // Add output file
    linkCmd += " -o \"" + exeFile.string() + "\"";
    
    std::cout << "Linking with " << (moduleObjFiles.size() + 1) << " object files..." << std::endl;
    int result = std::system(linkCmd.c_str());
    
    if (result != 0) {
        throw std::runtime_error("Linking failed");
    }
    
    std::cout << "✓ Executable: " << exeFile << std::endl;
    
    // Generate debug info if enabled
    if (debugMode) {
        std::cout << "✓ Debug symbols: Enabled (basic level)" << std::endl;
    }
    
    std::cout << "✓ Compilation successful!" << std::endl;
    
    return exeFile.string();
}

void CodeGenerator::cleanupCache() {
    try {
        if (std::filesystem::exists(cacheDir)) {
            int filesRemoved = 0;
            int moduleCachesRemoved = 0;
            
            for (const auto& entry : std::filesystem::directory_iterator(cacheDir)) {
                if (entry.is_regular_file()) {
                    // Remove main cache files
                    std::filesystem::remove(entry);
                    filesRemoved++;
                } else if (entry.is_directory() && entry.path().filename() == "modules") {
                    // Remove module cache directories
                    for (const auto& moduleEntry : std::filesystem::recursive_directory_iterator(entry)) {
                        if (moduleEntry.is_regular_file()) {
                            moduleCachesRemoved++;
                        }
                    }
                    std::filesystem::remove_all(entry);
                }
            }
            
            std::cout << "🧹 Cache cleaned: " << filesRemoved << " main files, " 
                      << moduleCachesRemoved << " module caches (keeping binaries)" << std::endl;
        }
        
        // Clear memory cache as well
        moduleCache.clear();
        moduleCachePaths.clear();
        moduleObjectFiles.clear();
        
    } catch (const std::exception& e) {
        std::cerr << "⚠️  Warning: Could not clean cache: " << e.what() << std::endl;
    }
}

// ==================== MODULE LOADING SYSTEM ====================

void CodeGenerator::codegen(ImportDeclAST* importDecl) {
    // Resolve module path
    std::string modulePath = resolveModulePath(importDecl->moduleName, currentSourceDir.string());
    
    // Load module
    std::shared_ptr<ProgramAST> moduleAst = loadModule(modulePath);
    if (!moduleAst) {
        throw std::runtime_error("Failed to load module: " + importDecl->moduleName);
    }
    
    // Process imported functions
    processImportedFunctions(moduleAst, importDecl->specificImports, importDecl->isWildcard);
}

std::string CodeGenerator::resolveModulePath(const std::string& importPath, const std::string& currentDir) {
    std::filesystem::path resolvedPath;
    
    // Check for relative paths (C++17 compatible way)
    if (importPath.substr(0, 2) == "./" || importPath.substr(0, 3) == "../") {
        // Relative path
        resolvedPath = std::filesystem::path(currentDir) / importPath;
    } else {
        // Try to resolve as package/module name
        // 1. First check in root project directory
        resolvedPath = projectRoot / importPath;
        
        // 2. If not found, check in packages directory
        if (!std::filesystem::exists(resolvedPath) || !resolvedPath.has_extension()) {
            std::filesystem::path packagePath = projectRoot / "packages" / importPath;
            if (std::filesystem::exists(packagePath) || std::filesystem::exists(packagePath.string() + ".fls")) {
                resolvedPath = packagePath;
            }
        }
    }
    
    // Add .fls extension if not present
    if (!resolvedPath.has_extension()) {
        resolvedPath += ".fls";
    }
    
    // Check if file exists
    if (!std::filesystem::exists(resolvedPath)) {
        // Add to missing modules list for reporting
        missingModules.push_back(importPath);
        return ""; // Return empty string to indicate not found
    }
    
    // Normalize path
    return std::filesystem::absolute(resolvedPath).string();
}

std::shared_ptr<ProgramAST> CodeGenerator::loadModule(const std::string& modulePath) {
    // Handle empty path (module not found)
    if (modulePath.empty()) {
        return nullptr;
    }
    
    // Check cache first (both memory and disk)
    std::shared_ptr<ProgramAST> cachedModule = loadModuleFromCache(modulePath);
    if (cachedModule && isModuleCacheValid(modulePath)) {
        // Generate object file for cached module if needed
        generateModuleObjectFile(modulePath, cachedModule);
        return cachedModule;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(modulePath)) {
        std::cerr << "Module file not found: " << modulePath << std::endl;
        return nullptr;
    }
    
    std::cout << "🔄 Loading module: " << std::filesystem::path(modulePath).filename() << std::endl;
    
    try {
        // Read module file
        std::ifstream file(modulePath);
        if (!file) {
            throw std::runtime_error("Could not open module file: " + modulePath);
        }
        
        std::string content;
        std::string line;
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        
        // Parse module (includes are at top of file)
        Lexer lexer(content);
        auto tokens = lexer.tokenize();
        
        Parser parser(std::move(tokens), modulePath);
        auto moduleAst = parser.parseProgram();
        
        // Cache the module with new caching system
        saveModuleCache(modulePath, moduleAst);
        
        // Generate object file for the module
        generateModuleObjectFile(modulePath, moduleAst);
        
        return moduleAst;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading module " << modulePath << ": " << e.what() << std::endl;
        return nullptr;
    }
}

void CodeGenerator::processImportedFunctions(std::shared_ptr<ProgramAST> moduleAst, 
                                           const std::vector<std::string>& specificImports, 
                                           bool isWildcard) {
    
    std::cout << "🔍 Processing imports - specificImports: [";
    for (const auto& imp : specificImports) {
        std::cout << imp << " ";
    }
    std::cout << "], isWildcard: " << (isWildcard ? "true" : "false") << std::endl;
    
    for (auto& decl : moduleAst->declarations) {
        if (auto funcDecl = dynamic_cast<FunctionDeclAST*>(decl.get())) {
            // Check if function should be imported
            bool shouldImport = false;
            
            if (isWildcard) {
                // Import all public functions
                shouldImport = funcDecl->isPublic;
            } else if (specificImports.empty()) {
                // Default import (import first public function)
                shouldImport = funcDecl->isPublic;
            } else {
                // Named imports
                for (const auto& importName : specificImports) {
                    if (funcDecl->name == importName && funcDecl->isPublic) {
                        shouldImport = true;
                        break;
                    }
                }
            }
            
            if (shouldImport) {
                // Generate code for the imported function
                codegen(funcDecl);
                std::cout << "✓ Imported function: " << funcDecl->name << std::endl;
            }
        }
    }
}

llvm::Value* CodeGenerator::codegenWhile(WhileStmtAST* whileStmt) {
    llvm::Function* currentFunction = builder->GetInsertBlock()->getParent();
    
    // Create basic blocks
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "while.cond", currentFunction);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context, "while.body", currentFunction);
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context, "while.after", currentFunction);
    
    // Jump to condition
    builder->CreateBr(condBB);
    
    // Generate condition
    builder->SetInsertPoint(condBB);
    llvm::Value* condValue = codegen(whileStmt->condition.get());
    if (!condValue) return nullptr;
    
    // Convert condition to boolean - check if it's already i1
    if (condValue->getType()->isIntegerTy(1)) {
        // Already boolean, use as is
    } else if (condValue->getType()->isIntegerTy()) {
        // Integer type, compare with zero
        llvm::Value* zero = llvm::ConstantInt::get(condValue->getType(), 0);
        condValue = builder->CreateICmpNE(condValue, zero, "whilecond");
    } else {
        throw std::runtime_error("While condition must be boolean or integer");
    }
    
    builder->CreateCondBr(condValue, bodyBB, afterBB);
    
    // Generate body
    builder->SetInsertPoint(bodyBB);
    codegen(whileStmt->body.get());
    
    // Jump back to condition (unless body has explicit return)
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(condBB);
    }
    
    // Continue after loop
    builder->SetInsertPoint(afterBB);
    
    return nullptr;
}

llvm::Value* CodeGenerator::codegenFor(ForStmtAST* forStmt) {
    llvm::Function* currentFunction = builder->GetInsertBlock()->getParent();
    
    // Generate initialization
    if (forStmt->init) {
        codegen(forStmt->init.get());
    }
    
    // Create basic blocks
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "for.cond", currentFunction);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context, "for.body", currentFunction);
    llvm::BasicBlock* updateBB = llvm::BasicBlock::Create(*context, "for.update", currentFunction);
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context, "for.after", currentFunction);
    
    // Jump to condition
    builder->CreateBr(condBB);
    
    // Generate condition
    builder->SetInsertPoint(condBB);
    if (forStmt->condition) {
        llvm::Value* condValue = codegen(forStmt->condition.get());
        if (!condValue) return nullptr;
        
        // Convert condition to boolean - check if it's already i1
        if (condValue->getType()->isIntegerTy(1)) {
            // Already boolean, use as is
        } else if (condValue->getType()->isIntegerTy()) {
            // Integer type, compare with zero
            llvm::Value* zero = llvm::ConstantInt::get(condValue->getType(), 0);
            condValue = builder->CreateICmpNE(condValue, zero, "forcond");
        } else {
            throw std::runtime_error("For condition must be boolean or integer");
        }
        
        builder->CreateCondBr(condValue, bodyBB, afterBB);
    } else {
        // No condition = infinite loop
        builder->CreateBr(bodyBB);
    }
    
    // Generate body
    builder->SetInsertPoint(bodyBB);
    codegen(forStmt->body.get());
    
    // Jump to update (unless body has explicit return)
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(updateBB);
    }
    
    // Generate update
    builder->SetInsertPoint(updateBB);
    if (forStmt->update) {
        codegen(forStmt->update.get());
    }
    
    // Jump back to condition
    builder->CreateBr(condBB);
    
    // Continue after loop
    builder->SetInsertPoint(afterBB);
    
    return nullptr;
}

llvm::Value* CodeGenerator::codegenForIn(ForInStmtAST* forInStmt) {
    // For now, implement simple integer iteration: for i in n { ... } iterates i from 0 to n-1
    llvm::Function* currentFunction = builder->GetInsertBlock()->getParent();
    
    // Generate the iterable value (should be an integer for simple case)
    llvm::Value* iterableVal = codegen(forInStmt->iterable.get());
    if (!iterableVal) return nullptr;
    
    // Determine the type for the loop variable - use i32 by default
    // TODO: In a proper implementation, we should parse the actual declared type
    llvm::Type* loopVarType = llvm::Type::getInt32Ty(*context);
    
    // Ensure iterableVal is compatible with loop variable type
    if (iterableVal->getType() != loopVarType) {
        if (iterableVal->getType()->isIntegerTy() && loopVarType->isIntegerTy()) {
            // Convert iterable to loop variable type
            if (iterableVal->getType()->getIntegerBitWidth() > loopVarType->getIntegerBitWidth()) {
                // Truncate if iterable is larger
                iterableVal = builder->CreateTrunc(iterableVal, loopVarType, "trunc");
            } else {
                // Extend if iterable is smaller
                iterableVal = builder->CreateSExt(iterableVal, loopVarType, "sext");
            }
        } else if (iterableVal->getType()->isFloatingPointTy()) {
            // Convert float to integer
            iterableVal = builder->CreateFPToSI(iterableVal, loopVarType, "f2i");
        }
    }
    
    // Create loop variable
    llvm::AllocaInst* loopVar = builder->CreateAlloca(loopVarType, nullptr, forInStmt->variable);
    namedValues[forInStmt->variable] = loopVar;
    
    // Initialize loop variable to 0
    llvm::Value* zero = llvm::ConstantInt::get(loopVarType, 0);
    builder->CreateStore(zero, loopVar);
    
    // Create basic blocks
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "forin.cond", currentFunction);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context, "forin.body", currentFunction);
    llvm::BasicBlock* updateBB = llvm::BasicBlock::Create(*context, "forin.update", currentFunction);
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context, "forin.after", currentFunction);
    
    // Jump to condition
    builder->CreateBr(condBB);
    
    // Generate condition: loopVar < iterableVal
    builder->SetInsertPoint(condBB);
    llvm::Value* currentVal = builder->CreateLoad(loopVarType, loopVar, "current");
    llvm::Value* condValue = builder->CreateICmpSLT(currentVal, iterableVal, "forincond");
    builder->CreateCondBr(condValue, bodyBB, afterBB);
    
    // Generate body
    builder->SetInsertPoint(bodyBB);
    codegen(forInStmt->body.get());
    
    // Jump to update (unless body has explicit return)
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(updateBB);
    }
    
    // Generate update: increment loop variable
    builder->SetInsertPoint(updateBB);
    llvm::Value* currentVal2 = builder->CreateLoad(loopVarType, loopVar, "current");
    llvm::Value* one = llvm::ConstantInt::get(loopVarType, 1);
    llvm::Value* nextVal = builder->CreateAdd(currentVal2, one, "next");
    builder->CreateStore(nextVal, loopVar);
    
    // Jump back to condition
    builder->CreateBr(condBB);
    
    // Continue after loop
    builder->SetInsertPoint(afterBB);
    
    return nullptr;
}

llvm::Value* CodeGenerator::codegenBlock(BlockStmtAST* blockStmt) {
    llvm::Value* lastValue = nullptr;
    
    for (auto& stmt : blockStmt->statements) {
        lastValue = codegen(stmt.get());
    }
    
    return lastValue;
}

// Builtin system implementations
void CodeGenerator::registerBuiltinFunctions() {
    // Register print function
    builtinFunctions["print"] = [this](const std::vector<llvm::Value*>& args) -> llvm::Value* {
        if (args.empty()) return nullptr;
        
        // Get printf function
        llvm::Function* printfFunc = module->getFunction("printf");
        if (!printfFunc) {
            llvm::FunctionType* printfType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*context),
                {llvm::Type::getInt8PtrTy(*context)},
                true
            );
            printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module.get());
        }
        
        // Create format string based on argument type
        std::string format = "%s";
        if (args[0]->getType()->isIntegerTy()) {
            format = "%d";
        } else if (args[0]->getType()->isDoubleTy()) {
            format = "%f";
        }
        
        std::vector<llvm::Value*> printfArgs;
        printfArgs.push_back(builder->CreateGlobalStringPtr(format));
        printfArgs.insert(printfArgs.end(), args.begin(), args.end());
        
        return builder->CreateCall(printfFunc, printfArgs);
    };
    
    // Register println function
    builtinFunctions["println"] = [this](const std::vector<llvm::Value*>& args) -> llvm::Value* {
        if (args.empty()) return nullptr;
        
        // Get printf function
        llvm::Function* printfFunc = module->getFunction("printf");
        if (!printfFunc) {
            llvm::FunctionType* printfType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*context),
                {llvm::Type::getInt8PtrTy(*context)},
                true
            );
            printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module.get());
        }
        
        // Create format string with newline
        std::string format = "%s\n";
        if (args[0]->getType()->isIntegerTy()) {
            format = "%d\n";
        } else if (args[0]->getType()->isDoubleTy()) {
            format = "%f\n";
        }
        
        std::vector<llvm::Value*> printfArgs;
        printfArgs.push_back(builder->CreateGlobalStringPtr(format));
        printfArgs.insert(printfArgs.end(), args.begin(), args.end());
        
        return builder->CreateCall(printfFunc, printfArgs);
    };
}

void CodeGenerator::registerBuiltinMethods() {
    // Register string methods
    builtinMethods["str"]["len"] = [this](llvm::Value* self, const std::vector<llvm::Value*>& args) -> llvm::Value* {
        // Get strlen function
        llvm::Function* strlenFunc = module->getFunction("strlen");
        if (!strlenFunc) {
            llvm::FunctionType* strlenType = llvm::FunctionType::get(
                llvm::Type::getInt64Ty(*context),
                {llvm::Type::getInt8PtrTy(*context)},
                false
            );
            strlenFunc = llvm::Function::Create(strlenType, llvm::Function::ExternalLinkage, "strlen", module.get());
        }
        
        return builder->CreateCall(strlenFunc, {self});
    };
    
    builtinMethods["str"]["contains"] = [this](llvm::Value* self, const std::vector<llvm::Value*>& args) -> llvm::Value* {
        if (args.empty()) return llvm::ConstantInt::getFalse(*context);
        
        // Get strstr function
        llvm::Function* strstrFunc = module->getFunction("strstr");
        if (!strstrFunc) {
            llvm::FunctionType* strstrType = llvm::FunctionType::get(
                llvm::Type::getInt8PtrTy(*context),
                {llvm::Type::getInt8PtrTy(*context), llvm::Type::getInt8PtrTy(*context)},
                false
            );
            strstrFunc = llvm::Function::Create(strstrType, llvm::Function::ExternalLinkage, "strstr", module.get());
        }
        
        llvm::Value* result = builder->CreateCall(strstrFunc, {self, args[0]});
        return builder->CreateIsNotNull(result);
    };
    
    // Register type conversion methods
    builtinMethods["str"]["toi32"] = [this](llvm::Value* self, const std::vector<llvm::Value*>& args) -> llvm::Value* {
        // Get atoi function
        llvm::Function* atoiFunc = module->getFunction("atoi");
        if (!atoiFunc) {
            llvm::FunctionType* atoiType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*context),
                {llvm::Type::getInt8PtrTy(*context)},
                false
            );
            atoiFunc = llvm::Function::Create(atoiType, llvm::Function::ExternalLinkage, "atoi", module.get());
        }
        
        return builder->CreateCall(atoiFunc, {self});
    };
    
    builtinMethods["str"]["toi64"] = [this](llvm::Value* self, const std::vector<llvm::Value*>& args) -> llvm::Value* {
        // Get atoll function
        llvm::Function* atollFunc = module->getFunction("atoll");
        if (!atollFunc) {
            llvm::FunctionType* atollType = llvm::FunctionType::get(
                llvm::Type::getInt64Ty(*context),
                {llvm::Type::getInt8PtrTy(*context)},
                false
            );
            atollFunc = llvm::Function::Create(atollType, llvm::Function::ExternalLinkage, "atoll", module.get());
        }
        
        return builder->CreateCall(atollFunc, {self});
    };
    
    builtinMethods["str"]["tof32"] = [this](llvm::Value* self, const std::vector<llvm::Value*>& args) -> llvm::Value* {
        // Get atof function and cast to float
        llvm::Function* atofFunc = module->getFunction("atof");
        if (!atofFunc) {
            llvm::FunctionType* atofType = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(*context),
                {llvm::Type::getInt8PtrTy(*context)},
                false
            );
            atofFunc = llvm::Function::Create(atofType, llvm::Function::ExternalLinkage, "atof", module.get());
        }
        
        llvm::Value* doubleVal = builder->CreateCall(atofFunc, {self});
        return builder->CreateFPTrunc(doubleVal, llvm::Type::getFloatTy(*context));
    };
    
    builtinMethods["str"]["tof64"] = [this](llvm::Value* self, const std::vector<llvm::Value*>& args) -> llvm::Value* {
        // Get atof function
        llvm::Function* atofFunc = module->getFunction("atof");
        if (!atofFunc) {
            llvm::FunctionType* atofType = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(*context),
                {llvm::Type::getInt8PtrTy(*context)},
                false
            );
            atofFunc = llvm::Function::Create(atofType, llvm::Function::ExternalLinkage, "atof", module.get());
        }
        
        return builder->CreateCall(atofFunc, {self});
    };
    
    // Register number to string conversion
    builtinMethods["int"]["tostr"] = [this](llvm::Value* self, const std::vector<llvm::Value*>& args) -> llvm::Value* {
        // For now, return a simple string representation
        return builder->CreateGlobalStringPtr("number");
    };
}

llvm::Value* CodeGenerator::callBuiltinFunction(const std::string& name, const std::vector<llvm::Value*>& args) {
    auto it = builtinFunctions.find(name);
    if (it != builtinFunctions.end()) {
        return it->second(args);
    }
    return nullptr;
}

llvm::Value* CodeGenerator::callBuiltinMethod(const std::string& type, const std::string& method, 
                                             llvm::Value* object, const std::vector<llvm::Value*>& args) {
    auto typeIt = builtinMethods.find(type);
    if (typeIt != builtinMethods.end()) {
        auto methodIt = typeIt->second.find(method);
        if (methodIt != typeIt->second.end()) {
            return methodIt->second(object, args);
        }
    }
    return nullptr;
}

std::string CodeGenerator::getTypeName(llvm::Value* value) {
    if (value->getType()->isIntegerTy()) {
        return "int";
    } else if (value->getType()->isFloatingPointTy()) {
        return "float";
    } else if (value->getType()->isPointerTy()) {
        return "str";
    }
    return "unknown";
}

// ==================== MODULE-SPECIFIC CACHING SYSTEM ====================

std::string CodeGenerator::getModuleCacheFileName(const std::string& modulePath) {
    // Create a hash-based filename from module path to avoid conflicts
    std::hash<std::string> hasher;
    size_t hash = hasher(modulePath);
    
    std::filesystem::path moduleFile(modulePath);
    std::string baseName = moduleFile.stem().string();
    
    return baseName + "_" + std::to_string(hash) + ".cache";
}

std::filesystem::path CodeGenerator::getModuleCacheDir(const std::string& modulePath) {
    // Create module-specific cache directory
    std::filesystem::path moduleFile(modulePath);
    std::string moduleName = moduleFile.stem().string();
    
    // Get relative path from project root to create organized cache structure
    std::filesystem::path relativePath;
    try {
        relativePath = std::filesystem::relative(moduleFile.parent_path(), projectRoot);
    } catch (const std::exception&) {
        // If relative path fails, use absolute path hash
        std::hash<std::string> hasher;
        size_t pathHash = hasher(moduleFile.parent_path().string());
        relativePath = "external_" + std::to_string(pathHash);
    }
    
    return cacheDir / "modules" / relativePath;
}

void CodeGenerator::createModuleCacheStructure(const std::string& modulePath) {
    std::filesystem::path moduleCacheDir = getModuleCacheDir(modulePath);
    std::filesystem::create_directories(moduleCacheDir);
    
    // Track this module's cache path
    moduleCachePaths[modulePath] = moduleCacheDir;
}

bool CodeGenerator::isModuleCacheValid(const std::string& modulePath) {
    std::filesystem::path moduleCacheDir = getModuleCacheDir(modulePath);
    std::string cacheFileName = getModuleCacheFileName(modulePath);
    std::filesystem::path cacheFile = moduleCacheDir / cacheFileName;
    
    if (!std::filesystem::exists(cacheFile)) {
        return false;
    }
    
    // Check if source file is newer than cache
    try {
        auto sourceTime = std::filesystem::last_write_time(modulePath);
        auto cacheTime = std::filesystem::last_write_time(cacheFile);
        
        return cacheTime >= sourceTime;
    } catch (const std::exception&) {
        return false; // If we can't check timestamps, assume invalid
    }
}

void CodeGenerator::saveModuleCache(const std::string& modulePath, std::shared_ptr<ProgramAST> moduleAst) {
    // Store in memory cache
    moduleCache[modulePath] = moduleAst;
    
    // Create cache directory structure
    createModuleCacheStructure(modulePath);
    
    // Save cache file to disk
    std::filesystem::path moduleCacheDir = getModuleCacheDir(modulePath);
    std::string cacheFileName = getModuleCacheFileName(modulePath);
    std::filesystem::path cacheFilePath = moduleCacheDir / cacheFileName;
    
    try {
        std::ofstream cacheFile(cacheFilePath);
        if (cacheFile.is_open()) {
            // Write cache metadata
            cacheFile << "# Flast Module Cache\n";
            cacheFile << "# Module: " << std::filesystem::path(modulePath).filename() << "\n";
            cacheFile << "# Generated: " << std::time(nullptr) << "\n";
            cacheFile << "# AST Nodes: " << moduleAst->declarations.size() << "\n\n";
            
            // Write module content summary
            for (auto& decl : moduleAst->declarations) {
                if (auto funcDecl = dynamic_cast<FunctionDeclAST*>(decl.get())) {
                    cacheFile << "FUNCTION: " << funcDecl->name << " (public: " << (funcDecl->isPublic ? "yes" : "no") << ")\n";
                }
            }
            
            cacheFile.close();
            std::cout << "💾 Cached module: " << std::filesystem::path(modulePath).filename() << " -> " << cacheFileName << std::endl;
        } else {
            std::cerr << "Error: Could not open cache file for writing: " << cacheFilePath << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not write cache file: " << e.what() << std::endl;
    }
}

std::shared_ptr<ProgramAST> CodeGenerator::loadModuleFromCache(const std::string& modulePath) {
    // Check memory cache first
    auto it = moduleCache.find(modulePath);
    if (it != moduleCache.end()) {
        std::cout << "⚡ Using cached module: " << std::filesystem::path(modulePath).filename() << std::endl;
        return it->second;
    }
    
    // TODO: In a full implementation, this would load from disk cache
    // For now, return nullptr to indicate no cache available
    return nullptr;
}

// ==================== MODULE OBJECT FILE GENERATION ====================

std::string CodeGenerator::getModuleObjectFileName(const std::string& modulePath) {
    // Create object file name from module path
    std::hash<std::string> hasher;
    size_t hash = hasher(modulePath);
    
    std::filesystem::path moduleFile(modulePath);
    std::string baseName = moduleFile.stem().string();
    
    return baseName + "_" + std::to_string(hash) + ".o";
}

void CodeGenerator::generateModuleObjectFile(const std::string& modulePath, std::shared_ptr<ProgramAST> moduleAst) {
    // Get module cache directory and object file name
    std::filesystem::path moduleCacheDir = getModuleCacheDir(modulePath);
    std::string objFileName = getModuleObjectFileName(modulePath);
    std::filesystem::path objFilePath = moduleCacheDir / objFileName;
    
    // Check if object file already exists and is valid
    if (isModuleObjectValid(modulePath)) {
        std::cout << "⚡ Using cached object: " << objFileName << std::endl;
        moduleObjectFiles[modulePath] = objFilePath.string();
        return;
    }
    
    std::cout << "🔧 Generating module object: " << objFileName << std::endl;
    
    try {
        // Create directory if it doesn't exist
        std::filesystem::create_directories(moduleCacheDir);
        
        // Generate a stub C file that will be compiled to object
        std::string stubCFileName = objFileName.substr(0, objFileName.find_last_of('.')) + "_stub.c";
        std::filesystem::path stubCFilePath = moduleCacheDir / stubCFileName;
        
        // Create a simple C stub file for the module
        std::ofstream stubFile(stubCFilePath);
        if (!stubFile) {
            throw std::runtime_error("Could not create stub file for module");
        }
        
        // Write module stub functions
        stubFile << "/* Module stub for: " << std::filesystem::path(modulePath).filename() << " */\n";
        stubFile << "#include <stdio.h>\n\n";
        
        // Use a set to avoid duplicate stub names
        std::set<std::string> stubNames;
        
        // Generate stub functions for public functions in module
        for (auto& decl : moduleAst->declarations) {
            if (auto funcDecl = dynamic_cast<FunctionDeclAST*>(decl.get())) {
                if (funcDecl->isPublic) {
                    std::string stubName = funcDecl->name;
                    if (stubNames.count(stubName) == 0) {
                        stubFile << "/* Stub for function: " << funcDecl->name << " */\n";
                        stubFile << "int __module_" << stubName << "_stub() { return 0; }\n\n";
                        stubNames.insert(stubName);
                    }
                }
            }
        }
        
        stubFile.close();
        
        // Compile the stub file to object file
        std::string compileCmd = "gcc -c \"" + stubCFilePath.string() + "\" -o \"" + objFilePath.string() + "\"";
        
        int result = std::system(compileCmd.c_str());
        if (result != 0) {
            throw std::runtime_error("Failed to compile module stub");
        }
        
        // Remove the stub C file
        std::filesystem::remove(stubCFilePath);
        
        // Track the object file
        moduleObjectFiles[modulePath] = objFilePath.string();
        
        std::cout << "✓ Module object generated: " << objFileName << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error generating module object: " << e.what() << std::endl;
    }
}

bool CodeGenerator::isModuleObjectValid(const std::string& modulePath) {
    std::filesystem::path moduleCacheDir = getModuleCacheDir(modulePath);
    std::string objFileName = getModuleObjectFileName(modulePath);
    std::filesystem::path objFilePath = moduleCacheDir / objFileName;
    
    if (!std::filesystem::exists(objFilePath)) {
        return false;
    }
    
    // Check if source file is newer than object file
    try {
        auto sourceTime = std::filesystem::last_write_time(modulePath);
        auto objTime = std::filesystem::last_write_time(objFilePath);
        
        return objTime >= sourceTime;
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> CodeGenerator::collectModuleObjectFiles() {
    std::vector<std::string> objectFiles;
    
    for (const auto& pair : moduleObjectFiles) {
        objectFiles.push_back(pair.second);
    }
    
    return objectFiles;
}

void CodeGenerator::reportMissingModules() {
    if (!missingModules.empty()) {
        std::cout << "\n=== MISSING MODULES ===" << std::endl;
        std::cout << "The following modules could not be found:" << std::endl;
        std::cout << "Searched in: " << std::endl;
        std::cout << "  1. Root project directory" << std::endl;
        std::cout << "  2. packages/ directory" << std::endl;
        std::cout << std::endl;
        
        for (const auto& module : missingModules) {
            std::cout << "❌ Module not found: " << module << std::endl;
        }
        
        std::cout << "\nPossible solutions:" << std::endl;
        std::cout << "  - Check if the module file exists" << std::endl;
        std::cout << "  - Verify the import path is correct" << std::endl;
        std::cout << "  - Create the missing module file" << std::endl;
        std::cout << "  - Check if the module is in the packages/ directory" << std::endl;
        std::cout << "=====================================" << std::endl;
    }
} 