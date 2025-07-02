#include "Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), current(0) {}

Token& Parser::peek(int offset) {
    size_t index = current + offset;
    if (index >= tokens.size()) {
        return tokens.back(); // Return EOF token
    }
    return tokens[index];
}

Token& Parser::advance() {
    if (current < tokens.size() - 1) {
        current++;
    }
    return tokens[current];
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) {
    return peek().type == type;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    
    throw std::runtime_error(message + " at line " + std::to_string(peek().line) + 
                           ", got " + tokenTypeToString(peek().type));
}

std::unique_ptr<ProgramAST> Parser::parse() {
    return parseProgram();
}

std::unique_ptr<ProgramAST> Parser::parseProgram() {
    auto program = std::make_unique<ProgramAST>();
    
    while (!check(TokenType::TOK_EOF)) {
        if (check(TokenType::TOK_IMPORT)) {
            program->imports.push_back(parseImport());
        } else if (check(TokenType::TOK_CLASS)) {
            program->classes.push_back(parseClass());
        } else if (check(TokenType::TOK_FUNC) || 
                  (check(TokenType::TOK_PUB) && peek(1).type == TokenType::TOK_FUNC)) {
            program->functions.push_back(parseFunction());
        } else {
            throw std::runtime_error("Unexpected token at top level: " + 
                                   tokenTypeToString(peek().type));
        }
    }
    
    return program;
}

std::unique_ptr<ImportAST> Parser::parseImport() {
    consume(TokenType::TOK_IMPORT, "Expected 'import'");
    
    std::string alias = peek().value;
    advance(); // consume identifier
    
    consume(TokenType::TOK_FROM, "Expected 'from'");
    
    std::string module = peek().value;
    if (peek().type != TokenType::TOK_STRING) {
        throw std::runtime_error("Expected module name string");
    }
    advance();
    
    consume(TokenType::TOK_SEMICOLON, "Expected ';'");
    
    return std::make_unique<ImportAST>(module, alias);
}

std::unique_ptr<ClassAST> Parser::parseClass() {
    consume(TokenType::TOK_CLASS, "Expected 'class'");
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw std::runtime_error("Expected class name");
    }
    
    std::string className = peek().value;
    advance();
    
    consume(TokenType::TOK_LBRACE, "Expected '{'");
    
    auto classNode = std::make_unique<ClassAST>(className);
    
    while (!check(TokenType::TOK_RBRACE) && !check(TokenType::TOK_EOF)) {
        if (check(TokenType::TOK_FUNC) || 
           (check(TokenType::TOK_PUB) && peek(1).type == TokenType::TOK_FUNC)) {
            classNode->methods.push_back(parseFunction(true));
        } else if (check(TokenType::TOK_LET) || 
                  (check(TokenType::TOK_PUB) && peek(1).type == TokenType::TOK_LET)) {
            classNode->members.push_back(parseVarDecl());
        } else {
            throw std::runtime_error("Expected method or member declaration in class");
        }
    }
    
    consume(TokenType::TOK_RBRACE, "Expected '}'");
    
    return classNode;
}

std::unique_ptr<FunctionAST> Parser::parseFunction(bool isClassMethod) {
    bool isPublic = false;
    if (match(TokenType::TOK_PUB)) {
        isPublic = true;
    }
    
    consume(TokenType::TOK_FUNC, "Expected 'func'");
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw std::runtime_error("Expected function name");
    }
    
    std::string funcName = peek().value;
    advance();
    
    consume(TokenType::TOK_LPAREN, "Expected '('");
    
    auto params = parseParameters();
    
    consume(TokenType::TOK_RPAREN, "Expected ')'");
    consume(TokenType::TOK_ARROW, "Expected '->'");
    
    if (!check(TokenType::TOK_IDENTIFIER) && !check(TokenType::TOK_INT)) {
        throw std::runtime_error("Expected return type");
    }
    
    std::string returnType = peek().value;
    advance();
    
    consume(TokenType::TOK_LBRACE, "Expected '{'");
    
    std::vector<StmtPtr> body;
    while (!check(TokenType::TOK_RBRACE) && !check(TokenType::TOK_EOF)) {
        body.push_back(parseStatement());
    }
    
    consume(TokenType::TOK_RBRACE, "Expected '}'");
    
    return std::make_unique<FunctionAST>(funcName, std::move(params), returnType, 
                                       std::move(body), isPublic);
}

std::unique_ptr<VarDeclStmtAST> Parser::parseVarDecl() {
    bool isPublic = false;
    if (match(TokenType::TOK_PUB)) {
        isPublic = true;
    }
    
    consume(TokenType::TOK_LET, "Expected 'let'");
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw std::runtime_error("Expected variable name");
    }
    
    std::string varName = peek().value;
    advance();
    
    consume(TokenType::TOK_COLON, "Expected ':'");
    
    if (!check(TokenType::TOK_IDENTIFIER) && !check(TokenType::TOK_INT)) {
        throw std::runtime_error("Expected type name");
    }
    
    std::string typeName = peek().value;
    advance();
    
    ExprPtr initializer = nullptr;
    if (match(TokenType::TOK_ASSIGN)) {
        initializer = parseExpression();
    }
    
    consume(TokenType::TOK_SEMICOLON, "Expected ';'");
    
    return std::make_unique<VarDeclStmtAST>(varName, typeName, std::move(initializer), isPublic);
}

StmtPtr Parser::parseStatement() {
    if (check(TokenType::TOK_LET) || 
       (check(TokenType::TOK_PUB) && peek(1).type == TokenType::TOK_LET)) {
        return parseVarDecl();
    }
    
    if (check(TokenType::TOK_RETURN)) {
        return parseReturnStatement();
    }
    
    // Try assignment
    size_t savedPos = current;
    try {
        auto stmt = parseAssignment();
        if (stmt) return stmt;
    } catch (...) {
        current = savedPos; // Backtrack
    }
    
    return parseExpressionStatement();
}

StmtPtr Parser::parseAssignment() {
    // Save position to backtrack if this isn't assignment
    size_t savedPos = current;
    
    try {
        auto expr = parseExpression();
        
        if (check(TokenType::TOK_ASSIGN)) {
            advance(); // consume assignment token
            auto value = parseExpression();
            consume(TokenType::TOK_SEMICOLON, "Expected ';'");
            return std::make_unique<AssignStmtAST>(std::move(expr), std::move(value));
        }
        
        // Restore position and return nullptr
        current = savedPos;
        return nullptr;
    } catch (...) {
        // Restore position and return nullptr
        current = savedPos;
        return nullptr;
    }
}

StmtPtr Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    consume(TokenType::TOK_SEMICOLON, "Expected ';'");
    return std::make_unique<ExprStmtAST>(std::move(expr));
}

StmtPtr Parser::parseReturnStatement() {
    consume(TokenType::TOK_RETURN, "Expected 'return'");
    
    ExprPtr expr = nullptr;
    if (!check(TokenType::TOK_SEMICOLON)) {
        expr = parseExpression();
    }
    
    consume(TokenType::TOK_SEMICOLON, "Expected ';'");
    return std::make_unique<ReturnStmtAST>(std::move(expr));
}

ExprPtr Parser::parseExpression() {
    return parseLogicalOr();
}

ExprPtr Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (match(TokenType::TOK_IDENTIFIER) && tokens[current-1].value == "||") {
        std::string op = tokens[current-1].value;
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryExprAST>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

ExprPtr Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    
    while (match(TokenType::TOK_IDENTIFIER) && tokens[current-1].value == "&&") {
        std::string op = tokens[current-1].value;
        auto right = parseEquality();
        expr = std::make_unique<BinaryExprAST>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

ExprPtr Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (match(TokenType::TOK_EQUAL) || match(TokenType::TOK_NOT_EQUAL)) {
        std::string op = tokens[current-1].value;
        auto right = parseComparison();
        expr = std::make_unique<BinaryExprAST>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

ExprPtr Parser::parseComparison() {
    auto expr = parseTerm();
    
    while (match(TokenType::TOK_GREATER) || match(TokenType::TOK_LESS)) {
        std::string op = tokens[current-1].value;
        auto right = parseTerm();
        expr = std::make_unique<BinaryExprAST>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

ExprPtr Parser::parseTerm() {
    auto expr = parseFactor();
    
    while (match(TokenType::TOK_PLUS) || match(TokenType::TOK_MINUS)) {
        std::string op = tokens[current-1].value;
        auto right = parseFactor();
        expr = std::make_unique<BinaryExprAST>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

ExprPtr Parser::parseFactor() {
    auto expr = parseUnary();
    
    while (match(TokenType::TOK_MULTIPLY) || match(TokenType::TOK_DIVIDE)) {
        std::string op = tokens[current-1].value;
        auto right = parseUnary();
        expr = std::make_unique<BinaryExprAST>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

ExprPtr Parser::parseUnary() {
    if (match(TokenType::TOK_MINUS)) {
        auto expr = parseUnary();
        return std::make_unique<BinaryExprAST>("-", 
            std::make_unique<NumberExprAST>(0), std::move(expr));
    }
    
    return parseCall();
}

ExprPtr Parser::parseCall() {
    auto expr = parsePrimary();
    
    while (true) {
        if (match(TokenType::TOK_LPAREN)) {
            // Function call
            auto args = parseArguments();
            consume(TokenType::TOK_RPAREN, "Expected ')'");
            
            if (auto var = dynamic_cast<VariableExprAST*>(expr.get())) {
                std::string funcName = var->name;
                expr.release(); // Release ownership
                expr = std::make_unique<CallExprAST>(funcName, std::move(args));
            } else {
                throw std::runtime_error("Invalid function call");
            }
        } else if (match(TokenType::TOK_DOT)) {
            // Member access
            if (!check(TokenType::TOK_IDENTIFIER)) {
                throw std::runtime_error("Expected member name after '.'");
            }
            
            std::string member = peek().value;
            advance();
            
            expr = std::make_unique<MemberAccessExprAST>(std::move(expr), member);
        } else {
            break;
        }
    }
    
    return expr;
}

ExprPtr Parser::parsePrimary() {
    if (match(TokenType::TOK_NUMBER)) {
        double value = std::stod(tokens[current-1].value);
        return std::make_unique<NumberExprAST>(value);
    }
    
    if (match(TokenType::TOK_STRING)) {
        return std::make_unique<StringExprAST>(tokens[current-1].value);
    }
    
    if (match(TokenType::TOK_NEW)) {
        if (!check(TokenType::TOK_IDENTIFIER)) {
            throw std::runtime_error("Expected class name after 'new'");
        }
        
        std::string className = peek().value;
        advance();
        
        consume(TokenType::TOK_LPAREN, "Expected '('");
        auto args = parseArguments();
        consume(TokenType::TOK_RPAREN, "Expected ')'");
        
        return std::make_unique<NewExprAST>(className, std::move(args));
    }
    
    if (match(TokenType::TOK_IDENTIFIER) || match(TokenType::TOK_PRINTLN) || 
        match(TokenType::TOK_EXIT_SUCCESS) || match(TokenType::TOK_SELF)) {
        return std::make_unique<VariableExprAST>(tokens[current-1].value);
    }
    
    if (match(TokenType::TOK_LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::TOK_RPAREN, "Expected ')'");
        return expr;
    }
    
    throw std::runtime_error("Unexpected token: " + tokenTypeToString(peek().type));
}

std::vector<ExprPtr> Parser::parseArguments() {
    std::vector<ExprPtr> args;
    
    if (!check(TokenType::TOK_RPAREN)) {
        do {
            args.push_back(parseExpression());
        } while (match(TokenType::TOK_COMMA));
    }
    
    return args;
}

std::vector<std::pair<std::string, std::string>> Parser::parseParameters() {
    std::vector<std::pair<std::string, std::string>> params;
    
    if (!check(TokenType::TOK_RPAREN)) {
        do {
            if (!check(TokenType::TOK_IDENTIFIER)) {
                throw std::runtime_error("Expected parameter name");
            }
            
            std::string paramName = peek().value;
            advance();
            
            consume(TokenType::TOK_COLON, "Expected ':'");
            
            if (!check(TokenType::TOK_IDENTIFIER) && !check(TokenType::TOK_INT)) {
                throw std::runtime_error("Expected parameter type");
            }
            
            std::string paramType = peek().value;
            advance();
            
            params.push_back({paramName, paramType});
        } while (match(TokenType::TOK_COMMA));
    }
    
    return params;
} 