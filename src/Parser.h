#pragma once
#include "Token.h"
#include "AST.h"
#include "SemanticAnalyzer.h"
#include "ErrorHandler.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>

class ParseError : public std::runtime_error {
public:
    int line;
    int column;
    std::string errorCode;
    
    ParseError(const std::string& message, int line = 0, int column = 0, const std::string& code = "")
        : std::runtime_error(message), line(line), column(column), errorCode(code) {}
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    std::string fileName;
    std::shared_ptr<SemanticAnalyzer> semanticAnalyzer;
    
    // Parser state
    bool inLoop = false;
    bool inFunction = false;
    int genericDepth = 0;
    
    // Helper methods
    Token& peek();
    Token& peekNext();
    Token& previous();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token advance();
    void consume(TokenType type, const std::string& message);
    
    // Error handling
    void error(const std::string& message);
    void synchronize();
    ParseError parseError(const std::string& message);
    
    // Type parsing
    std::shared_ptr<TypeInfo> parseType();
    std::shared_ptr<TypeInfo> parseComplexType();
    std::shared_ptr<TypeInfo> parseGenericType();
    std::vector<std::shared_ptr<TypeInfo>> parseGenericParameters();
    
public:
    Parser(const std::vector<Token>& tokens, const std::string& fileName = "");
    
    // Main parsing entry point
    std::shared_ptr<ProgramAST> parseProgram();
    
    // Declaration parsing (Rust-like)
    std::shared_ptr<DeclAST> parseDeclaration();
    std::shared_ptr<FunctionDeclAST> parseFunctionDecl();  // func keyword
    std::shared_ptr<StructDeclAST> parseStructDecl();     // struct keyword  
    std::shared_ptr<EnumDeclAST> parseEnumDecl();         // enum keyword
    std::shared_ptr<TraitDeclAST> parseTraitDecl();       // trait keyword
    std::shared_ptr<ImplDeclAST> parseImplDecl();         // impl keyword
    std::shared_ptr<ImportDeclAST> parseImportDecl();     // import/use keywords
    std::shared_ptr<ModuleDeclAST> parseModuleDecl();     // mod keyword
    
    // Statement parsing
    std::shared_ptr<StmtAST> parseStatement();
    std::shared_ptr<VarDeclStmtAST> parseVarDecl();
    std::shared_ptr<AssignStmtAST> parseAssignment();
    std::shared_ptr<ExprStmtAST> parseExpressionStatement();
    std::shared_ptr<BlockStmtAST> parseBlock();
    std::shared_ptr<IfStmtAST> parseIfStatement();
    std::shared_ptr<WhileStmtAST> parseWhileStatement();
    std::shared_ptr<StmtAST> parseForStatement();  // Can return ForStmtAST or ForInStmtAST
    std::shared_ptr<ForInStmtAST> parseForInStatement();
    std::shared_ptr<MatchStmtAST> parseMatchStatement();  // Pattern matching
    std::shared_ptr<ReturnStmtAST> parseReturnStatement();
    std::shared_ptr<BreakStmtAST> parseBreakStatement();
    std::shared_ptr<ContinueStmtAST> parseContinueStatement();
    std::shared_ptr<TryCatchStmtAST> parseTryCatchStatement();
    std::shared_ptr<ThrowStmtAST> parseThrowStatement();
    
    // Expression parsing with proper precedence
    std::shared_ptr<ExprAST> parseExpression();
    std::shared_ptr<ExprAST> parseAssignmentExpression();
    std::shared_ptr<ExprAST> parseTernaryExpression();
    std::shared_ptr<ExprAST> parseLogicalOrExpression();
    std::shared_ptr<ExprAST> parseLogicalAndExpression();
    std::shared_ptr<ExprAST> parseBitwiseOrExpression();
    std::shared_ptr<ExprAST> parseBitwiseXorExpression();
    std::shared_ptr<ExprAST> parseBitwiseAndExpression();
    std::shared_ptr<ExprAST> parseEqualityExpression();
    std::shared_ptr<ExprAST> parseComparisonExpression();
    std::shared_ptr<ExprAST> parseShiftExpression();
    std::shared_ptr<ExprAST> parseTermExpression();      // + -
    std::shared_ptr<ExprAST> parseFactorExpression();    // * / %
    std::shared_ptr<ExprAST> parsePowerExpression();     // **
    std::shared_ptr<ExprAST> parseUnaryExpression();
    std::shared_ptr<ExprAST> parseCallExpression();
    std::shared_ptr<ExprAST> parsePrimaryExpression();
    
    // Advanced expression parsing
    std::shared_ptr<ExprAST> parseBinaryExpression(std::shared_ptr<ExprAST> left, Precedence minPrec);
    std::shared_ptr<ExprAST> parsePostfixExpression(std::shared_ptr<ExprAST> expr);
    std::shared_ptr<ExprAST> parseLambdaExpression();
    std::shared_ptr<ExprAST> parseListExpression();
    std::shared_ptr<ExprAST> parseMapExpression();
    std::shared_ptr<ExprAST> parseTupleExpression();
    std::shared_ptr<ExprAST> parseStructInitExpression();
    std::shared_ptr<ExprAST> parseEnumVariantExpression();
    
    // Literal parsing with scientific notation support
    std::shared_ptr<ExprAST> parseNumberLiteral();
    std::shared_ptr<ExprAST> parseScientificLiteral();
    std::shared_ptr<ExprAST> parseStringLiteral();
    std::shared_ptr<ExprAST> parseCharLiteral();
    std::shared_ptr<ExprAST> parseBoolLiteral();
    std::shared_ptr<ExprAST> parseNullLiteral();
    
    // Pattern parsing (for match expressions)
    std::shared_ptr<ExprAST> parsePattern();
    std::shared_ptr<ExprAST> parseStructPattern();
    std::shared_ptr<ExprAST> parseEnumPattern();
    std::shared_ptr<ExprAST> parseWildcardPattern();
    std::shared_ptr<ExprAST> parseGuardPattern();
    
    // Parameter and generic parsing
    std::vector<ParameterAST> parseParameterList();
    ParameterAST parseParameter();
    std::vector<std::shared_ptr<TypeInfo>> parseGenericConstraints();
    
    // Import/module parsing (TypeScript-like)
    std::vector<std::string> parseImportSpecifiers();
    std::string parseModulePath();
    
    // Utility methods
    bool isAssignmentOperator(TokenType type);
    bool isBinaryOperator(TokenType type);
    bool isUnaryOperator(TokenType type);
    bool isTypeKeyword(TokenType type);
    bool isAccessModifier(TokenType type);
    
    // Precedence handling
    Precedence getOperatorPrecedence(TokenType type);
    bool isRightAssociative(TokenType type);
    
    // Built-in method recognition
    bool isBuiltinMethod(const std::string& methodName);
    std::shared_ptr<ExprAST> parseBuiltinMethodCall(std::shared_ptr<ExprAST> object, const std::string& methodName);
    
    // Error recovery
    void skipToNextStatement();
    void skipToMatchingBrace();
    void skipToMatchingParen();
    
    // Validation helpers
    bool validateGenericConstraints(const std::vector<std::shared_ptr<TypeInfo>>& constraints);
    bool validateParameterList(const std::vector<ParameterAST>& parameters);
    bool validateReturnType(std::shared_ptr<TypeInfo> returnType);
    
    // Debug and analysis integration
    void attachSemanticAnalyzer(std::shared_ptr<SemanticAnalyzer> analyzer);
    bool hasErrors() const { return !errors.empty(); }
    std::vector<ParseError> getErrors() const { return errors; }
    void printErrors() const;
    
private:
    std::vector<ParseError> errors;
    
    // Helper methods for complex parsing
    std::shared_ptr<ExprAST> parseComplexAssignment(std::shared_ptr<ExprAST> target);
    std::shared_ptr<ExprAST> parseMethodChain(std::shared_ptr<ExprAST> object);
    std::shared_ptr<ExprAST> parseGenericCall(const std::string& functionName);
    
    // Type system integration
    std::shared_ptr<TypeInfo> inferExpressionType(std::shared_ptr<ExprAST> expr);
    bool isValidTypeConversion(const TypeInfo& from, const TypeInfo& to);
    
    // Advanced parsing features
    std::shared_ptr<ExprAST> parseAsyncExpression();
    std::shared_ptr<ExprAST> parseAwaitExpression();
    std::shared_ptr<ExprAST> parseUnsafeExpression();
    std::shared_ptr<ExprAST> parseBoxExpression();
    std::shared_ptr<ExprAST> parseRefExpression();
    std::shared_ptr<ExprAST> parseDerefExpression();
    
    // Context tracking
    void enterFunction(std::shared_ptr<TypeInfo> returnType);
    void exitFunction();
    void enterLoop();
    void exitLoop();
    void enterGeneric();
    void exitGeneric();
    
    std::shared_ptr<TypeInfo> currentFunctionReturnType;
    std::vector<bool> loopStack;
    std::vector<std::shared_ptr<TypeInfo>> genericStack;
}; 