#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::unordered_map<std::string, TokenType> keywords;
    
    void initKeywords();
    char peek(size_t offset = 0);
    char advance();
    void skipWhitespace();
    void skipComment();
    Token readNumber();
    Token readString();
    Token readIdentifier();
    Token readOperator();
    
public:
    Lexer(const std::string& source);
    Token nextToken();
    std::vector<Token> tokenize();
}; 