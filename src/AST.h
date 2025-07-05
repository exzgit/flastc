#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include <unordered_map>
#include <iostream>

// Forward declarations
struct ExprAST;
struct StmtAST;
struct DeclAST;
struct NumberExprAST;
struct ScientificExprAST;
struct StringExprAST;
struct BoolExprAST;
struct NullExprAST;
struct VariableExprAST;
struct BinaryExprAST;
struct UnaryExprAST;
struct CallExprAST;
struct MemberAccessExprAST;
struct IndexExprAST;
struct NewExprAST;
struct ListExprAST;
struct MapExprAST;
struct TupleExprAST;
struct LambdaExprAST;
struct BuiltinMethodExprAST;
struct MethodCallExprAST;
struct TypeCastAST;
struct ArrayExprAST;
struct VarDeclStmtAST;
struct AssignStmtAST;
struct ExprStmtAST;
struct ReturnStmtAST;
struct BlockStmtAST;
struct IfStmtAST;
struct WhileStmtAST;
struct ForStmtAST;
struct ForInStmtAST;
struct MatchStmtAST;
struct BreakStmtAST;
struct ContinueStmtAST;
struct TryCatchStmtAST;
struct ThrowStmtAST;
struct StructDeclAST;
struct EnumDeclAST;
struct TraitDeclAST;
struct ImplDeclAST;
struct FunctionDeclAST;

struct ImportDeclAST;
struct ModuleDeclAST;
struct ProgramAST;

// ==================== AST VISITOR INTERFACE ====================
// Visitor interface
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    // Expression visitors
    virtual void visit(NumberExprAST& node) = 0;
    virtual void visit(ScientificExprAST& node) = 0;
    virtual void visit(StringExprAST& node) = 0;
    virtual void visit(BoolExprAST& node) = 0;
    virtual void visit(NullExprAST& node) = 0;
    virtual void visit(VariableExprAST& node) = 0;
    virtual void visit(BinaryExprAST& node) = 0;
    virtual void visit(UnaryExprAST& node) = 0;
    virtual void visit(CallExprAST& node) = 0;
    virtual void visit(MemberAccessExprAST& node) = 0;
    virtual void visit(IndexExprAST& node) = 0;
    virtual void visit(NewExprAST& node) = 0;
    virtual void visit(ListExprAST& node) = 0;
    virtual void visit(MapExprAST& node) = 0;
    virtual void visit(TupleExprAST& node) = 0;
    virtual void visit(LambdaExprAST& node) = 0;
    virtual void visit(BuiltinMethodExprAST& node) = 0;
    virtual void visit(MethodCallExprAST& node) = 0;
    virtual void visit(TypeCastAST& node) = 0;
    virtual void visit(ArrayExprAST& node) = 0;
    // Statement visitors
    virtual void visit(VarDeclStmtAST& node) = 0;
    virtual void visit(AssignStmtAST& node) = 0;
    virtual void visit(ExprStmtAST& node) = 0;
    virtual void visit(ReturnStmtAST& node) = 0;
    virtual void visit(BlockStmtAST& node) = 0;
    virtual void visit(IfStmtAST& node) = 0;
    virtual void visit(WhileStmtAST& node) = 0;
    virtual void visit(ForStmtAST& node) = 0;
    virtual void visit(ForInStmtAST& node) = 0;
    virtual void visit(MatchStmtAST& node) = 0;
    virtual void visit(BreakStmtAST& node) = 0;
    virtual void visit(ContinueStmtAST& node) = 0;
    virtual void visit(TryCatchStmtAST& node) = 0;
    virtual void visit(ThrowStmtAST& node) = 0;
    // Declaration visitors
    virtual void visit(StructDeclAST& node) = 0;
    virtual void visit(EnumDeclAST& node) = 0;
    virtual void visit(TraitDeclAST& node) = 0;
    virtual void visit(ImplDeclAST& node) = 0;
    virtual void visit(FunctionDeclAST& node) = 0;

    virtual void visit(ImportDeclAST& node) = 0;
    virtual void visit(ModuleDeclAST& node) = 0;
    // Program visitor
    virtual void visit(ProgramAST& node) = 0;
};

// Enhanced Rust-like type system
enum class FlastType {
    VOID,
    I8, I16, I32, I64, I128,
    U8, U16, U32, U64, U128,
    F32, F64,
    CHAR, STR, STRING,
    BOOL,
    VEC, ARRAY, SLICE,
    MAP, SET, TUPLE,
    OPTION, RESULT,
    STRUCT, ENUM, UNION, TRAIT,
    FUNCTION, CLOSURE,
    MODULE, CRATE,
    BOX, REF,
    SELF,
    AUTO, UNKNOWN
};

struct TypeInfo {
    FlastType type;
    std::string className;
    std::vector<std::shared_ptr<TypeInfo>> parameters;
    bool isPointer = false;
    bool isReference = false;
    bool isConst = false;
    bool isOptional = false;
    
    TypeInfo(FlastType t = FlastType::UNKNOWN) : type(t) {}
    TypeInfo(FlastType t, const std::string& className) : type(t), className(className) {}
    
    std::string toString() const {
        switch (type) {
            case FlastType::VOID: return "void";
            case FlastType::I8: return "i8";
            case FlastType::I16: return "i16";
            case FlastType::I32: return "i32";
            case FlastType::I64: return "i64";
            case FlastType::I128: return "i128";
            case FlastType::U8: return "u8";
            case FlastType::U16: return "u16";
            case FlastType::U32: return "u32";
            case FlastType::U64: return "u64";
            case FlastType::U128: return "u128";
            case FlastType::F32: return "f32";
            case FlastType::F64: return "f64";
            case FlastType::CHAR: return "char";
            case FlastType::STR: return "str";
            case FlastType::STRING: return "string";
            case FlastType::BOOL: return "bool";
            case FlastType::VEC: return "vec";
            case FlastType::ARRAY: return "array";
            case FlastType::SLICE: return "slice";
            case FlastType::MAP: return "map";
            case FlastType::SET: return "set";
            case FlastType::TUPLE: return "tuple";
            case FlastType::OPTION: return "option";
            case FlastType::RESULT: return "result";
            case FlastType::STRUCT: return className.empty() ? "struct" : className;

            case FlastType::TRAIT: return "trait";
            case FlastType::ENUM: return "enum";
            case FlastType::UNION: return "union";
            case FlastType::FUNCTION: return "function";
            case FlastType::CLOSURE: return "closure";
            case FlastType::MODULE: return "module";
            case FlastType::CRATE: return "crate";
            case FlastType::BOX: return "box";
            case FlastType::REF: return "ref";
            case FlastType::SELF: return "self";
            case FlastType::AUTO: return "auto";
            default: return "unknown";
        }
    }
};

// Base AST classes
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
    virtual std::string toString() const = 0;
    virtual std::string getNodeType() const = 0;
};

struct ExprAST : ASTNode {
    std::shared_ptr<TypeInfo> type;
    ExprAST() : type(std::make_shared<TypeInfo>()) {}
};

struct StmtAST : ASTNode {};

struct DeclAST : ASTNode {};

// ==================== EXPRESSIONS ====================

// Literal expressions
struct NumberExprAST : ExprAST {
    double value;
    bool isScientific;
    std::string originalText; 
    
    NumberExprAST(double val, bool scientific = false, const std::string& original = "") 
        : value(val), isScientific(scientific), originalText(original) {
        type = std::make_shared<TypeInfo>(FlastType::F64);
    }
    std::string toString() const override { 
        return originalText.empty() ? std::to_string(value) : originalText; 
    }
    std::string getNodeType() const override { return "NumberExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Scientific notation numbers (e.g., 10e1, 1.5e-3)
struct ScientificExprAST : ExprAST {
    double value;
    std::string originalText;
    
    ScientificExprAST(double val, const std::string& original) 
        : value(val), originalText(original) {
        type = std::make_shared<TypeInfo>(FlastType::F64);
    }
    std::string toString() const override { return originalText; }
    std::string getNodeType() const override { return "ScientificExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

struct StringExprAST : ExprAST {
    std::string value;
    StringExprAST(const std::string& val) : value(val) {
        type = std::make_shared<TypeInfo>(FlastType::STRING);
    }
    std::string toString() const override { return "\"" + value + "\""; }
    std::string getNodeType() const override { return "StringExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

struct BoolExprAST : ExprAST {
    bool value;
    BoolExprAST(bool val) : value(val) {
        type = std::make_shared<TypeInfo>(FlastType::BOOL);
    }
    std::string toString() const override { return value ? "true" : "false"; }
    std::string getNodeType() const override { return "BoolExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

struct NullExprAST : ExprAST {
    NullExprAST() {
        type = std::make_shared<TypeInfo>(FlastType::UNKNOWN);
        type->isOptional = true;
    }
    std::string toString() const override { return "null"; }
    std::string getNodeType() const override { return "NullExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Variable and identifiers
struct VariableExprAST : ExprAST {
    std::string name;
    VariableExprAST(const std::string& name) : name(name) {}
    std::string toString() const override { return name; }
    std::string getNodeType() const override { return "VariableExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Binary operations
struct BinaryExprAST : ExprAST {
    std::string op;
    std::shared_ptr<ExprAST> left, right;
    
    BinaryExprAST(const std::string& op, std::shared_ptr<ExprAST> left, std::shared_ptr<ExprAST> right)
        : op(op), left(left), right(right) {}
    
    std::string toString() const override {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }
    std::string getNodeType() const override { return "BinaryExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Unary operations
struct UnaryExprAST : ExprAST {
    std::string op;
    std::shared_ptr<ExprAST> operand;
    bool isPrefix;
    
    UnaryExprAST(const std::string& op, std::shared_ptr<ExprAST> operand, bool isPrefix = true)
        : op(op), operand(operand), isPrefix(isPrefix) {}
    
    std::string toString() const override {
        if (isPrefix) {
            return op + operand->toString();
        } else {
            return operand->toString() + op;
        }
    }
    std::string getNodeType() const override { return "UnaryExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Function calls
struct CallExprAST : ExprAST {
    std::string callee;
    std::vector<std::shared_ptr<ExprAST>> args;
    
    CallExprAST(const std::string& callee, std::vector<std::shared_ptr<ExprAST>> args)
        : callee(callee), args(args) {}
    
    std::string toString() const override {
        std::string result = callee + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->toString();
        }
        result += ")";
        return result;
    }
    std::string getNodeType() const override { return "CallExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Member access (obj.member)
struct MemberAccessExprAST : ExprAST {
    std::shared_ptr<ExprAST> object;
    std::string member;
    bool isSafeAccess = false;  
    
    MemberAccessExprAST(std::shared_ptr<ExprAST> object, const std::string& member, bool isSafeAccess = false)
        : object(object), member(member), isSafeAccess(isSafeAccess) {}
    
    std::string toString() const override {
        return object->toString() + (isSafeAccess ? "?." : ".") + member;
    }
    std::string getNodeType() const override { return "MemberAccessExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Array/List access (arr[index])
struct IndexExprAST : ExprAST {
    std::shared_ptr<ExprAST> object;
    std::shared_ptr<ExprAST> index;
    
    IndexExprAST(std::shared_ptr<ExprAST> object, std::shared_ptr<ExprAST> index)
        : object(object), index(index) {}
    
    std::string toString() const override {
        return object->toString() + "[" + index->toString() + "]";
    }
    std::string getNodeType() const override { return "IndexExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Object creation (new Class())
struct NewExprAST : ExprAST {
    std::string className;
    std::vector<std::shared_ptr<ExprAST>> args;
    
    NewExprAST(const std::string& className, std::vector<std::shared_ptr<ExprAST>> args)
        : className(className), args(args) {}
    
    std::string toString() const override {
        std::string result = "new " + className + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->toString();
        }
        result += ")";
        return result;
    }
    std::string getNodeType() const override { return "NewExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// List literals [1, 2, 3]
struct ListExprAST : ExprAST {
    std::vector<std::shared_ptr<ExprAST>> elements;
    
    ListExprAST(std::vector<std::shared_ptr<ExprAST>> elements) : elements(elements) {
        type = std::make_shared<TypeInfo>(FlastType::VEC);
    }
    
    std::string toString() const override {
        std::string result = "[";
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) result += ", ";
            result += elements[i]->toString();
        }
        result += "]";
        return result;
    }
    std::string getNodeType() const override { return "ListExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Map literals {key: value, ...}
struct MapExprAST : ExprAST {
    std::vector<std::pair<std::shared_ptr<ExprAST>, std::shared_ptr<ExprAST>>> pairs;
    
    MapExprAST(std::vector<std::pair<std::shared_ptr<ExprAST>, std::shared_ptr<ExprAST>>> pairs) 
        : pairs(pairs) {
        type = std::make_shared<TypeInfo>(FlastType::MAP);
    }
    
    std::string toString() const override {
        std::string result = "{";
        for (size_t i = 0; i < pairs.size(); ++i) {
            if (i > 0) result += ", ";
            result += pairs[i].first->toString() + ": " + pairs[i].second->toString();
        }
        result += "}";
        return result;
    }
    std::string getNodeType() const override { return "MapExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Tuple literals (a, b, c)
struct TupleExprAST : ExprAST {
    std::vector<std::shared_ptr<ExprAST>> elements;
    
    TupleExprAST(std::vector<std::shared_ptr<ExprAST>> elements) : elements(elements) {
        type = std::make_shared<TypeInfo>(FlastType::TUPLE);
    }
    
    std::string toString() const override {
        std::string result = "(";
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) result += ", ";
            result += elements[i]->toString();
        }
        result += ")";
        return result;
    }
    std::string getNodeType() const override { return "TupleExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Lambda expressions
struct LambdaExprAST : ExprAST {
    std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>> parameters;
    std::shared_ptr<TypeInfo> returnType;
    std::vector<std::shared_ptr<StmtAST>> body;
    
    LambdaExprAST(std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>> parameters,
                  std::shared_ptr<TypeInfo> returnType,
                  std::vector<std::shared_ptr<StmtAST>> body)
        : parameters(parameters), returnType(returnType), body(body) {
        type = std::make_shared<TypeInfo>(FlastType::FUNCTION);
    }
    
    std::string toString() const override {
        std::string result = "lambda(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) result += ", ";
            result += parameters[i].first + ": " + parameters[i].second->toString();
        }
        result += ") -> " + returnType->toString() + " { ... }";
        return result;
    }
    std::string getNodeType() const override { return "LambdaExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Built-in method calls (obj.type(), value.tostring())
struct BuiltinMethodExprAST : ExprAST {
    std::shared_ptr<ExprAST> object;
    std::string methodName;
    std::vector<std::shared_ptr<ExprAST>> args;
    
    BuiltinMethodExprAST(std::shared_ptr<ExprAST> object, const std::string& methodName,
                         std::vector<std::shared_ptr<ExprAST>> args)
        : object(object), methodName(methodName), args(args) {}
    
    std::string toString() const override {
        std::string result = object->toString() + "." + methodName + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->toString();
        }
        result += ")";
        return result;
    }
    std::string getNodeType() const override { return "BuiltinMethodExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Method call expressions (object.method())
struct MethodCallExprAST : ExprAST {
    std::shared_ptr<ExprAST> object;
    std::string method;
    std::vector<std::shared_ptr<ExprAST>> args;
    
    MethodCallExprAST(std::shared_ptr<ExprAST> object, const std::string& method, 
                      std::vector<std::shared_ptr<ExprAST>> args)
        : object(object), method(method), args(args) {}
    
    std::string toString() const override {
        std::string result = object->toString() + "." + method + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += ", ";
            result += args[i]->toString();
        }
        return result + ")";
    }
    std::string getNodeType() const override { return "MethodCallExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Type casting
struct TypeCastAST : ExprAST {
    std::shared_ptr<ExprAST> expression;
    std::shared_ptr<TypeInfo> targetType;
    
    TypeCastAST(std::shared_ptr<ExprAST> expression, std::shared_ptr<TypeInfo> targetType)
        : expression(expression), targetType(targetType) {}
    
    std::string toString() const override {
        return expression->toString() + " as " + targetType->toString();
    }
    std::string getNodeType() const override { return "TypeCast"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Array expressions (for compatibility with ASTVisitor)
struct ArrayExprAST : ExprAST {
    std::vector<std::shared_ptr<ExprAST>> elements;
    
    ArrayExprAST(std::vector<std::shared_ptr<ExprAST>> elements) : elements(elements) {
        type = std::make_shared<TypeInfo>(FlastType::ARRAY);
    }
    
    std::string toString() const override {
        std::string result = "[";
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) result += ", ";
            result += elements[i]->toString();
        }
        result += "]";
        return result;
    }
    std::string getNodeType() const override { return "ArrayExpr"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// ==================== STATEMENTS ====================

// Variable declarations
struct VarDeclStmtAST : StmtAST {
    std::string name;
    std::shared_ptr<TypeInfo> type;
    std::shared_ptr<ExprAST> initializer;
    bool isConst = false;
    bool isPublic = false;
    
    VarDeclStmtAST(const std::string& name, std::shared_ptr<TypeInfo> type, 
                   std::shared_ptr<ExprAST> initializer = nullptr, bool isConst = false, bool isPublic = false)
        : name(name), type(type), initializer(initializer), isConst(isConst), isPublic(isPublic) {}
    
    std::string toString() const override {
        std::string result = std::string(isPublic ? "pub " : "") + std::string(isConst ? "const " : "let ") + name;
        if (type) result += ": " + type->toString();
        if (initializer) result += " = " + initializer->toString();
        return result + ";";
    }
    std::string getNodeType() const override { return "VarDeclStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Assignment statements
struct AssignStmtAST : StmtAST {
    std::shared_ptr<ExprAST> target;
    std::string op;  // =, +=, -=, etc.
    std::shared_ptr<ExprAST> value;
    
    AssignStmtAST(std::shared_ptr<ExprAST> target, const std::string& op, std::shared_ptr<ExprAST> value)
        : target(target), op(op), value(value) {}
    
    std::string toString() const override {
        return target->toString() + " " + op + " " + value->toString() + ";";
    }
    std::string getNodeType() const override { return "AssignStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Expression statements
struct ExprStmtAST : StmtAST {
    std::shared_ptr<ExprAST> expression;
    
    ExprStmtAST(std::shared_ptr<ExprAST> expression) : expression(expression) {}
    
    std::string toString() const override {
        return expression->toString() + ";";
    }
    std::string getNodeType() const override { return "ExprStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Return statements
struct ReturnStmtAST : StmtAST {
    std::shared_ptr<ExprAST> value;
    
    ReturnStmtAST(std::shared_ptr<ExprAST> value = nullptr) : value(value) {}
    
    std::string toString() const override {
        if (value) return "return " + value->toString() + ";";
        return "return;";
    }
    std::string getNodeType() const override { return "ReturnStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Block statements
struct BlockStmtAST : StmtAST {
    std::vector<std::shared_ptr<StmtAST>> statements;
    
    BlockStmtAST(std::vector<std::shared_ptr<StmtAST>> statements) : statements(statements) {}
    
    std::string toString() const override {
        std::string result = "{\n";
        for (const auto& stmt : statements) {
            result += "  " + stmt->toString() + "\n";
        }
        result += "}";
        return result;
    }
    std::string getNodeType() const override { return "BlockStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// If statements
struct IfStmtAST : StmtAST {
    std::shared_ptr<ExprAST> condition;
    std::shared_ptr<StmtAST> thenStmt;
    std::shared_ptr<StmtAST> elseStmt;
    
    IfStmtAST(std::shared_ptr<ExprAST> condition, std::shared_ptr<StmtAST> thenStmt, 
              std::shared_ptr<StmtAST> elseStmt = nullptr)
        : condition(condition), thenStmt(thenStmt), elseStmt(elseStmt) {}
    
    std::string toString() const override {
        std::string result = "if (" + condition->toString() + ") " + thenStmt->toString();
        if (elseStmt) result += " else " + elseStmt->toString();
        return result;
    }
    std::string getNodeType() const override { return "IfStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// While statements
struct WhileStmtAST : StmtAST {
    std::shared_ptr<ExprAST> condition;
    std::shared_ptr<StmtAST> body;
    
    WhileStmtAST(std::shared_ptr<ExprAST> condition, std::shared_ptr<StmtAST> body)
        : condition(condition), body(body) {}
    
    std::string toString() const override {
        return "while (" + condition->toString() + ") " + body->toString();
    }
    std::string getNodeType() const override { return "WhileStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// For statements
struct ForStmtAST : StmtAST {
    std::shared_ptr<StmtAST> init;
    std::shared_ptr<ExprAST> condition;
    std::shared_ptr<StmtAST> update;
    std::shared_ptr<StmtAST> body;
    
    ForStmtAST(std::shared_ptr<StmtAST> init, std::shared_ptr<ExprAST> condition,
               std::shared_ptr<StmtAST> update, std::shared_ptr<StmtAST> body)
        : init(init), condition(condition), update(update), body(body) {}
    
    std::string toString() const override {
        std::string result = "for (";
        if (init) result += init->toString();
        result += " ";
        if (condition) result += condition->toString();
        result += "; ";
        if (update) result += update->toString();
        result += ") ";
        if (body) result += body->toString();
        return result;
    }
    std::string getNodeType() const override { return "ForStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// For-in statements (for item in collection)
struct ForInStmtAST : StmtAST {
    std::string variable;
    std::shared_ptr<ExprAST> iterable;
    std::shared_ptr<StmtAST> body;
    
    ForInStmtAST(const std::string& variable, std::shared_ptr<ExprAST> iterable, std::shared_ptr<StmtAST> body)
        : variable(variable), iterable(iterable), body(body) {}
    
    std::string toString() const override {
        return "for " + variable + " in " + iterable->toString() + " " + body->toString();
    }
    std::string getNodeType() const override { return "ForInStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Break/Continue statements
struct BreakStmtAST : StmtAST {
    std::string toString() const override { return "break;"; }
    std::string getNodeType() const override { return "BreakStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

struct ContinueStmtAST : StmtAST {
    std::string toString() const override { return "continue;"; }
    std::string getNodeType() const override { return "ContinueStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Match statements (Rust-like pattern matching)
struct MatchStmtAST : StmtAST {
    std::shared_ptr<ExprAST> value;
    std::vector<std::pair<std::shared_ptr<ExprAST>, std::shared_ptr<StmtAST>>> arms; // pattern -> body
    
    MatchStmtAST(std::shared_ptr<ExprAST> value, 
                 std::vector<std::pair<std::shared_ptr<ExprAST>, std::shared_ptr<StmtAST>>> arms)
        : value(value), arms(arms) {}
    
    std::string toString() const override {
        std::string result = "match " + value->toString() + " {\n";
        for (const auto& arm : arms) {
            result += "  " + arm.first->toString() + " => " + arm.second->toString() + ",\n";
        }
        result += "}";
        return result;
    }
    std::string getNodeType() const override { return "MatchStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// ==================== DECLARATIONS ====================

// Struct declarations (Rust-like)
struct StructDeclAST : DeclAST {
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>> fields;
    std::vector<std::shared_ptr<TypeInfo>> generics;
    bool isPublic = false;
    
    StructDeclAST(const std::string& name, 
                  std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>> fields,
                  std::vector<std::shared_ptr<TypeInfo>> generics = {},
                  bool isPublic = false)
        : name(name), fields(fields), generics(generics), isPublic(isPublic) {}
    
    std::string toString() const override {
        std::string result = std::string(isPublic ? "pub " : "") + "struct " + name;
        if (!generics.empty()) {
            result += "<";
            for (size_t i = 0; i < generics.size(); ++i) {
                if (i > 0) result += ", ";
                result += generics[i]->toString();
            }
            result += ">";
        }
        result += " {\n";
        for (const auto& field : fields) {
            result += "  " + field.first + ": " + field.second->toString() + ",\n";
        }
        result += "}";
        return result;
    }
    std::string getNodeType() const override { return "StructDecl"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Enum declarations (Rust-like)
struct EnumDeclAST : DeclAST {
    std::string name;
    std::vector<std::pair<std::string, std::vector<std::shared_ptr<TypeInfo>>>> variants; // name -> types
    std::vector<std::shared_ptr<TypeInfo>> generics;
    bool isPublic = false;
    
    EnumDeclAST(const std::string& name,
                std::vector<std::pair<std::string, std::vector<std::shared_ptr<TypeInfo>>>> variants,
                std::vector<std::shared_ptr<TypeInfo>> generics = {},
                bool isPublic = false)
        : name(name), variants(variants), generics(generics), isPublic(isPublic) {}
    
    std::string toString() const override {
        std::string result = std::string(isPublic ? "pub " : "") + "enum " + name;
        if (!generics.empty()) {
            result += "<";
            for (size_t i = 0; i < generics.size(); ++i) {
                if (i > 0) result += ", ";
                result += generics[i]->toString();
            }
            result += ">";
        }
        result += " {\n";
        for (const auto& variant : variants) {
            result += "  " + variant.first;
            if (!variant.second.empty()) {
                result += "(";
                for (size_t i = 0; i < variant.second.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += variant.second[i]->toString();
                }
                result += ")";
            }
            result += ",\n";
        }
        result += "}";
        return result;
    }
    std::string getNodeType() const override { return "EnumDecl"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Trait declarations (Rust-like interfaces)
struct TraitDeclAST : DeclAST {
    std::string name;
    std::vector<std::shared_ptr<DeclAST>> methods;
    std::vector<std::shared_ptr<TypeInfo>> generics;
    bool isPublic = false;
    
    TraitDeclAST(const std::string& name,
                 std::vector<std::shared_ptr<DeclAST>> methods,
                 std::vector<std::shared_ptr<TypeInfo>> generics = {},
                 bool isPublic = false)
        : name(name), methods(methods), generics(generics), isPublic(isPublic) {}
    
    std::string toString() const override {
        std::string result = std::string(isPublic ? "pub " : "") + "trait " + name;
        if (!generics.empty()) {
            result += "<";
            for (size_t i = 0; i < generics.size(); ++i) {
                if (i > 0) result += ", ";
                result += generics[i]->toString();
            }
            result += ">";
        }
        result += " {\n";
        for (const auto& method : methods) {
            result += "  " + method->toString() + "\n";
        }
        result += "}";
        return result;
    }
    std::string getNodeType() const override { return "TraitDecl"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Impl blocks (Rust-like implementations)
struct ImplDeclAST : DeclAST {
    std::shared_ptr<TypeInfo> targetType;
    std::shared_ptr<TypeInfo> traitType; // Optional, for trait implementations
    std::vector<std::shared_ptr<DeclAST>> methods;
    std::vector<std::shared_ptr<TypeInfo>> generics;
    
    ImplDeclAST(std::shared_ptr<TypeInfo> targetType,
                std::vector<std::shared_ptr<DeclAST>> methods,
                std::shared_ptr<TypeInfo> traitType = nullptr,
                std::vector<std::shared_ptr<TypeInfo>> generics = {})
        : targetType(targetType), traitType(traitType), methods(methods), generics(generics) {}
    
    std::string toString() const override {
        std::string result = "impl";
        if (!generics.empty()) {
            result += "<";
            for (size_t i = 0; i < generics.size(); ++i) {
                if (i > 0) result += ", ";
                result += generics[i]->toString();
            }
            result += ">";
        }
        if (traitType) {
            result += " " + traitType->toString() + " for";
        }
        result += " " + targetType->toString() + " {\n";
        for (const auto& method : methods) {
            result += "  " + method->toString() + "\n";
        }
        result += "}";
        return result;
    }
    std::string getNodeType() const override { return "ImplDecl"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Try-catch statements
struct TryCatchStmtAST : StmtAST {
    std::shared_ptr<StmtAST> tryBody;
    std::string exceptionVar;
    std::shared_ptr<TypeInfo> exceptionType;
    std::shared_ptr<StmtAST> catchBody;
    std::shared_ptr<StmtAST> finallyBody;
    
    TryCatchStmtAST(std::shared_ptr<StmtAST> tryBody, const std::string& exceptionVar,
                    std::shared_ptr<TypeInfo> exceptionType, std::shared_ptr<StmtAST> catchBody,
                    std::shared_ptr<StmtAST> finallyBody = nullptr)
        : tryBody(tryBody), exceptionVar(exceptionVar), exceptionType(exceptionType),
          catchBody(catchBody), finallyBody(finallyBody) {}
    
    std::string toString() const override {
        std::string result = "try " + tryBody->toString();
        result += " catch (" + exceptionVar + ": " + exceptionType->toString() + ") " + catchBody->toString();
        if (finallyBody) result += " finally " + finallyBody->toString();
        return result;
    }
    std::string getNodeType() const override { return "TryCatchStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Throw statements
struct ThrowStmtAST : StmtAST {
    std::shared_ptr<ExprAST> exception;
    
    ThrowStmtAST(std::shared_ptr<ExprAST> exception) : exception(exception) {}
    
    std::string toString() const override {
        return "throw " + exception->toString() + ";";
    }
    std::string getNodeType() const override { return "ThrowStmt"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// ==================== DECLARATIONS ====================

// Function parameters
struct ParameterAST {
    std::string name;
    std::shared_ptr<TypeInfo> type;
    std::shared_ptr<ExprAST> defaultValue;
    bool isOptional = false;
    
    ParameterAST(const std::string& name, std::shared_ptr<TypeInfo> type, 
                 std::shared_ptr<ExprAST> defaultValue = nullptr, bool isOptional = false)
        : name(name), type(type), defaultValue(defaultValue), isOptional(isOptional) {}
    
    std::string toString() const {
        std::string result = name + ": " + type->toString();
        if (defaultValue) result += " = " + defaultValue->toString();
        if (isOptional) result += "?";
        return result;
    }
};

// Function declarations
struct FunctionDeclAST : DeclAST {
    std::string name;
    std::vector<ParameterAST> parameters;
    std::shared_ptr<TypeInfo> returnType;
    std::shared_ptr<BlockStmtAST> body;
    bool isPublic = false;
    bool isStatic = false;
    bool isVirtual = false;
    bool isOverride = false;
    bool isAsync = false;
    bool isExtern = false;
    std::string externLang;  // "C", "C++", etc.
    
    FunctionDeclAST(const std::string& name, std::vector<ParameterAST> parameters,
                    std::shared_ptr<TypeInfo> returnType, std::shared_ptr<BlockStmtAST> body,
                    bool isPublic = false, bool isStatic = false, bool isVirtual = false,
                    bool isOverride = false, bool isAsync = false, bool isExtern = false,
                    const std::string& externLang = "")
        : name(name), parameters(parameters), returnType(returnType), body(body),
          isPublic(isPublic), isStatic(isStatic), isVirtual(isVirtual), isOverride(isOverride),
          isAsync(isAsync), isExtern(isExtern), externLang(externLang) {}
    
    std::string toString() const override {
        std::string result = "";
        if (isPublic) result += "pub ";
        if (isStatic) result += "static ";
        if (isVirtual) result += "virtual ";
        if (isOverride) result += "override ";
        if (isAsync) result += "async ";
        if (isExtern) result += "extern \"" + externLang + "\" ";
        
        result += "fn " + name + "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) result += ", ";
            result += parameters[i].toString();
        }
        result += ") -> " + returnType->toString();
        
        if (body && !isExtern) {
            result += " " + body->toString();
        } else {
            result += ";";
        }
        
        return result;
    }
    std::string getNodeType() const override { return "FunctionDecl"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Class field
struct FieldDeclAST {
    std::string name;
    std::shared_ptr<TypeInfo> type;
    std::shared_ptr<ExprAST> initializer;
    bool isPublic = false;
    bool isStatic = false;
    bool isConst = false;
    
    FieldDeclAST(const std::string& name, std::shared_ptr<TypeInfo> type,
                 std::shared_ptr<ExprAST> initializer = nullptr, bool isPublic = false,
                 bool isStatic = false, bool isConst = false)
        : name(name), type(type), initializer(initializer), isPublic(isPublic),
          isStatic(isStatic), isConst(isConst) {}
    
    std::string toString() const {
        std::string result = "";
        if (isPublic) result += "pub ";
        if (isStatic) result += "static ";
        if (isConst) result += "const ";
        
        result += name + ": " + type->toString();
        if (initializer) result += " = " + initializer->toString();
        return result + ";";
    }
};



// Import declarations
struct ImportDeclAST : DeclAST {
    std::string moduleName;
    std::string alias;
    std::vector<std::string> specificImports;
    bool isWildcard = false;
    
    ImportDeclAST(const std::string& moduleName, const std::string& alias = "",
                  std::vector<std::string> specificImports = {}, bool isWildcard = false)
        : moduleName(moduleName), alias(alias), specificImports(specificImports), isWildcard(isWildcard) {}
    
    std::string toString() const override {
        std::string result = "import ";
        if (isWildcard) {
            result += "*";
        } else if (!specificImports.empty()) {
            result += "{";
            for (size_t i = 0; i < specificImports.size(); ++i) {
                if (i > 0) result += ", ";
                result += specificImports[i];
            }
            result += "}";
        } else {
            result += moduleName;
        }
        
        if (!alias.empty()) {
            result += " as " + alias;
        }
        
        if (!moduleName.empty() && (!specificImports.empty() || isWildcard)) {
            result += " from " + moduleName;
        }
        
        return result + ";";
    }
    std::string getNodeType() const override { return "ImportDecl"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Module declaration
struct ModuleDeclAST : DeclAST {
    std::string name;
    std::vector<std::shared_ptr<DeclAST>> declarations;
    
    ModuleDeclAST(const std::string& name, std::vector<std::shared_ptr<DeclAST>> declarations)
        : name(name), declarations(declarations) {}
    
    std::string toString() const override {
        std::string result = "module " + name + " {\n";
        for (const auto& decl : declarations) {
            result += "  " + decl->toString() + "\n";
        }
        result += "}";
        return result;
    }
    std::string getNodeType() const override { return "ModuleDecl"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Program (top-level)
struct ProgramAST : ASTNode {
    std::vector<std::shared_ptr<DeclAST>> declarations;
    
    ProgramAST(std::vector<std::shared_ptr<DeclAST>> declarations) : declarations(declarations) {}
    
    std::string toString() const override {
        std::string result = "Program:\n";
        for (const auto& decl : declarations) {
            result += decl->toString() + "\n";
        }
        return result;
    }
    std::string getNodeType() const override { return "Program"; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// ==================== UTILITY FUNCTIONS ====================

// Built-in module definitions
struct BuiltinModule {
    std::string name;
    std::unordered_map<std::string, std::shared_ptr<TypeInfo>> functions;
    std::unordered_map<std::string, std::shared_ptr<TypeInfo>> constants;
    
    BuiltinModule(const std::string& name) : name(name) {}
};

// Built-in modules registry
class BuiltinModules {
public:
    static std::unordered_map<std::string, BuiltinModule> modules;
    static void initializeBuiltins();
    static BuiltinModule* getModule(const std::string& name);
};

// Type checking utilities
bool isNumericType(FlastType type);
bool isCompatibleType(const TypeInfo& left, const TypeInfo& right);
std::shared_ptr<TypeInfo> getCommonType(const TypeInfo& left, const TypeInfo& right);

// Built-in method registry
struct BuiltinMethod {
    std::string name;
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;
    std::shared_ptr<TypeInfo> returnType;
    bool isUniversal; // Available on all types (like .type())
    
    BuiltinMethod(const std::string& name, std::vector<std::shared_ptr<TypeInfo>> paramTypes,
                  std::shared_ptr<TypeInfo> returnType, bool isUniversal = false)
        : name(name), paramTypes(paramTypes), returnType(returnType), isUniversal(isUniversal) {}
};

class BuiltinMethods {
public:
    static std::vector<BuiltinMethod> methods;
    static void initializeBuiltinMethods();
    static std::shared_ptr<TypeInfo> getMethodReturnType(const std::string& methodName, 
                                                          const TypeInfo& objectType);
    static bool isBuiltinMethod(const std::string& methodName, const TypeInfo& objectType);
}; 