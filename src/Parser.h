#pragma once
#include "Token.h"
#include "AST.h"
#include <vector>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    Token& peek(int offset = 0);
    Token& advance();
    bool match(TokenType type);
    bool check(TokenType type);
    void consume(TokenType type, const std::string& message);
    
    // Parsing methods
    std::unique_ptr<ProgramAST> parseProgram();
    std::unique_ptr<ImportAST> parseImport();
    std::unique_ptr<ClassAST> parseClass();
    std::unique_ptr<FunctionAST> parseFunction(bool isClassMethod = false);
    std::unique_ptr<VarDeclStmtAST> parseVarDecl();
    
    StmtPtr parseStatement();
    StmtPtr parseAssignment();
    StmtPtr parseExpressionStatement();
    StmtPtr parseReturnStatement();
    
    ExprPtr parseExpression();
    ExprPtr parseLogicalOr();
    ExprPtr parseLogicalAnd();
    ExprPtr parseEquality();
    ExprPtr parseComparison();
    ExprPtr parseTerm();
    ExprPtr parseFactor();
    ExprPtr parseUnary();
    ExprPtr parseCall();
    ExprPtr parsePrimary();
    
    std::vector<ExprPtr> parseArguments();
    std::vector<std::pair<std::string, std::string>> parseParameters();
    
public:
    Parser(std::vector<Token> tokens);
    std::unique_ptr<ProgramAST> parse();
}; 