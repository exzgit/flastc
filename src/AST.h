#pragma once
#include <string>
#include <vector>
#include <memory>

// Forward declarations
class ExprAST;
class StmtAST;

using ExprPtr = std::unique_ptr<ExprAST>;
using StmtPtr = std::unique_ptr<StmtAST>;

// Base Expression class
class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual std::string toString() const = 0;
};

// Base Statement class
class StmtAST {
public:
    virtual ~StmtAST() = default;
    virtual std::string toString() const = 0;
};

// Number literal expression
class NumberExprAST : public ExprAST {
public:
    double value;
    
    NumberExprAST(double val) : value(val) {}
    std::string toString() const override {
        return std::to_string(value);
    }
};

// String literal expression
class StringExprAST : public ExprAST {
public:
    std::string value;
    
    StringExprAST(const std::string& val) : value(val) {}
    std::string toString() const override {
        return "\"" + value + "\"";
    }
};

// Variable reference expression
class VariableExprAST : public ExprAST {
public:
    std::string name;
    
    VariableExprAST(const std::string& name) : name(name) {}
    std::string toString() const override {
        return name;
    }
};

// Binary operation expression
class BinaryExprAST : public ExprAST {
public:
    std::string op;
    ExprPtr lhs, rhs;
    
    BinaryExprAST(const std::string& op, ExprPtr lhs, ExprPtr rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    
    std::string toString() const override {
        return "(" + lhs->toString() + " " + op + " " + rhs->toString() + ")";
    }
};

// Function call expression
class CallExprAST : public ExprAST {
public:
    std::string callee;
    std::vector<ExprPtr> args;
    
    CallExprAST(const std::string& callee, std::vector<ExprPtr> args)
        : callee(callee), args(std::move(args)) {}
    
    std::string toString() const override {
        std::string result = callee + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->toString();
        }
        result += ")";
        return result;
    }
};

// Member access expression (obj.member)
class MemberAccessExprAST : public ExprAST {
public:
    ExprPtr object;
    std::string member;
    
    MemberAccessExprAST(ExprPtr object, const std::string& member)
        : object(std::move(object)), member(member) {}
    
    std::string toString() const override {
        return object->toString() + "." + member;
    }
};

// New expression (object instantiation)
class NewExprAST : public ExprAST {
public:
    std::string className;
    std::vector<ExprPtr> args;
    
    NewExprAST(const std::string& className, std::vector<ExprPtr> args)
        : className(className), args(std::move(args)) {}
    
    std::string toString() const override {
        std::string result = "new " + className + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->toString();
        }
        result += ")";
        return result;
    }
};

// Variable declaration statement
class VarDeclStmtAST : public StmtAST {
public:
    std::string name;
    std::string type;
    ExprPtr initializer;
    bool isPublic;
    
    VarDeclStmtAST(const std::string& name, const std::string& type, 
                   ExprPtr initializer = nullptr, bool isPublic = false)
        : name(name), type(type), initializer(std::move(initializer)), isPublic(isPublic) {}
    
    std::string toString() const override {
        std::string result = isPublic ? "pub " : "";
        result += "let " + name + ": " + type;
        if (initializer) {
            result += " = " + initializer->toString();
        }
        return result + ";";
    }
};

// Assignment statement
class AssignStmtAST : public StmtAST {
public:
    ExprPtr lhs;
    ExprPtr rhs;
    
    AssignStmtAST(ExprPtr lhs, ExprPtr rhs)
        : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    
    std::string toString() const override {
        return lhs->toString() + " = " + rhs->toString() + ";";
    }
};

// Expression statement
class ExprStmtAST : public StmtAST {
public:
    ExprPtr expr;
    
    ExprStmtAST(ExprPtr expr) : expr(std::move(expr)) {}
    
    std::string toString() const override {
        return expr->toString() + ";";
    }
};

// Return statement
class ReturnStmtAST : public StmtAST {
public:
    ExprPtr expr;
    
    ReturnStmtAST(ExprPtr expr = nullptr) : expr(std::move(expr)) {}
    
    std::string toString() const override {
        if (expr) {
            return "return " + expr->toString() + ";";
        }
        return "return;";
    }
};

// Function declaration
class FunctionAST {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> params; // (name, type)
    std::string returnType;
    std::vector<StmtPtr> body;
    bool isPublic;
    
    FunctionAST(const std::string& name, 
                std::vector<std::pair<std::string, std::string>> params,
                const std::string& returnType,
                std::vector<StmtPtr> body,
                bool isPublic = false)
        : name(name), params(std::move(params)), returnType(returnType), 
          body(std::move(body)), isPublic(isPublic) {}
    
    std::string toString() const {
        std::string result = isPublic ? "pub " : "";
        result += "func " + name + "(";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) result += ", ";
            result += params[i].first + ": " + params[i].second;
        }
        result += ") -> " + returnType + " {\n";
        for (const auto& stmt : body) {
            result += "    " + stmt->toString() + "\n";
        }
        result += "}";
        return result;
    }
};

// Class declaration
class ClassAST {
public:
    std::string name;
    std::vector<std::unique_ptr<VarDeclStmtAST>> members;
    std::vector<std::unique_ptr<FunctionAST>> methods;
    
    ClassAST(const std::string& name) : name(name) {}
    
    std::string toString() const {
        std::string result = "class " + name + " {\n";
        for (const auto& member : members) {
            result += "    " + member->toString() + "\n";
        }
        for (const auto& method : methods) {
            result += "    " + method->toString() + "\n";
        }
        result += "}";
        return result;
    }
};

// Import statement
class ImportAST {
public:
    std::string module;
    std::string alias;
    
    ImportAST(const std::string& module, const std::string& alias = "")
        : module(module), alias(alias) {}
    
    std::string toString() const {
        if (!alias.empty()) {
            return "import " + alias + " from \"" + module + "\";";
        }
        return "import \"" + module + "\";";
    }
};

// Program (top-level AST node)
class ProgramAST {
public:
    std::vector<std::unique_ptr<ImportAST>> imports;
    std::vector<std::unique_ptr<ClassAST>> classes;
    std::vector<std::unique_ptr<FunctionAST>> functions;
    
    std::string toString() const {
        std::string result;
        for (const auto& import : imports) {
            result += import->toString() + "\n";
        }
        for (const auto& cls : classes) {
            result += cls->toString() + "\n\n";
        }
        for (const auto& func : functions) {
            result += func->toString() + "\n\n";
        }
        return result;
    }
}; 