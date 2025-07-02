#include "CodeGen.h"
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
#include <cstdlib>

CodeGenerator::CodeGenerator() : debugCompileUnit(nullptr), debugFile(nullptr) {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("flast", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    debugBuilder = std::make_unique<llvm::DIBuilder>(*module);
    
    createBuiltinFunctions();
}

void CodeGenerator::setupProjectStructure(const std::string& sourceFile) {
    // Get source file directory as project root
    projectRoot = std::filesystem::absolute(std::filesystem::path(sourceFile)).parent_path();
    
    // Create build structure
    buildDir = projectRoot / ".build";
    binDir = buildDir / "bin";
    cacheDir = buildDir / "cache";
    
    // Clean existing build directory if it exists
    if (std::filesystem::exists(buildDir)) {
        std::cout << "🧹 Cleaning existing build directory..." << std::endl;
        try {
            std::filesystem::remove_all(buildDir);
            std::cout << "✓ Removed old build files" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "⚠️  Warning: Could not fully clean build directory: " << e.what() << std::endl;
        }
    }
    
    // Create fresh directories
    std::filesystem::create_directories(binDir);
    std::filesystem::create_directories(cacheDir);
    
    std::cout << "📁 Project structure:" << std::endl;
    std::cout << "  Root: " << projectRoot << std::endl;
    std::cout << "  Build: " << buildDir << " (fresh)" << std::endl;
    std::cout << "  Binaries: " << binDir << std::endl;
    std::cout << "  Cache: " << cacheDir << std::endl;
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
    if (typeName == "int") {
        return llvm::Type::getInt32Ty(*context);
    } else if (typeName == "double") {
        return llvm::Type::getDoubleTy(*context);
    } else if (typeName == "string") {
        return llvm::Type::getInt8PtrTy(*context);
    } else if (typeName == "void") {
        return llvm::Type::getVoidTy(*context);
    } else {
        // Check if it's a class type
        auto it = classes.find(typeName);
        if (it != classes.end()) {
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
    
    // Generate classes first
    for (auto& cls : program->classes) {
        codegen(cls.get());
    }
    
    // Generate functions
    for (auto& func : program->functions) {
        codegen(func.get());
    }
    
    // Finalize debug info only if enabled
    // debugBuilder->finalize();
}

llvm::StructType* CodeGenerator::codegen(ClassAST* cls) {
    std::vector<llvm::Type*> memberTypes;
    
    // Add member variable types
    for (auto& member : cls->members) {
        memberTypes.push_back(getFlastType(member->type));
    }
    
    llvm::StructType* structType = llvm::StructType::create(*context, cls->name);
    structType->setBody(memberTypes);
    
    classes[cls->name] = structType;
    
    // Generate methods (simplified for now)
    for (auto& method : cls->methods) {
        // Add 'self' parameter for methods
        auto params = method->params;
        params.insert(params.begin(), {"self", cls->name});
        
        // Create a temporary function AST with modified parameters
        auto methodFunc = std::make_unique<FunctionAST>(
            cls->name + "_" + method->name,
            std::move(params),
            method->returnType,
            std::vector<StmtPtr>(),
            method->isPublic
        );
        
        codegen(methodFunc.get());
    }
    
    return structType;
}

llvm::Function* CodeGenerator::codegen(FunctionAST* func) {
    // Create function signature
    std::vector<llvm::Type*> argTypes;
    for (auto& param : func->params) {
        argTypes.push_back(getFlastType(param.second));
    }
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        getFlastType(func->returnType), argTypes, false);
    
    llvm::Function* function = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, func->name, module.get());
    
    functions[func->name] = function;
    
    // Debug info temporarily disabled to fix compilation
    // if (debugCompileUnit && debugFile) {
    //     llvm::DISubroutineType* funcDebugType = debugBuilder->createSubroutineType(
    //         debugBuilder->getOrCreateTypeArray(llvm::None));
    //     
    //     llvm::DISubprogram* funcDebugInfo = debugBuilder->createFunction(
    //         debugCompileUnit,   // Scope (use compile unit instead of file)
    //         func->name,         // Name
    //         func->name,         // Linkage name
    //         debugFile,          // File
    //         1,                  // Line number
    //         funcDebugType,      // Type
    //         1,                  // Scope line
    //         llvm::DINode::FlagPrototyped, // Flags
    //         llvm::DISubprogram::SPFlagDefinition // SP Flags (remove optimization flag)
    //     );
    //     
    //     function->setSubprogram(funcDebugInfo);
    // }
    
    // Create entry block
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(entry);
    
    // Create allocas for parameters
    auto argIt = function->arg_begin();
    for (size_t i = 0; i < func->params.size(); ++i) {
        llvm::AllocaInst* alloca = builder->CreateAlloca(
            getFlastType(func->params[i].second), nullptr, func->params[i].first);
        
        builder->CreateStore(&(*argIt), alloca);
        namedValues[func->params[i].first] = alloca;
        ++argIt;
    }
    
    // Generate function body
    llvm::Value* retVal = nullptr;
    for (auto& stmt : func->body) {
        retVal = codegen(stmt.get());
    }
    
    // Add return if not present
    if (!retVal || !llvm::isa<llvm::ReturnInst>(retVal)) {
        if (func->returnType == "void") {
            builder->CreateRetVoid();
        } else {
            builder->CreateRet(llvm::ConstantInt::get(*context, llvm::APInt(32, 0)));
        }
    }
    
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
    }
    
    throw std::runtime_error("Unknown statement type");
}

llvm::Value* CodeGenerator::codegenNumber(NumberExprAST* expr) {
    if (expr->value == (int)expr->value) {
        return llvm::ConstantInt::get(*context, llvm::APInt(32, (int)expr->value));
    } else {
        return llvm::ConstantFP::get(*context, llvm::APFloat(expr->value));
    }
}

llvm::Value* CodeGenerator::codegenString(StringExprAST* expr) {
    return builder->CreateGlobalStringPtr(expr->value);
}

llvm::Value* CodeGenerator::codegenVariable(VariableExprAST* expr) {
    if (expr->name == "EXIT_SUCCESS") {
        return llvm::ConstantInt::get(*context, llvm::APInt(32, 0));
    }
    
    llvm::Value* var = namedValues[expr->name];
    if (!var) {
        throw std::runtime_error("Unknown variable: " + expr->name);
    }
    
    return builder->CreateLoad(var->getType()->getPointerElementType(), var, expr->name);
}

llvm::Value* CodeGenerator::codegenBinary(BinaryExprAST* expr) {
    llvm::Value* lhs = codegen(expr->lhs.get());
    llvm::Value* rhs = codegen(expr->rhs.get());
    
    if (!lhs || !rhs) {
        return nullptr;
    }
    
    if (expr->op == "+") {
        return builder->CreateAdd(lhs, rhs, "addtmp");
    } else if (expr->op == "-") {
        return builder->CreateSub(lhs, rhs, "subtmp");
    } else if (expr->op == "*") {
        return builder->CreateMul(lhs, rhs, "multmp");
    } else if (expr->op == "/") {
        return builder->CreateSDiv(lhs, rhs, "divtmp");
    } else if (expr->op == "<") {
        return builder->CreateICmpSLT(lhs, rhs, "cmptmp");
    } else if (expr->op == ">") {
        return builder->CreateICmpSGT(lhs, rhs, "cmptmp");
    } else if (expr->op == "==") {
        return builder->CreateICmpEQ(lhs, rhs, "cmptmp");
    } else if (expr->op == "!=") {
        return builder->CreateICmpNE(lhs, rhs, "cmptmp");
    }
    
    throw std::runtime_error("Unknown binary operator: " + expr->op);
}

llvm::Value* CodeGenerator::codegenCall(CallExprAST* expr) {
    if (expr->callee == "println") {
        // Handle println specially
        std::vector<llvm::Value*> args;
        
        for (auto& arg : expr->args) {
            llvm::Value* argVal = codegen(arg.get());
            args.push_back(argVal);
        }
        
        // Create format string based on argument types
        std::string format = "";
        std::vector<llvm::Value*> printfArgs;
        
        for (auto& arg : args) {
            if (arg->getType()->isIntegerTy()) {
                format += "%d ";
                printfArgs.push_back(arg);
            } else if (arg->getType()->isDoubleTy()) {
                format += "%f ";
                printfArgs.push_back(arg);
            } else if (arg->getType()->isPointerTy()) {
                format += "%s ";
                printfArgs.push_back(arg);
            }
        }
        format += "\n";
        
        llvm::Value* formatStr = builder->CreateGlobalStringPtr(format);
        printfArgs.insert(printfArgs.begin(), formatStr);
        
        llvm::Function* printf = module->getFunction("printf");
        return builder->CreateCall(printf, printfArgs);
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

llvm::Value* CodeGenerator::codegenNew(NewExprAST* expr) {
    auto it = classes.find(expr->className);
    if (it == classes.end()) {
        throw std::runtime_error("Unknown class: " + expr->className);
    }
    
    llvm::StructType* structType = it->second;
    llvm::Value* size = llvm::ConstantInt::get(*context, 
        llvm::APInt(64, module->getDataLayout().getTypeAllocSize(structType)));
    
    llvm::Value* ptr = builder->CreateCall(mallocFunc, {size});
    return builder->CreateBitCast(ptr, llvm::PointerType::get(structType, 0));
}

llvm::Value* CodeGenerator::codegenVarDecl(VarDeclStmtAST* stmt) {
    llvm::Type* type = getFlastType(stmt->type);
    llvm::AllocaInst* alloca = builder->CreateAlloca(type, nullptr, stmt->name);
    
    if (stmt->initializer) {
        llvm::Value* initVal = codegen(stmt->initializer.get());
        builder->CreateStore(initVal, alloca);
    }
    
    namedValues[stmt->name] = alloca;
    return alloca;
}

llvm::Value* CodeGenerator::codegenAssign(AssignStmtAST* stmt) {
    llvm::Value* rhs = codegen(stmt->rhs.get());
    
    if (auto varExpr = dynamic_cast<VariableExprAST*>(stmt->lhs.get())) {
        llvm::Value* var = namedValues[varExpr->name];
        if (!var) {
            throw std::runtime_error("Unknown variable: " + varExpr->name);
        }
        return builder->CreateStore(rhs, var);
    }
    
    throw std::runtime_error("Invalid assignment target");
}

llvm::Value* CodeGenerator::codegenExprStmt(ExprStmtAST* stmt) {
    return codegen(stmt->expr.get());
}

llvm::Value* CodeGenerator::codegenReturn(ReturnStmtAST* stmt) {
    if (stmt->expr) {
        llvm::Value* retVal = codegen(stmt->expr.get());
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
    
    // Generate object file
    writeObjectFile(objFile.string());
    std::cout << "✓ Object file: " << objFile << std::endl;
    
    // Link to executable with proper flags
    std::string linkCmd = "gcc -no-pie";
    
    if (debugMode) {
        linkCmd += " -g";  // Add debug symbols
        std::cout << "✓ Debug mode enabled" << std::endl;
    }
    
    if (optimized) {
        linkCmd += " -O2";  // Optimization
        std::cout << "✓ Optimization enabled" << std::endl;
    }
    
    linkCmd += " \"" + objFile.string() + "\" -o \"" + exeFile.string() + "\"";
    
    std::cout << "Linking..." << std::endl;
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
            for (const auto& entry : std::filesystem::directory_iterator(cacheDir)) {
                std::filesystem::remove_all(entry);
            }
            std::cout << "🧹 Cache cleaned (keeping binaries)" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "⚠️  Warning: Could not clean cache: " << e.what() << std::endl;
    }
} 