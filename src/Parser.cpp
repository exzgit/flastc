#include "Parser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Parser::Parser(const std::vector<Token>& tokens, const std::string& fileName) 
    : tokens(tokens), current(0), fileName(fileName) {}

// ==================== UTILITY METHODS ====================

Token& Parser::peek() {
    if (isAtEnd()) {
        static Token eof(TokenType::TOK_EOF, "", 0, 0);
        return eof;
    }
    return tokens[current];
}


Token& Parser::peekNext() {
    if (current + 1 >= tokens.size()) {
        static Token eof(TokenType::TOK_EOF, "", 0, 0);
        return eof;
    }
    return tokens[current + 1];
}

Token& Parser::previous() {
    if (current == 0) {
        static Token eof(TokenType::TOK_EOF, "", 0, 0);
        return eof;
    }
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return current >= tokens.size() || (current < tokens.size() && tokens[current].type == TokenType::TOK_EOF);
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    throw parseError(message);
}

void Parser::error(const std::string& message) {
    Token& current_token = peek();
    // Get line content from the original source
    std::string lineContent = "";
    if (!tokens.empty() && current < tokens.size()) {
        // Try to find the line content from the token
        for (const auto& token : tokens) {
            if (token.line == current_token.line) {
                // This is a simplified approach - in a real implementation,
                // you'd want to store the original source lines
                lineContent = "// Line " + std::to_string(token.line);
                break;
            }
        }
    }
    ErrorContext context(fileName, current_token.line, current_token.column, lineContent, current_token.value);
    REPORT_ERROR(ErrorCode::INVALID_EXPRESSION, message, context);
}

ParseError Parser::parseError(const std::string& message) {
    Token& current_token = peek();
    // Get line content from the original source
    std::string lineContent = "";
    if (!tokens.empty() && current < tokens.size()) {
        // Try to find the line content from the token
        for (const auto& token : tokens) {
            if (token.line == current_token.line) {
                // This is a simplified approach - in a real implementation,
                // you'd want to store the original source lines
                lineContent = "// Line " + std::to_string(token.line);
                break;
            }
        }
    }
    ErrorContext context(fileName, current_token.line, current_token.column, lineContent, current_token.value);
    REPORT_ERROR(ErrorCode::INVALID_EXPRESSION, message, context);
    return ParseError(message, current_token.line, current_token.column);
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::TOK_SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::TOK_FUNC:
            case TokenType::TOK_STRUCT:
            case TokenType::TOK_ENUM:
            case TokenType::TOK_TRAIT:
            case TokenType::TOK_IMPL:

            case TokenType::TOK_LET:
            case TokenType::TOK_IF:
            case TokenType::TOK_WHILE:
            case TokenType::TOK_FOR:
            case TokenType::TOK_RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

// ==================== MAIN PARSING ====================

std::shared_ptr<ProgramAST> Parser::parseProgram() {
    std::vector<std::shared_ptr<DeclAST>> declarations;
    
    while (!isAtEnd()) {
        try {
            if (auto decl = parseDeclaration()) {
                declarations.push_back(decl);
            }
        } catch (const ParseError& e) {
            synchronize();
        }
    }
    
    return std::make_shared<ProgramAST>(declarations);
}

std::shared_ptr<DeclAST> Parser::parseDeclaration() {
    try {
        // Access modifiers
        bool isPublic = match({TokenType::TOK_PUB});
        bool isStatic = match({TokenType::TOK_STATIC});
        bool isConst = match({TokenType::TOK_CONST});
        bool isUnsafe = match({TokenType::TOK_UNSAFE});
        bool isAsync = match({TokenType::TOK_ASYNC});
        
        // External declarations
        if (match({TokenType::TOK_EXTERN})) {
            std::string linkage = "C";
            if (check(TokenType::TOK_STRING)) {
                linkage = advance().value;
            }
            
            if (check(TokenType::TOK_FUNC)) {
                auto func = parseFunctionDecl();
                func->isExtern = true;
                func->externLang = linkage;
                func->isPublic = isPublic;
                func->isStatic = isStatic;
                func->isAsync = isAsync;
                return func;
            }
        }
        
        // Declaration types
        if (check(TokenType::TOK_FUNC)) {
            auto func = parseFunctionDecl();
            func->isPublic = isPublic;
            func->isStatic = isStatic;
            func->isAsync = isAsync;
            return func;
        }
        
        if (check(TokenType::TOK_STRUCT)) return parseStructDecl();
        if (check(TokenType::TOK_ENUM)) return parseEnumDecl();
        if (check(TokenType::TOK_TRAIT)) return parseTraitDecl();
        if (check(TokenType::TOK_IMPL)) return parseImplDecl();

        if (check(TokenType::TOK_IMPORT) || check(TokenType::TOK_USE)) return parseImportDecl();
        if (check(TokenType::TOK_MOD)) return parseModuleDecl();
        
        // Variable declarations at global level not supported for now
        // Use local variables inside functions instead
        
        throw parseError("Expected declaration");
        
    } catch (const ParseError& e) {
        synchronize();
        return nullptr;
    }
}

// ==================== FUNCTION DECLARATION (Rust-like) ====================

std::shared_ptr<FunctionDeclAST> Parser::parseFunctionDecl() {
    consume(TokenType::TOK_FUNC, "Expected 'fn'");
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw parseError("Expected function name");
    }
    
    std::string name = advance().value;
    
    // Generic parameters
    std::vector<std::shared_ptr<TypeInfo>> generics;
    if (match({TokenType::TOK_LESS})) {
        generics = parseGenericParameters();
        consume(TokenType::TOK_GREATER, "Expected '>' after generic parameters");
    }
    
    // Parameters
    consume(TokenType::TOK_LPAREN, "Expected '(' after function name");
    std::vector<ParameterAST> parameters = parseParameterList();
    consume(TokenType::TOK_RPAREN, "Expected ')' after parameters");
    
    // Return type
    std::shared_ptr<TypeInfo> returnType;
    if (match({TokenType::TOK_ARROW})) {
        returnType = parseType();
    } else {
        returnType = std::make_shared<TypeInfo>(FlastType::VOID);
    }
    
    // Function body
    std::shared_ptr<BlockStmtAST> body = nullptr;
    if (check(TokenType::TOK_LBRACE)) {
        enterFunction(returnType);
        body = parseBlock();
        exitFunction();
    } else {
        consume(TokenType::TOK_SEMICOLON, "Expected ';' or function body");
    }
    
    return std::make_shared<FunctionDeclAST>(name, parameters, returnType, body);
}

// ==================== STRUCT DECLARATION ====================

std::shared_ptr<StructDeclAST> Parser::parseStructDecl() {
    consume(TokenType::TOK_STRUCT, "Expected 'struct'");
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw parseError("Expected struct name");
    }
    
    std::string name = advance().value;
    
    // Generic parameters
    std::vector<std::shared_ptr<TypeInfo>> generics;
    if (match({TokenType::TOK_LESS})) {
        generics = parseGenericParameters();
        consume(TokenType::TOK_GREATER, "Expected '>' after generic parameters");
    }
    
    consume(TokenType::TOK_LBRACE, "Expected '{' after struct name");
    
    // Parse fields
    std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>> fields;
    
    while (!check(TokenType::TOK_RBRACE) && !isAtEnd()) {
        // Field visibility
        bool isPublic = match({TokenType::TOK_PUB});
        
        if (!check(TokenType::TOK_IDENTIFIER)) {
            throw parseError("Expected field name");
        }
        
        std::string fieldName = advance().value;
        consume(TokenType::TOK_COLON, "Expected ':' after field name");
        
        auto fieldType = parseType();
        fields.emplace_back(fieldName, fieldType);
        
        if (!match({TokenType::TOK_COMMA})) {
            break;
        }
    }
    
    consume(TokenType::TOK_RBRACE, "Expected '}' after struct fields");
    
    return std::make_shared<StructDeclAST>(name, fields, generics);
}

// ==================== ENUM DECLARATION ====================

std::shared_ptr<EnumDeclAST> Parser::parseEnumDecl() {
    consume(TokenType::TOK_ENUM, "Expected 'enum'");
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw parseError("Expected enum name");
    }
    
    std::string name = advance().value;
    
    // Generic parameters
    std::vector<std::shared_ptr<TypeInfo>> generics;
    if (match({TokenType::TOK_LESS})) {
        generics = parseGenericParameters();
        consume(TokenType::TOK_GREATER, "Expected '>' after generic parameters");
    }
    
    consume(TokenType::TOK_LBRACE, "Expected '{' after enum name");
    
    // Parse variants
    std::vector<std::pair<std::string, std::vector<std::shared_ptr<TypeInfo>>>> variants;
    
    while (!check(TokenType::TOK_RBRACE) && !isAtEnd()) {
        if (!check(TokenType::TOK_IDENTIFIER)) {
            throw parseError("Expected variant name");
        }
        
        std::string variantName = advance().value;
        std::vector<std::shared_ptr<TypeInfo>> variantTypes;
        
        // Tuple-like variant
        if (match({TokenType::TOK_LPAREN})) {
            while (!check(TokenType::TOK_RPAREN) && !isAtEnd()) {
                variantTypes.push_back(parseType());
                if (!match({TokenType::TOK_COMMA})) {
                    break;
                }
            }
            consume(TokenType::TOK_RPAREN, "Expected ')' after variant types");
        }
        
        variants.emplace_back(variantName, variantTypes);
        
        if (!match({TokenType::TOK_COMMA})) {
            break;
        }
    }
    
    consume(TokenType::TOK_RBRACE, "Expected '}' after enum variants");
    
    return std::make_shared<EnumDeclAST>(name, variants, generics);
}

// ==================== EXPRESSION PARSING WITH PRECEDENCE ====================

std::shared_ptr<ExprAST> Parser::parseExpression() {
    return parseAssignmentExpression();
}

std::shared_ptr<ExprAST> Parser::parseAssignmentExpression() {
    auto expr = parseTernaryExpression();
    
    if (isAssignmentOperator(peek().type)) {
        std::string op = advance().value;
        auto right = parseAssignmentExpression();
        return std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseTernaryExpression() {
    auto expr = parseLogicalOrExpression();
    
    if (match({TokenType::TOK_QUESTION})) {
        auto thenExpr = parseExpression();
        consume(TokenType::TOK_COLON, "Expected ':' in ternary expression");
        auto elseExpr = parseExpression();
        
        // Create a ternary expression (represented as special binary op)
        auto condition = std::make_shared<BinaryExprAST>("?:", expr, thenExpr);
        return std::make_shared<BinaryExprAST>("?:", condition, elseExpr);
    }
    
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseLogicalOrExpression() {
    auto expr = parseLogicalAndExpression();
    
    while (match({TokenType::TOK_LOGICAL_OR, TokenType::TOK_OR})) {
        std::string op = previous().value;
        auto right = parseLogicalAndExpression();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseLogicalAndExpression() {
    auto expr = parseBitwiseOrExpression();
    
    while (match({TokenType::TOK_LOGICAL_AND, TokenType::TOK_AND})) {
        std::string op = previous().value;
        auto right = parseBitwiseOrExpression();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseBitwiseOrExpression() {
    auto expr = parseBitwiseXorExpression();
    
    while (match({TokenType::TOK_BIT_OR})) {
        std::string op = previous().value;
        auto right = parseBitwiseXorExpression();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseBitwiseXorExpression() {
    auto expr = parseBitwiseAndExpression();
    
    while (match({TokenType::TOK_BIT_XOR, TokenType::TOK_XOR})) {
        std::string op = previous().value;
        auto right = parseBitwiseAndExpression();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseBitwiseAndExpression() {
    auto expr = parseEqualityExpression();
    
    while (match({TokenType::TOK_BIT_AND})) {
        std::string op = previous().value;
        auto right = parseEqualityExpression();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

// EQUALITY has SUPER LOW precedence as requested
std::shared_ptr<ExprAST> Parser::parseEqualityExpression() {
    auto expr = parseComparisonExpression();
    
    while (match({TokenType::TOK_EQUAL, TokenType::TOK_NOT_EQUAL, 
                  TokenType::TOK_STRICT_EQUAL, TokenType::TOK_STRICT_NOT_EQUAL})) {
        std::string op = previous().value;
        auto right = parseComparisonExpression();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

// COMPARISON has precedence above equality
std::shared_ptr<ExprAST> Parser::parseComparisonExpression() {
    auto expr = parseShiftExpression();
    
    while (match({TokenType::TOK_LESS, TokenType::TOK_GREATER, 
                  TokenType::TOK_LESS_EQUAL, TokenType::TOK_GREATER_EQUAL,
                  TokenType::TOK_SPACESHIP})) {
        std::string op = previous().value;
        auto right = parseShiftExpression();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseShiftExpression() {
    auto expr = parseTermExpression();
    
    while (match({TokenType::TOK_LEFT_SHIFT, TokenType::TOK_RIGHT_SHIFT, 
                  TokenType::TOK_UNSIGNED_RIGHT_SHIFT})) {
        std::string op = previous().value;
        auto right = parseTermExpression();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

// TERM: + and - have LOW precedence
std::shared_ptr<ExprAST> Parser::parseTermExpression() {
    auto expr = parseFactorExpression();
    
    while (match({TokenType::TOK_PLUS, TokenType::TOK_MINUS})) {
        std::string op = previous().value;
        auto right = parseFactorExpression();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

// FACTOR: *, /, % have HIGH precedence  
std::shared_ptr<ExprAST> Parser::parseFactorExpression() {
    auto expr = parsePowerExpression();
    
    while (match({TokenType::TOK_MULTIPLY, TokenType::TOK_DIVIDE, TokenType::TOK_MODULO})) {
        std::string op = previous().value;
        auto right = parsePowerExpression();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

// POWER: ** has HIGHEST precedence and is right-associative
std::shared_ptr<ExprAST> Parser::parsePowerExpression() {
    auto expr = parseUnaryExpression();
    
    if (match({TokenType::TOK_POWER})) {
        std::string op = previous().value;
        auto right = parsePowerExpression(); // Right associative
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseUnaryExpression() {
    if (match({TokenType::TOK_LOGICAL_NOT, TokenType::TOK_NOT, TokenType::TOK_MINUS, 
               TokenType::TOK_PLUS, TokenType::TOK_BIT_NOT, TokenType::TOK_INCREMENT, 
               TokenType::TOK_DECREMENT, TokenType::TOK_ADDRESS_OF, TokenType::TOK_DEREFERENCE})) {
        std::string op = previous().value;
        auto expr = parseUnaryExpression();
        return std::make_shared<UnaryExprAST>(op, expr, true);
    }
    
    return parseCallExpression();
}

std::shared_ptr<ExprAST> Parser::parseCallExpression() {
    auto expr = parsePrimaryExpression();
    
    while (true) {
        if (match({TokenType::TOK_LPAREN})) {
            // Function call
            std::vector<std::shared_ptr<ExprAST>> args;
            
            if (!check(TokenType::TOK_RPAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match({TokenType::TOK_COMMA}));
            }
            
            consume(TokenType::TOK_RPAREN, "Expected ')' after arguments");
            
            if (auto varExpr = std::dynamic_pointer_cast<VariableExprAST>(expr)) {
                // Regular function call
                expr = std::make_shared<CallExprAST>(varExpr->name, args);
            } else if (auto memberExpr = std::dynamic_pointer_cast<MemberAccessExprAST>(expr)) {
                // Method call: object.method()
                expr = std::make_shared<MethodCallExprAST>(memberExpr->object, memberExpr->member, args);
            } else {
                // Other complex expression calls not yet implemented
                throw parseError("Complex function calls not yet implemented");
            }
            
        } else if (match({TokenType::TOK_DOT})) {
            // Member access
            if (!check(TokenType::TOK_IDENTIFIER)) {
                throw parseError("Expected property name after '.'");
            }
            
            std::string member = advance().value;
            expr = std::make_shared<MemberAccessExprAST>(expr, member);
            
        } else if (match({TokenType::TOK_LBRACKET})) {
            // Array/index access
            auto index = parseExpression();
            consume(TokenType::TOK_RBRACKET, "Expected ']' after index");
            expr = std::make_shared<IndexExprAST>(expr, index);
            
        } else {
            break;
        }
    }
    
    return expr;
}

std::shared_ptr<ExprAST> Parser::parsePrimaryExpression() {
    // Numbers (including scientific notation)
    if (check(TokenType::TOK_NUMBER)) {
        return parseNumberLiteral();
    }
    
    if (check(TokenType::TOK_SCIENTIFIC)) {
        return parseScientificLiteral();
    }
    
    // Strings
    if (check(TokenType::TOK_STRING)) {
        return parseStringLiteral();
    }
    
    // Booleans  
    if (check(TokenType::TOK_TRUE) || check(TokenType::TOK_FALSE)) {
        return parseBoolLiteral();
    }
    
    // Null values
    if (check(TokenType::TOK_NULL_VALUE) || check(TokenType::TOK_NONE)) {
        return parseNullLiteral();
    }
    
    // Built-in functions
    if (check(TokenType::TOK_PRINTLN)) {
        std::string name = advance().value;
        return std::make_shared<VariableExprAST>(name);
    }
    
    // Self keyword
    if (check(TokenType::TOK_SELF)) {
        std::string name = advance().value;
        return std::make_shared<VariableExprAST>(name);
    }
    
    // New expressions (object instantiation)
    if (check(TokenType::TOK_NEW)) {
        advance(); // consume 'new'
        
        if (!check(TokenType::TOK_IDENTIFIER)) {
            throw parseError("Expected class name after 'new'");
        }
        
        std::string className = advance().value;
        
        // Parse constructor arguments
        std::vector<std::shared_ptr<ExprAST>> args;
        if (match({TokenType::TOK_LPAREN})) {
            if (!check(TokenType::TOK_RPAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match({TokenType::TOK_COMMA}));
            }
            consume(TokenType::TOK_RPAREN, "Expected ')' after constructor arguments");
        }
        
        return std::make_shared<NewExprAST>(className, args);
    }
    
    // Identifiers
    if (check(TokenType::TOK_IDENTIFIER)) {
        std::string name = advance().value;
        return std::make_shared<VariableExprAST>(name);
    }
    
    // Parenthesized expressions
    if (match({TokenType::TOK_LPAREN})) {
        auto expr = parseExpression();
        consume(TokenType::TOK_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    // List literals
    if (check(TokenType::TOK_LBRACKET)) {
        return parseListExpression();
    }
    
    throw parseError("Expected expression");
}

// ==================== LITERAL PARSING ====================

std::shared_ptr<ExprAST> Parser::parseNumberLiteral() {
    Token token = advance();
    double value = std::stod(token.value);
    return std::make_shared<NumberExprAST>(value, false, token.value);
}

std::shared_ptr<ExprAST> Parser::parseScientificLiteral() {
    Token token = advance();
    double value = std::stod(token.value);
    return std::make_shared<ScientificExprAST>(value, token.value);
}

std::shared_ptr<ExprAST> Parser::parseStringLiteral() {
    Token token = advance();
    return std::make_shared<StringExprAST>(token.value);
}

std::shared_ptr<ExprAST> Parser::parseBoolLiteral() {
    Token token = advance();
    bool value = (token.type == TokenType::TOK_TRUE);
    return std::make_shared<BoolExprAST>(value);
}

std::shared_ptr<ExprAST> Parser::parseNullLiteral() {
    advance(); // consume null/none token
    return std::make_shared<NullExprAST>();
}

std::shared_ptr<ExprAST> Parser::parseListExpression() {
    consume(TokenType::TOK_LBRACKET, "Expected '['");
    
    std::vector<std::shared_ptr<ExprAST>> elements;
    
    if (!check(TokenType::TOK_RBRACKET)) {
        do {
            elements.push_back(parseExpression());
        } while (match({TokenType::TOK_COMMA}));
    }
    
    consume(TokenType::TOK_RBRACKET, "Expected ']' after list elements");
    
    return std::make_shared<ListExprAST>(elements);
}

// ==================== UTILITY METHODS ====================

bool Parser::isAssignmentOperator(TokenType type) {
    return type == TokenType::TOK_ASSIGN ||
           type == TokenType::TOK_PLUS_ASSIGN ||
           type == TokenType::TOK_MINUS_ASSIGN ||
           type == TokenType::TOK_MULT_ASSIGN ||
           type == TokenType::TOK_DIV_ASSIGN ||
           type == TokenType::TOK_MOD_ASSIGN ||
           type == TokenType::TOK_POWER_ASSIGN ||
           type == TokenType::TOK_BIT_AND_ASSIGN ||
           type == TokenType::TOK_BIT_OR_ASSIGN ||
           type == TokenType::TOK_BIT_XOR_ASSIGN ||
           type == TokenType::TOK_LEFT_SHIFT_ASSIGN ||
           type == TokenType::TOK_RIGHT_SHIFT_ASSIGN;
}

std::shared_ptr<TypeInfo> Parser::parseType() {
    // Parse primitive types
    if (match({TokenType::TOK_INT8})) return std::make_shared<TypeInfo>(FlastType::I8);
    if (match({TokenType::TOK_INT16})) return std::make_shared<TypeInfo>(FlastType::I16);
    if (match({TokenType::TOK_INT32})) return std::make_shared<TypeInfo>(FlastType::I32);
    if (match({TokenType::TOK_INT64})) return std::make_shared<TypeInfo>(FlastType::I64);
    if (match({TokenType::TOK_INT128})) return std::make_shared<TypeInfo>(FlastType::I128);
    if (match({TokenType::TOK_UINT8})) return std::make_shared<TypeInfo>(FlastType::U8);
    if (match({TokenType::TOK_UINT16})) return std::make_shared<TypeInfo>(FlastType::U16);
    if (match({TokenType::TOK_UINT32})) return std::make_shared<TypeInfo>(FlastType::U32);
    if (match({TokenType::TOK_UINT64})) return std::make_shared<TypeInfo>(FlastType::U64);
    if (match({TokenType::TOK_UINT128})) return std::make_shared<TypeInfo>(FlastType::U128);
    if (match({TokenType::TOK_FLOAT32})) return std::make_shared<TypeInfo>(FlastType::F32);
    if (match({TokenType::TOK_FLOAT64})) return std::make_shared<TypeInfo>(FlastType::F64);
    if (match({TokenType::TOK_BOOL_TYPE})) return std::make_shared<TypeInfo>(FlastType::BOOL);
    if (match({TokenType::TOK_STRING_TYPE})) return std::make_shared<TypeInfo>(FlastType::STRING);
    if (match({TokenType::TOK_CHAR_TYPE})) return std::make_shared<TypeInfo>(FlastType::CHAR);
    if (match({TokenType::TOK_VOID})) return std::make_shared<TypeInfo>(FlastType::VOID);
    if (match({TokenType::TOK_POINTER})) return std::make_shared<TypeInfo>(FlastType::REF);
    
    // Self type (for struct methods)
    if (match({TokenType::TOK_SELF_TYPE})) return std::make_shared<TypeInfo>(FlastType::SELF);
    
    // Auto type inference
    if (match({TokenType::TOK_AUTO})) return std::make_shared<TypeInfo>(FlastType::AUTO);
    
    // Parse complex type expressions (e.g., lib.merk.car, std::vector<int>)
    return parseComplexType();
}

std::shared_ptr<TypeInfo> Parser::parseComplexType() {
    // Start with the base identifier
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw parseError("Expected type identifier");
    }
    
    std::string baseName = advance().value;
    auto typeInfo = std::make_shared<TypeInfo>(FlastType::STRUCT, baseName);
    
    // Parse qualified names (e.g., lib.merk.car)
    while (match({TokenType::TOK_DOT})) {
        if (!check(TokenType::TOK_IDENTIFIER)) {
            throw parseError("Expected identifier after '.' in type name");
        }
        std::string qualifier = advance().value;
        typeInfo->className += "." + qualifier;
    }
    
    // Parse generic parameters (e.g., vector<int>, map<string, int>)
    if (match({TokenType::TOK_LESS})) {
        typeInfo->parameters = parseGenericParameters();
        consume(TokenType::TOK_GREATER, "Expected '>' after generic parameters");
    }
    
    // Parse pointer/reference modifiers
    while (true) {
        if (match({TokenType::TOK_MULTIPLY})) {
            typeInfo->isPointer = true;
        } else if (match({TokenType::TOK_REF})) {
            typeInfo->isReference = true;
        } else if (match({TokenType::TOK_CONSTANT})) {
            typeInfo->isConst = true;
        } else {
            break;
        }
    }
    
    // Parse optional type (e.g., option<int>)
    if (match({TokenType::TOK_OPTION})) {
        consume(TokenType::TOK_LESS, "Expected '<' after 'option'");
        auto innerType = parseType();
        consume(TokenType::TOK_GREATER, "Expected '>' after option type");
        typeInfo = std::make_shared<TypeInfo>(FlastType::OPTION);
        typeInfo->parameters.push_back(innerType);
    }
    
    // Parse result type (e.g., result<int, string>)
    if (match({TokenType::TOK_RESULT})) {
        consume(TokenType::TOK_LESS, "Expected '<' after 'result'");
        auto okType = parseType();
        consume(TokenType::TOK_COMMA, "Expected ',' between result types");
        auto errType = parseType();
        consume(TokenType::TOK_GREATER, "Expected '>' after result types");
        typeInfo = std::make_shared<TypeInfo>(FlastType::RESULT);
        typeInfo->parameters.push_back(okType);
        typeInfo->parameters.push_back(errType);
    }
    
    // Parse array types (e.g., array<int, 10>)
    if (match({TokenType::TOK_ARRAY})) {
        consume(TokenType::TOK_LESS, "Expected '<' after 'array'");
        auto elementType = parseType();
        consume(TokenType::TOK_COMMA, "Expected ',' between array type and size");
        
        // Parse array size (can be a number or expression)
        if (check(TokenType::TOK_NUMBER)) {
            std::string sizeStr = advance().value;
            // For now, we'll store the size as a string in className
            // In a full implementation, you'd want to parse this as an expression
            typeInfo = std::make_shared<TypeInfo>(FlastType::ARRAY);
            typeInfo->parameters.push_back(elementType);
            typeInfo->className = sizeStr;
        } else {
            throw parseError("Expected array size");
        }
        consume(TokenType::TOK_GREATER, "Expected '>' after array type");
    }
    
    return typeInfo;
}

std::vector<ParameterAST> Parser::parseParameterList() {
    std::vector<ParameterAST> parameters;
    
    if (!check(TokenType::TOK_RPAREN)) {
        do {
            parameters.push_back(parseParameter());
        } while (match({TokenType::TOK_COMMA}));
    }
    
    return parameters;
}

ParameterAST Parser::parseParameter() {
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw parseError("Expected parameter name");
    }
    
    std::string name = advance().value;
    consume(TokenType::TOK_COLON, "Expected ':' after parameter name");
    
    auto type = parseType();
    
    // Default value
    std::shared_ptr<ExprAST> defaultValue = nullptr;
    if (match({TokenType::TOK_ASSIGN})) {
        defaultValue = parseExpression();
    }
    
    return ParameterAST(name, type, defaultValue);
}

std::vector<std::shared_ptr<TypeInfo>> Parser::parseGenericParameters() {
    std::vector<std::shared_ptr<TypeInfo>> generics;
    
    do {
        generics.push_back(parseType());
    } while (match({TokenType::TOK_COMMA}));
    
    return generics;
}

void Parser::enterFunction(std::shared_ptr<TypeInfo> returnType) {
    inFunction = true;
    currentFunctionReturnType = returnType;
}

void Parser::exitFunction() {
    inFunction = false;
    currentFunctionReturnType = nullptr;
}

void Parser::enterLoop() {
    loopStack.push_back(true);
    inLoop = true;
}

void Parser::exitLoop() {
    if (!loopStack.empty()) {
        loopStack.pop_back();
    }
    inLoop = !loopStack.empty();
}

void Parser::attachSemanticAnalyzer(std::shared_ptr<SemanticAnalyzer> analyzer) {
    semanticAnalyzer = analyzer;
}

// ==================== MISSING IMPLEMENTATIONS ====================

std::shared_ptr<TraitDeclAST> Parser::parseTraitDecl() {
    consume(TokenType::TOK_TRAIT, "Expected 'trait'");
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw parseError("Expected trait name");
    }
    
    std::string name = advance().value;
    
    consume(TokenType::TOK_LBRACE, "Expected '{' after trait name");
    
    std::vector<std::shared_ptr<DeclAST>> methods;
    
    while (!check(TokenType::TOK_RBRACE) && !isAtEnd()) {
        if (check(TokenType::TOK_FUNC)) {
            methods.push_back(parseFunctionDecl());
        } else {
            advance(); // Skip unknown tokens
        }
    }
    
    consume(TokenType::TOK_RBRACE, "Expected '}' after trait methods");
    
    return std::make_shared<TraitDeclAST>(name, methods);
}

std::shared_ptr<ImplDeclAST> Parser::parseImplDecl() {
    consume(TokenType::TOK_IMPL, "Expected 'impl'");
    
    auto targetType = parseType();
    
    consume(TokenType::TOK_LBRACE, "Expected '{' after impl target");
    
    std::vector<std::shared_ptr<DeclAST>> methods;
    
    while (!check(TokenType::TOK_RBRACE) && !isAtEnd()) {
        if (check(TokenType::TOK_FUNC)) {
            methods.push_back(parseFunctionDecl());
        } else {
            advance(); // Skip unknown tokens
        }
    }
    
    consume(TokenType::TOK_RBRACE, "Expected '}' after impl methods");
    
    return std::make_shared<ImplDeclAST>(targetType, methods);
}



std::shared_ptr<ImportDeclAST> Parser::parseImportDecl() {
    advance(); // consume 'import' or 'use'
    
    std::vector<std::string> specificImports;
    std::string alias;
    std::string moduleName;
    bool isWildcard = false;
    
    // Handle "import { func1, func2 } from 'module'" or "import func from 'module'"
    if (check(TokenType::TOK_LBRACE)) {
        // Named imports: import { name1, name2 } from "module"
        advance(); // consume '{'
        
        do {
            if (!check(TokenType::TOK_IDENTIFIER)) {
                throw parseError("Expected import name");
            }
            specificImports.push_back(advance().value);
        } while (match({TokenType::TOK_COMMA}));
        
        consume(TokenType::TOK_RBRACE, "Expected '}' after import list");
        
    } else if (check(TokenType::TOK_MULTIPLY)) {
        // Wildcard import: import * from "module"
        advance(); // consume '*'
        isWildcard = true;
        
    } else if (check(TokenType::TOK_IDENTIFIER)) {
        // Default import: import name from "module"
        std::string importName = advance().value;
        specificImports.push_back(importName);
    }
    
    // Handle "from" keyword
    if (match({TokenType::TOK_FROM}) || match({TokenType::TOK_IDENTIFIER})) {
        // Skip if it's "from" or treat as module name if no "from"
        if (previous().value != "from" && check(TokenType::TOK_STRING)) {
            moduleName = previous().value; // It's actually module name
        }
    }
    
    // Get module path
    if (check(TokenType::TOK_STRING)) {
        moduleName = advance().value;
    } else if (check(TokenType::TOK_IDENTIFIER)) {
        moduleName = advance().value;
    } else if (moduleName.empty()) {
        throw parseError("Expected module path");
    }
    
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after import");
    
    return std::make_shared<ImportDeclAST>(moduleName, alias, specificImports, isWildcard);
}

std::shared_ptr<ModuleDeclAST> Parser::parseModuleDecl() {
    consume(TokenType::TOK_MOD, "Expected 'mod'");
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw parseError("Expected module name");
    }
    
    std::string name = advance().value;
    
    consume(TokenType::TOK_LBRACE, "Expected '{' after module name");
    
    std::vector<std::shared_ptr<DeclAST>> declarations;
    
    while (!check(TokenType::TOK_RBRACE) && !isAtEnd()) {
        if (auto decl = parseDeclaration()) {
            declarations.push_back(decl);
        }
    }
    
    consume(TokenType::TOK_RBRACE, "Expected '}' after module body");
    
    return std::make_shared<ModuleDeclAST>(name, declarations);
}

std::shared_ptr<VarDeclStmtAST> Parser::parseVarDecl() {
    bool isConst = match({TokenType::TOK_CONST});
    
    if (!isConst) {
        consume(TokenType::TOK_LET, "Expected 'let' or 'const'");
    }
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw parseError("Expected variable name");
    }
    
    std::string name = advance().value;
    
    std::shared_ptr<TypeInfo> type = nullptr;
    if (match({TokenType::TOK_COLON})) {
        type = parseType();
    }
    
    std::shared_ptr<ExprAST> initializer = nullptr;
    if (match({TokenType::TOK_ASSIGN})) {
        initializer = parseExpression();
    }
    
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_shared<VarDeclStmtAST>(name, type, initializer, isConst);
}

std::shared_ptr<BlockStmtAST> Parser::parseBlock() {
    consume(TokenType::TOK_LBRACE, "Expected '{'");
    
    std::vector<std::shared_ptr<StmtAST>> statements;
    
    while (!check(TokenType::TOK_RBRACE) && !isAtEnd()) {
        if (check(TokenType::TOK_RETURN)) {
            advance(); // consume 'return'
            auto expr = parseExpression();
            consume(TokenType::TOK_SEMICOLON, "Expected ';' after return");
            statements.push_back(std::make_shared<ReturnStmtAST>(expr));
        } else if (check(TokenType::TOK_LET) || check(TokenType::TOK_CONST)) {
            // Parse variable declaration
            auto varDecl = parseVarDecl();
            statements.push_back(varDecl);
        } else if (check(TokenType::TOK_WHILE)) {
            // Parse while loop
            auto whileStmt = parseWhileStatement();
            statements.push_back(whileStmt);
        } else if (check(TokenType::TOK_FOR)) {
            // Parse for loop (could be C-style or for-in)
            auto forStmt = parseForStatement();
            statements.push_back(forStmt);
        } else {
            // Parse expression statement (like println call)
            auto expr = parseExpression();
            consume(TokenType::TOK_SEMICOLON, "Expected ';' after expression");
            statements.push_back(std::make_shared<ExprStmtAST>(expr));
        }
    }
    
    consume(TokenType::TOK_RBRACE, "Expected '}'");
    
    return std::make_shared<BlockStmtAST>(statements);
}

std::shared_ptr<WhileStmtAST> Parser::parseWhileStatement() {
    consume(TokenType::TOK_WHILE, "Expected 'while'");
    
    // Parse condition (without parentheses - Rust style)
    auto condition = parseExpression();
    
    // Parse body
    auto body = parseBlock();
    
    return std::make_shared<WhileStmtAST>(condition, body);
}

std::shared_ptr<StmtAST> Parser::parseForStatement() {
    consume(TokenType::TOK_FOR, "Expected 'for'");
    
    // Check if this is a for-in loop by looking ahead
    // Syntax: for let variable: type in iterable { ... }
    if (check(TokenType::TOK_LET)) {
        size_t pos = current;
        advance(); // consume 'let'
        
        if (check(TokenType::TOK_IDENTIFIER)) {
            advance(); // consume variable name
            
            // Check for type annotation
            if (match({TokenType::TOK_COLON})) {
                parseType(); // consume type
            }
            
            // Check for 'in' keyword
            if (check(TokenType::TOK_IN)) {
                // Reset and parse as for-in loop
                current = pos;
                return parseForInStatement();
            }
        }
        
        // Reset position and parse as C-style for loop
        current = pos;
    }
    
    // Parse C-style for loop
    // Parse initialization
    std::shared_ptr<StmtAST> init = nullptr;
    if (check(TokenType::TOK_LET) || check(TokenType::TOK_CONST)) {
        init = parseVarDecl();
    } else if (!check(TokenType::TOK_SEMICOLON)) {
        auto expr = parseExpression();
        consume(TokenType::TOK_SEMICOLON, "Expected ';' after for loop init");
        init = std::make_shared<ExprStmtAST>(expr);
    } else {
        advance(); // consume ';' for empty init
    }
    
    // Parse condition
    std::shared_ptr<ExprAST> condition = nullptr;
    if (!check(TokenType::TOK_SEMICOLON)) {
        condition = parseExpression();
    }
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after for loop condition");
    
    // Parse update
    std::shared_ptr<StmtAST> update = nullptr;
    if (!check(TokenType::TOK_LBRACE)) {
        auto expr = parseExpression();
        update = std::make_shared<ExprStmtAST>(expr);
    }
    
    // Parse body
    auto body = parseBlock();
    
    return std::make_shared<ForStmtAST>(init, condition, update, body);
}

std::shared_ptr<ForInStmtAST> Parser::parseForInStatement() {
    // Already consumed 'for', now parse: let variable: type in iterable { body }
    consume(TokenType::TOK_LET, "Expected 'let' in for-in loop");
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        throw parseError("Expected variable name in for-in loop");
    }
    
    std::string variable = advance().value;
    
    // Optional type annotation
    if (match({TokenType::TOK_COLON})) {
        parseType(); // consume type (for now, we'll ignore it)
    }
    
    consume(TokenType::TOK_IN, "Expected 'in' in for-in loop");
    
    // Parse iterable expression
    auto iterable = parseExpression();
    
    // Parse body
    auto body = parseBlock();
    
    return std::make_shared<ForInStmtAST>(variable, iterable, body);
}

void Parser::printErrors() const {
    for (const auto& error : errors) {
        std::cerr << "Parse Error at line " << error.line << ", column " << error.column 
                  << ": " << error.what() << std::endl;
    }
} 